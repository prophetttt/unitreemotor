#ifndef DASHBOARDPANEL_H
#define DASHBOARDPANEL_H

#include "wx/panel.h"

class DashboardPanel : public wxPanel
{
public:
    DashboardPanel(wxWindow* parent);

private:
    // 初始化左侧数据解析区域 (只读)
    void InitDataParsing(wxSizer* parentSizer);
    // 初始化右侧调试数据输入区域 (可写)
    void InitDebugInput(wxSizer* parentSizer);
};

#endif // DASHBOARDPANEL_H