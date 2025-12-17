#include "ConfigPanel.h"
#include "MainFrame.h" // 引入颜色常量
#include "wx/sizer.h"
#include "wx/stattext.h"
#include "wx/button.h"
#include "wx/textctrl.h"

ConfigPanel::ConfigPanel(wxWindow* parent)
    : wxPanel(parent, wxID_ANY)
{
    // 设置 Panel 背景色，匹配 MainFrame 的 BG_DARK
    SetBackgroundColour(BG_DARK);

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // 标题
    wxStaticText* title = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("● 配置工具"));
    title->SetForegroundColour(TEXT_LIGHT);
    title->SetFont(title->GetFont().Bold());
    mainSizer->Add(title, 0, wxALL, 10);
    
    // 按钮布局区域
    wxPanel* buttonPanel = new wxPanel(this, wxID_ANY);
    buttonPanel->SetBackgroundColour(BG_MID);
    wxFlexGridSizer* buttonSizer = new wxFlexGridSizer(5, 5, 5); // 5列，间距5

    auto CreateConfigButton = [&](const wxString& label) {
        wxButton* btn = new wxButton(buttonPanel, wxID_ANY, label);
        btn->SetForegroundColour(ACCENT_BLUE); // 蓝色文字
        // 很难在所有平台上强制实现“蓝色边框”的幽灵按钮效果，这里保持默认背景
        return btn;
    };
    
    // 第1行: 查询/修改/输入
    buttonSizer->Add(CreateConfigButton(wxString::FromUTF8("查询ID")), 0, wxEXPAND);
    buttonSizer->Add(CreateConfigButton(wxString::FromUTF8("修改ID")), 0, wxEXPAND);
    buttonSizer->Add(new wxTextCtrl(buttonPanel, wxID_ANY, wxString::FromUTF8("0"), wxDefaultPosition, wxSize(60, -1)), 0, wxEXPAND);
    buttonSizer->Add(CreateConfigButton(wxString::FromUTF8("恢复电机模式")), 0, wxEXPAND);
    buttonSizer->Add(CreateConfigButton(wxString::FromUTF8("查询版本号")), 0, wxEXPAND);

    // 第2行: 自动引导/固件更新
    buttonSizer->Add(CreateConfigButton(wxString::FromUTF8("开启自动引导")), 0, wxEXPAND);
    buttonSizer->Add(CreateConfigButton(wxString::FromUTF8("关闭自动引导")), 0, wxEXPAND);
    buttonSizer->Add(new wxStaticText(buttonPanel, wxID_ANY, wxString::FromUTF8("")), 0, wxEXPAND); // 占位
    buttonSizer->Add(CreateConfigButton(wxString::FromUTF8("临时自动引导")), 0, wxEXPAND);
    buttonSizer->Add(CreateConfigButton(wxString::FromUTF8("退出自动引导")), 0, wxEXPAND);
    
    // 第3行: 更新固件
    buttonSizer->Add(CreateConfigButton(wxString::FromUTF8("点击上传")), 0, wxEXPAND);
    buttonSizer->Add(CreateConfigButton(wxString::FromUTF8("更新固件")), 0, wxEXPAND);

    buttonPanel->SetSizerAndFit(buttonSizer);

    mainSizer->Add(buttonPanel, 1, wxEXPAND | wxALL, 5);

    SetSizerAndFit(mainSizer);
}