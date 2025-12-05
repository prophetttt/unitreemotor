#include "MainFrame.h"
#include "wx/sizer.h"
#include "wx/button.h"
#include "wx/stattext.h"
#include "wx/choice.h"
#include <iostream>

#include "DashboardPanel.h" 
#include "ConfigPanel.h"

// MainFrame 实现
MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame(NULL, wxID_ANY, title, pos, size)
{
    // 1. 设置主窗口背景为深色
    // SetBackgroundColour(BG_DARK);

    // 在 Linux GNOME 下，显式设置常用字体，确保内容可见
    wxFont defaultFont(12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "DejaVu Sans");
    if (!defaultFont.IsOk()) {
        // 备选字体
        defaultFont = wxFont(12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Liberation Sans");
    }
    SetFont(defaultFont);
    std::cout << "Default font set: " << defaultFont.GetFaceName().ToStdString() << std::endl;

    // 递归设置所有子控件字体
    std::function<void(wxWindow*, const wxFont&)> SetFontRecursive = [&](wxWindow* win, const wxFont& font) {
        if (win) win->SetFont(font);
        const wxWindowList& children = win->GetChildren();
        for (wxWindowList::const_iterator it = children.begin(); it != children.end(); ++it) {
            wxWindow* childWin = dynamic_cast<wxWindow*>(*it);
            if (childWin) {
                SetFontRecursive(childWin, font);
            }
        }
    };
    SetFontRecursive(this, defaultFont);

    // 2. 创建主 Sizer (垂直布局)
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // 3. 初始化顶部控制栏
    InitTopBar(mainSizer);

    // 4. 初始化中部面板 (Dashboard)
    m_dashboardPanel = new DashboardPanel(this);
    mainSizer->Add(m_dashboardPanel, 1, wxEXPAND | wxALL, 10); // 占用垂直空间

    // 5. 初始化底部配置面板 (Config)
    m_configPanel = new ConfigPanel(this);
    mainSizer->Add(m_configPanel, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

    // 6. 应用 Sizer
    SetSizer(mainSizer);
    // Center(); // 窗口居中
}

void MainFrame::InitTopBar(wxSizer* parentSizer)
{
    wxPanel* topBar = new wxPanel(this, wxID_ANY);
    topBar->SetBackgroundColour(TEXT_LIGHT);
    // 继承主窗口字体，确保字体可见
    topBar->SetFont(GetFont());
    wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);

    // --- 左侧：下拉框 ---
    wxArrayString motors, ports;
    motors.Add("GO-M8010-6"); motors.Add("M-100");
    ports.Add("USB Serial Port (COM3)"); ports.Add("CAN Port");

    topSizer->Add(new wxChoice(topBar, wxID_ANY, wxDefaultPosition, wxDefaultSize, motors), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    topSizer->Add(new wxChoice(topBar, wxID_ANY, wxDefaultPosition, wxDefaultSize, ports), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    
    // --- 中间：状态显示 ---
    auto CreateStatusLabel = [&](const wxString& text) {
        wxStaticText* label = new wxStaticText(topBar, wxID_ANY, text);
        label->SetForegroundColour(TEXT_LIGHT);
        label->SetFont(GetFont());
        return label;
    };

    topSizer->AddStretchSpacer(1); // 弹性空间

    wxString recive("接收: 0Hz");

    topSizer->Add(CreateStatusLabel(recive), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    topSizer->Add(CreateStatusLabel(recive), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    topSizer->Add(CreateStatusLabel(recive), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    topSizer->Add(CreateStatusLabel(recive), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    
    topSizer->AddStretchSpacer(1); // 弹性空间

    // --- 右侧：开始按钮 ---
    wxButton* startButton = new wxButton(topBar, wxID_ANY, "开始调试");
    // 尝试设置蓝色背景 (在某些系统上 wxButton 的颜色定制能力有限)
    startButton->SetBackgroundColour(ACCENT_BLUE);
    startButton->SetForegroundColour(*wxWHITE);
    startButton->SetMinSize(wxSize(100, -1));

    topSizer->Add(startButton, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

    topBar->SetSizerAndFit(topSizer);
    parentSizer->Add(topBar, 0, wxEXPAND | wxALL, 5);
}

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    // 如果您需要处理事件，在这里添加事件处理器，例如:
    // EVT_BUTTON(wxID_ANY, MainFrame::OnStartDebug) 
    // 目前保持为空即可通过编译
wxEND_EVENT_TABLE()