#include "wx/app.h"
#include "MainFrame.h"
#include "serial_initalize.h"

// ============================================================================
// 定义应用程序类
// ============================================================================
class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

// ============================================================================
// 程序的实际入口点
// ============================================================================
wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    // 1. 创建主窗口实例
    MainFrame* frame = new MainFrame(
        wxString::FromUTF8("Unitree 电机调试助手 (Demo)"),
        wxPoint(50, 50),
        wxSize(800, 700)
    );

    // 2. 显示主窗口
    frame->Show(true);

    return true;
}