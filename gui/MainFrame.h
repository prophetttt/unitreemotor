#ifndef MAINFRAME_H
#define MAINFRAME_H

#include "wx/frame.h"
#include "DashboardPanel.h"
#include "ConfigPanel.h"



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
    // 顶部控制区域
    void InitTopBar(wxSizer* parentSizer);

    // 中部数据区
    DashboardPanel* m_dashboardPanel;
    // 底部配置区
    ConfigPanel* m_configPanel;

    
};

#endif // MAINFRAME_H