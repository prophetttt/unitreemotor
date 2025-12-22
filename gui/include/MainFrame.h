#ifndef MAINFRAME_H
#define MAINFRAME_H

#include "wx/frame.h"
#include "ConfigPanel.h"
#include "serial_initalize.h"
#include "coreThread.h"
#include "unitreeMotor.h"




// === 颜色常量定义 (为了简化，放在头文件中) ===
// 主背景色 (黑色)
const wxColour BG_DARK = wxColour(30, 30, 30); 
// 分组面板背景色 (深灰)
const wxColour BG_MID = wxColour(37, 37, 38); 
// 浅色文字
const wxColour TEXT_LIGHT = wxColour(204, 204, 204); 
// 强调色 (蓝色)
const wxColour ACCENT_BLUE = wxColour(0, 136, 204);

class MainFrame : public wxFrame
{
public:
    MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

private:
    void InitMenuBar();
    // 顶部控制区域
    void InitTopBar(wxSizer* parentSizer);

    void InitDebugPanel(wxSizer* parentSizer);
    void InitConfigPanel(wxSizer* parentSizer);
    // 底部配置区
    ConfigPanel* m_configPanel;

    //控件ID
    enum {
        ID_VIEW_DEBUG = 10001,
        ID_VIEW_CONFIG
    };

    // void OnToggleView(wxCommandEvent& event) {
    //     bool isChecked = event.IsChecked();
        
    //     if (event.GetId() == ID_VIEW_DEBUG) {
    //         // 核心方法：Show(item, bool)
    //         mainSizer->Show(m_dashboardPanel, isChecked);
    //     } 
    //     else if (event.GetId() == ID_VIEW_CONFIG) {
    //         mainSizer->Show(m_configPanel, isChecked);
    //     }

    //     // 必须调用 Layout()，否则界面不会实时刷新
    //     this->Layout();
    // }

    // 后台线程
    // motorBackend* backendThread;
    
};

#endif // MAINFRAME_H