
// Start of wxWidgets "Hello World" Program
#include "MyApp.hpp"
#include "MyFrame.h"
#include <iostream>

//this class will give us the entrypoint method
//we pass it to wxIMPLEMENT_APP
//class extends wxApp

wxIMPLEMENT_APP(MyApp); 

bool MyApp::OnInit()
{
    std::cout<<"we not runnin!!!\n";
    MyFrame *frame = new MyFrame();
    frame->SetIcon(wxICON(IDI_ICON1));
    return true;
}


