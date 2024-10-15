#include "MyFrame.h"
#include <fstream>
#include "MyApp.hpp"
#include "Panels.h"

topPanel *tp; 
MyTaskBar* tBar;

MyFrame::MyFrame()
    : wxFrame(nullptr, wxID_ANY, "Power Sound Do-er", wxDefaultPosition, wxSize(400, 230), wxDEFAULT_FRAME_STYLE),
    fileD(this, _("Open WAV file"), "", "","WAV files (*.wav)|*.wav", wxFD_OPEN|wxFD_FILE_MUST_EXIST)
{   

    //menu stuff
    menuItems();
    OnStartUp();
    //the parent panel to the two panels
    tp = new topPanel(this);
    // we should be able to show up to 128 characters on Windows
    
    wxBoxSizer *hbox = new wxBoxSizer(wxVERTICAL);
    hbox->Add(tp, 1, wxEXPAND);

    Bind(wxEVT_CLOSE_WINDOW, &MyFrame::OnClose, this);

    SetSizer(hbox);

    this->Center();
    Show(false);

    tBar = new MyTaskBar(this); 
    if (!tBar->SetIcon(wxICON(IDI_ICON1),
                                 "Power Sound Do-er"
                                 ) )
    {
        wxLogError("Could not set icon.");
    }
    
}

MyFrame::~MyFrame(){
    tBar->Destroy();
}

void MyFrame::menuItems(){


    std::unique_ptr<wxMenu> menuFile(new wxMenu);
    menuFile->Append(wxID_OPEN, wxT("Add"));
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);
    
    std::unique_ptr<wxMenu> menuHelp(new wxMenu);
    menuHelp->Append(wxID_HELP);
    menuHelp->AppendSeparator();
    menuHelp->Append(wxID_ABOUT);
 
    std::unique_ptr<wxMenuBar> menuBar(new wxMenuBar);
    menuBar->Append(menuFile.release(), "&File");
    menuBar->Append(menuHelp.release(), "&Help");
    
    SetMenuBar( menuBar.release() );
 
    CreateStatusBar();
    SetStatusText("HI!!");

    Bind(wxEVT_MENU, &MyFrame::OnOpen, this, wxID_OPEN);
    Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MyFrame::OnHellp, this, wxID_HELP);
    Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);

}

void MyFrame::OnOpen(wxCommandEvent& event){
    printf("open file\n");
        if(fileD.ShowModal() == wxID_OK){
            tp->addSong(fileD.GetPath());
        }
}

void MyFrame::OnClose(wxCloseEvent& event){

    if(weLeaving){
        event.Skip();
    }else{
        event.Veto();
        this->Show(false);
    }

}


void MyFrame::OnExit(wxCommandEvent& event)
{
    weLeaving = true;
    Close(true);
}
 
void MyFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("An app that allows you to set a tone for Plugging In or Out\nyour computer.\n\nVersion: 1.0.0\nUsed: wxWidgets, SFML(for sound), c++",
                 "About PSD", wxOK | wxICON_INFORMATION);
}

void MyFrame::OnHellp(wxCommandEvent& event){
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string exePath(buffer);
    std::string path = exePath.substr(0, exePath.find_last_of("\\"));

    std::string command = "start " + path + "\\README.txt";
    system(command.c_str());
}

//this is a custom method 
//what happens when you interect with something bound to
//it
void MyFrame::OnHello(wxCommandEvent& event)
{
    wxLogMessage("Hello world from wxWidgets!");
}

void MyFrame::OnStartUp(){

    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string exePath(buffer);
    std::string path = exePath.substr(0, exePath.find_last_of("\\"));

    wxRegKey key(wxRegKey::HKCU, "Software\\Microsoft\\Windows\\CurrentVersion\\Run");
    key.SetValue("PSDApp", path + "\\PSD.exe");
    if(key.Exists()){
        printf("the key exists!\n");
    }
}


//MY TASKBAR STUFF//
enum
{
    PU_RESTORE = 10001,
    PU_EXIT,
    PU_ABOUT,
};

MyTaskBar::MyTaskBar(MyFrame* frame) : pFrame(frame){
}

wxBEGIN_EVENT_TABLE(MyTaskBar, wxTaskBarIcon)
    EVT_MENU(PU_RESTORE, MyTaskBar::OnMenuRestore)
    EVT_MENU(PU_EXIT,    MyTaskBar::OnMenuExit)
    EVT_MENU(PU_ABOUT,MyTaskBar::OnMenuAbout)
    EVT_TASKBAR_LEFT_DOWN  (MyTaskBar::OnLeftButtonClick)
    EVT_TASKBAR_RIGHT_DOWN (MyTaskBar::OnRightButtonClick)
wxEND_EVENT_TABLE()


void MyTaskBar::OnMenuRestore(wxCommandEvent& )
{
    printf("on show");
    pFrame->Show(true);
}

void MyTaskBar::OnMenuExit(wxCommandEvent& )
{
    pFrame->weLeaving = true;
    pFrame->Close(true);
    //mthis->Destroy();
}

void MyTaskBar::OnMenuAbout(wxCommandEvent& )
{
    if(!pFrame->IsShown()){
        pFrame->Show(true);
        wxMessageBox("An app that allows you to set a tone for\nPlugging In or Out your computer.",
                    "About PSD", wxOK | wxICON_INFORMATION);
    }
}


// Overridables
wxMenu *MyTaskBar::CreatePopupMenu()
{
    wxMenu *menu = new wxMenu;
    menu->Append(PU_RESTORE, "&Show main window");
    menu->AppendSeparator();
    menu->Append(PU_ABOUT, "&About PSD");
    /* OSX has built-in quit menu for the dock menu, but not for the status item */
#ifdef __WXOSX__
    if ( OSXIsStatusItem() )
#endif
    {
        menu->AppendSeparator();
        menu->Append(PU_EXIT,    "E&xit");
    }
    return menu;
}

void MyTaskBar::OnLeftButtonClick(wxTaskBarIconEvent&)
{
    pFrame->Show(true);
}

void MyTaskBar::OnRightButtonClick(wxTaskBarIconEvent&)
{
    printf("button something");
}