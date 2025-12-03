#include "DashboardPanel.h"
#include "MainFrame.h" // 引入颜色常量
#include "wx/sizer.h"
#include "wx/stattext.h"
#include "wx/textctrl.h"

DashboardPanel::DashboardPanel(wxWindow* parent)
    : wxPanel(parent, wxID_ANY)
{
    // 设置 Panel 背景色，匹配 MainFrame 的 BG_DARK
    SetBackgroundColour(BG_DARK);

    // 主 Sizer：水平布局，用于容纳左右两部分
    wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);

    // 初始化左右两侧，并添加到主 Sizer
    InitDataParsing(mainSizer);
    InitDebugInput(mainSizer);

    SetSizerAndFit(mainSizer);
}

void DashboardPanel::InitDataParsing(wxSizer* parentSizer)
{
    wxPanel* panel = new wxPanel(this, wxID_ANY);
    panel->SetBackgroundColour(BG_MID);

    wxBoxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);

    // 标题
    wxStaticText* title = new wxStaticText(panel, wxID_ANY, "● 数据解析");
    title->SetForegroundColour(TEXT_LIGHT);
    title->SetFont(title->GetFont().Bold());
    panelSizer->Add(title, 0, wxALL, 10);

    // 网格布局 (标签: 值)
    wxFlexGridSizer* gridSizer = new wxFlexGridSizer(2, 20, 10); // 2列，水平/垂直间距

    auto AddReadOnlyField = [&](const wxString& labelText, const wxString& valueText) {
        wxStaticText* label = new wxStaticText(panel, wxID_ANY, labelText);
        label->SetForegroundColour(TEXT_LIGHT);
        wxStaticText* value = new wxStaticText(panel, wxID_ANY, valueText);
        value->SetForegroundColour(ACCENT_BLUE); // 数据值用蓝色
        gridSizer->Add(label, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        gridSizer->Add(value, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
    };

    AddReadOnlyField("电机ID:", "0");
    AddReadOnlyField("转子位置:", "0.00");
    AddReadOnlyField("电机模式:", "0");
    AddReadOnlyField("电机温度:", "正常");
    AddReadOnlyField("转子扭矩:", "0.00");
    AddReadOnlyField("错误状态:", "正常");
    AddReadOnlyField("转子速度:", "0.00");
    AddReadOnlyField("传感器:", "0");

    panelSizer->Add(gridSizer, 1, wxEXPAND | wxALL, 10);
    panel->SetSizerAndFit(panelSizer);
    
    // 添加到主 Sizer，权重为1，使其平分宽度
    parentSizer->Add(panel, 1, wxEXPAND | wxALL, 5); 
}

void DashboardPanel::InitDebugInput(wxSizer* parentSizer)
{
    wxPanel* panel = new wxPanel(this, wxID_ANY);
    panel->SetBackgroundColour(BG_MID);
    
    wxBoxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);

    // 标题
    wxStaticText* title = new wxStaticText(panel, wxID_ANY, "● 调试数据输入");
    title->SetForegroundColour(TEXT_LIGHT);
    title->SetFont(title->GetFont().Bold());
    panelSizer->Add(title, 0, wxALL, 10);

    // 网格布局 (标签: 输入框)
    wxFlexGridSizer* gridSizer = new wxFlexGridSizer(2, 20, 10); // 2列，水平/垂直间距

    auto AddInputField = [&](const wxString& labelText, const wxString& placeholder) {
        wxStaticText* label = new wxStaticText(panel, wxID_ANY, labelText);
        label->SetForegroundColour(TEXT_LIGHT);
        wxTextCtrl* input = new wxTextCtrl(panel, wxID_ANY, placeholder, wxDefaultPosition, wxSize(120, -1));
        
        // 尝试深色输入框样式
        input->SetBackgroundColour(BG_DARK); 
        input->SetForegroundColour(TEXT_LIGHT); 

        gridSizer->Add(label, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        gridSizer->Add(input, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
    };
    
    AddInputField("电机ID(0-15):", "0");
    AddInputField("转子位置(rad):", "0");
    AddInputField("电机模式:", "0");
    AddInputField("位置kp:", "0");
    AddInputField("转子扭矩(Nm):", "0");
    AddInputField("速度kd:", "0");
    AddInputField("转子速度(rad/s):", "0");
    AddInputField("通信频率(Hz):", "1000");

    panelSizer->Add(gridSizer, 1, wxEXPAND | wxALL, 10);
    panel->SetSizerAndFit(panelSizer);

    parentSizer->Add(panel, 1, wxEXPAND | wxALL, 5);
}