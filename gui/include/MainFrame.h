#ifndef MAINFRAME_H
#define MAINFRAME_H

#include "wx/frame.h"
#include <wx/button.h>
#include "serial_initalize.h"
#include "coreThread.h"
#include "unitreeMotor.h"
#include "wx/colour.h"
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/tglbtn.h>
#include <map>
#include "motorBackend.h"
#include "wx/combobox.h"

#define CALLBACK


class MainFrame : public wxFrame
{
public:
    MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
    
private:
    // initialize menubar
    void InitMenuBar();
    // initialize top serial port control bar
    void InitTopBar(wxSizer* parentSizer);
    void InitDebugPanel(wxSizer* parentSizer);
    void InitConfigPanel(wxSizer* parentSizer);

/*************** Bind(frontend data to backend) *********************/
    // void OnDebugBackendUpdate(wxCommandEvent& event);
    // void OnConfigBackendUpdate(wxCommandEvent& event);

  /*************** Bind(backend data to frontend) *********************/
    // 数据显示与输入控件映射

    CALLBACK void UpdateDataDisplay(InformationEvent& event);
    void UpdateDataDisplayNone();

    std::map<wxString, wxStaticText*> m_dataDisplays;
    std::map<wxString, wxTextCtrl*> m_dataInputs;
    

    // 后台线程
    MotorBackend backendThread;


    wxToggleButton *startDebugButton;
    wxToggleButton *startConfigButton;

    //pointer to enable/disable panelbuttom
    wxButton *sendDebugButton;
    wxButton *sendConfigButton;


    wxComboBox *portBox ;
};

#endif // MAINFRAME_H