#pragma once
#include <wx/wx.h>
#include <wx/power.h>
#include <wx/spinctrl.h>
#include <unordered_set>
#include <iostream>
#include <future> 
#include <atomic>


class topPanel : public wxPanel{


    public:        
        std::mutex m;
        std::atomic<bool> itemFound{false};
        std::atomic<bool> isChanged{false};
        std::atomic<bool> isPlugged{true};
        std::atomic<bool> isRemoved{true};
        std::atomic<bool> weLeaving{false};

        const char* dbname;
        wxString songPath;
        std::vector<std::string> info;
        std::unordered_set<std::string> addedTitles;
        std::shared_ptr<wxComboBox> onBPicker;
        std::shared_ptr<wxComboBox> onSPicker;

        //CONSTRUCTOR
        topPanel(wxFrame* parent);
        ~topPanel();
        void addSong(wxString song);
        void playOut();
        void playIn();
        std::string getSongPath(int type);
    private:
        void guiStuff();
        void OnPlayB1(wxCommandEvent&);
        void OnPlayB2(wxCommandEvent&);
        void OnApplyB(wxCommandEvent&);
};

enum{
    ID_PLAYB1 = 1,
    ID_PLAYB2 = 2,
    ID_APPLYB3 = 3,
    ID_Combo = 4
};