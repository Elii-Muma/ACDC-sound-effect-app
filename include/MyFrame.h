#pragma once
#include <wx/wx.h>
#include <wx/msw/registry.h>
#include <wx/utils.h>
#include <wx/wxprec.h>
#include <wx/taskbar.h>
#include <atomic>
#include <fstream>
#include <wx/artprov.h>


//wxFrame is the main window we use 
//we extend it 
//with custom class
class MyFrame : public wxFrame
{
public:
    std::atomic<bool> weLeaving{false};
    wxFileDialog fileD;

    MyFrame();
    ~MyFrame();
 
private:
    //these take in an event
    void OnHello(wxCommandEvent& event);
    void menuItems();
    void OnStartUp();
    void OnOpen(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnHellp(wxCommandEvent& event);
};


//unique ID that allows us to react to a menu command

enum
{
    ID_Hello = 1,
};


class MyTaskBar : public wxTaskBarIcon{

    public:
        MyTaskBar(MyFrame* frame);
        void OnLeftButtonClick(wxTaskBarIconEvent&);
        void OnRightButtonClick(wxTaskBarIconEvent&);
        void OnMenuRestore(wxCommandEvent&);
        void OnMenuExit(wxCommandEvent&);
        void OnMenuAbout(wxCommandEvent&);
        virtual wxMenu *CreatePopupMenu() wxOVERRIDE;

        wxDECLARE_EVENT_TABLE();
    
    private:
        MyFrame *pFrame;
};