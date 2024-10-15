#pragma once
#include <wx/wx.h>
#include <wx/msw/registry.h>
#include "wx/taskbar.h"

class MyApp : public wxApp
{
public:
    bool OnInit() override;
};
 
wxDECLARE_APP(MyApp);

