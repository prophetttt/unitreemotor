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

    // 设置 Noto Sans CJK 字体以支持中文字符显示
    wxFont defaultFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxString::FromUTF8("Noto Sans CJK SC"));
    if (!defaultFont.IsOk()) {
        // 备选字体：尝试其他 Noto Sans CJK 变体
        defaultFont = wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxString::FromUTF8("Noto Sans CJK TC"));
    }
    if (!defaultFont.IsOk()) {
        // 再次尝试通用名称
        defaultFont = wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxString::FromUTF8("Noto Sans CJK"));
    }
    if (!defaultFont.IsOk()) {
        // 最后备选：使用系统默认的 CJK 字体
        defaultFont = wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    }
    SetFont(defaultFont);
    std::cout << "Default font set: " << defaultFont.GetFaceName().ToStdString() << std::endl;

    

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
    motors.Add(wxString::FromUTF8("GO-M8010-6")); motors.Add(wxString::FromUTF8("M-100"));
    ports.Add(wxString::FromUTF8("USB Serial Port (COM3)")); ports.Add(wxString::FromUTF8("CAN Port"));

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

    wxString recive(wxString::FromUTF8("接收: 0Hz"));

    topSizer->Add(CreateStatusLabel(recive), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    topSizer->Add(CreateStatusLabel(recive), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    topSizer->Add(CreateStatusLabel(recive), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    topSizer->Add(CreateStatusLabel(recive), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    
    topSizer->AddStretchSpacer(1); // 弹性空间

    // --- 右侧：开始按钮 ---
    wxButton* startButton = new wxButton(topBar, wxID_ANY, wxString::FromUTF8("开始调试"));
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