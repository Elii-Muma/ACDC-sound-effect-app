#include "Panels.h"
#include "MyFrame.h"
#include "TheDatabase.hpp"
#include "ThreadPool.hpp"
#include "SoundManager.hpp"
#include "MyApp.hpp"
#include <string>


Database *db;
SPlayer player;
MyPool pool(8);

topPanel::topPanel(wxFrame* parent) : wxPanel(parent){

    this->SetBackgroundColour(wxColor(100, 100, 110));
    guiStuff();

    pool.enqueue([this]{

        dbname = "/database/ToneDetails.db";
        db->createDb(dbname);
        db->createTable(dbname);
        info = db->selectData(dbname, "TITLE");
        for(int i=0; i<info.size(); i++){
            addedTitles.insert(info[i]);
        }

        if(!info.empty()){
            //add songs in info to the pickers
            for(int i=0; i<info.size(); i++){
                onBPicker->Append(info[i]);
                onSPicker->Append(info[i]);
            }

            std::string t1 = db->initSelect(dbname, "CHOSEN", "PLUGGEDOUT");
            std::string t2 = db->initSelect(dbname, "CHOSEN", "PLUGGEDIN");
            std::string t3 = db->initSelect(dbname, "CHOSEN", "BOTH");

            if(!t3.empty()){
                onSPicker->SetStringSelection(t3);
                onBPicker->SetStringSelection(t3);
            }else
                printf("no same tone\n");

            if(!t1.empty()){
                onBPicker->SetStringSelection(t1);

            }else
                printf("no in tone\n");

            if(!t2.empty()){
                onSPicker->SetStringSelection(t2);

            }else
                printf("no out tone\n");

        }else{
            //LOAD THE DEFAULT SOUNDS
            //GET SOUNDS FROM GETEXECUTABLE PATH + RES//SOUND EFFECTS *.wav ?? can that work
            //or load them one by one by name?
        }

    });

    pool.enqueue([this]{
        while(!weLeaving){
            const wxPowerType pt = wxGetPowerType();

            if(pt == wxPOWER_SOCKET){
                if(isPlugged){
                    printf("plugged!\n");
                    playIn();
                    isRemoved = true;
                    isPlugged = false;
                }
            }

            if(pt == wxPOWER_BATTERY){
                if(isRemoved){
                    printf("unPlugged!\n");
                    playOut();
                    isPlugged = true;
                    isRemoved = false;
                }
            }
            
            sf::sleep(sf::milliseconds(800));
        }
    });


}

topPanel::~topPanel(){
    weLeaving = true;
}

void topPanel::guiStuff(){
    
    //the lis thingy
    wxStaticText *poTone = new wxStaticText(this, wxID_ANY, "PLUGGED OUT", wxPoint(55, 5));
    poTone->SetFont(wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    poTone->SetForegroundColour(wxColor(255, 255, 255));
    onBPicker = std::make_shared<wxComboBox>(this, wxID_ANY, "PlugIn Tone", wxPoint(55, 25), wxSize(300, 50));

    wxStaticText *piTone = new wxStaticText(this, wxID_ANY, "PLUGGED IN", wxPoint(55, 55));
    piTone->SetFont(wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    piTone->SetForegroundColour(wxColor(255, 255, 255));
    onSPicker = std::make_shared<wxComboBox>(this, wxID_ANY, "PlugOut Tone", wxPoint(55, 75), wxSize(300, 50));

    wxButton *playBTone = new wxButton(this, ID_PLAYB1, "play", wxPoint(10, 25), wxSize(40, 23));
    playBTone->Bind(wxEVT_BUTTON, &topPanel::OnPlayB1, this);
    wxButton *playSTone = new wxButton(this, ID_PLAYB2, "play", wxPoint(10, 75), wxSize(40, 23));
    playSTone->Bind(wxEVT_BUTTON, &topPanel::OnPlayB2, this);


    wxButton *applyTone = new wxButton(this, ID_APPLYB3, "apply", wxPoint(10, 110));
    applyTone->Bind(wxEVT_BUTTON, &topPanel::OnApplyB, this);


}

void topPanel::OnApplyB(wxCommandEvent&){
    //here we'll get the selected song and set it as selected in the table
    //then this is what will be playing
    
    //get the currently selected files
    //itll be their titles
    pool.enqueue([this]{
            db->updateToUnchosen(dbname);
            
            //we have the retrival run on the main gui thread
            std::string bStr(onBPicker->GetStringSelection().mb_str());
            std::string sStr(onSPicker->GetStringSelection().mb_str());

            //pass their titles to the query thingy
            db->updateToChosen(dbname, bStr, sStr);
            db->updateToNone(dbname);
            if(bStr == sStr){
                printf("same tone for both\n");
                db->updatePowerType(dbname, "BOTH", bStr);
            }else{
                db->updatePowerType(dbname, "PLUGGEDOUT", bStr);
                db->updatePowerType(dbname, "PLUGGEDIN", sStr);
            }
    });
    
}

void topPanel::OnPlayB1(wxCommandEvent&){
    //play song thats chosen by pluggedout
        playOut();
}

void topPanel::OnPlayB2(wxCommandEvent&){
    //play song thats chosen by pluggedin
        playIn();
}

void topPanel::addSong(wxString song){

    songPath = song;
    
    pool.enqueue([this]{
            
        //no other thingy is using this
        bool itemFound = false;
        std::string path(songPath.mb_str());

        std::cout<< "path is: " << path << std::endl;
        std::string title = std::filesystem::path(path).filename().string(); 
        //add title to set

        //try to add the song to the list
        //dont know why but it doesnt work as intended when i remove the guard
        int index;
        {
            std::lock_guard lock(m);
            info = db->selectData(dbname, "TITLE");
        }

        //if the list isnt empty
        if(info.size() != 0){

            if(addedTitles.find(title) == addedTitles.end()){
                addedTitles.insert(title);
                printf("item not found!\n");
            }else{
                itemFound = true;
                printf("item found\n");
            }
            
            //if we didnt find the item, add the last item
            if(!itemFound){
                //add song to the db
                {
                    std::lock_guard lock(m);
                    db->insertData(dbname, path, title, "UNCHOSEN", "NONE");
                    //update the array
                    info = db->selectData(dbname, "TITLE");
                    onBPicker->Append(info.back());
                    onSPicker->Append(info.back());
                }
            }

        }else{//list is empty

            db->insertData(dbname, path, title, "UNCHOSEN", "NONE");
            info = db->selectData(dbname, "TITLE");

                {
                    printf("list is empty, adding %s\n", title.c_str());
                    std::lock_guard lock(m);
                    onBPicker->Append(info.back());
                    onSPicker->Append(info.back());
                    addedTitles.insert(title);
                }
        }
        printf("done adding\n");
    });
}

void topPanel::playOut(){

    pool.enqueue([this]{
        sf::SoundBuffer buffer1;
        buffer1.loadFromFile(getSongPath(2).c_str());
        player.playS2(buffer1);
    });

}

void topPanel::playIn(){

        pool.enqueue([this]{
            sf::SoundBuffer buffer2;
            buffer2.loadFromFile(getSongPath(1).c_str());
            player.playS1(buffer2);
        });

}


std::string topPanel::getSongPath(int type){

    if(!info.empty()){
        std::string p1 = db->selectPath(dbname, "PLUGGEDIN"); 
        std::string p2 = db->selectPath(dbname, "PLUGGEDOUT"); 
        std::string p3 = db->selectPath(dbname, "BOTH"); 

        //get titles selected
        if(type == 1 && !p1.empty()){
            return p1;
        }

        if(type == 2 && !p2.empty()){
            return p2;
        }

        if((type == 1 || type == 2) && !p3.empty()){
            return p3;
        }

    }

    return " ";
}


