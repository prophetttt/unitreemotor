#include "MainFrame.h"
#include "wx/sizer.h"
#include "wx/button.h"
#include "wx/stattext.h"
#include "wx/choice.h"
#include "wx/menu.h"
#include "wx/combobox.h"
#include <iostream>
#include "serial_initalize.h"
#include "commandEvent.h"

// MainFrame 实现
MainFrame::MainFrame(const wxString &title, const wxPoint &pos, const wxSize &size)
    : wxFrame(NULL, wxID_ANY, title, pos, size)
{
    this->SetBackgroundColour(wxColour(18, 18, 20));
    wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
    InitMenuBar();
    // 3. 初始化顶部控制栏
    InitTopBar(mainSizer);

    // 初始化并排的Panel(控制 && 数据回显)
    InitDebugPanel(mainSizer);
    // 4. 初始化中部面板 (Dashboard)

    InitConfigPanel(mainSizer);
    // m_dashboardPanel = new DashboardPanel(this);
    // mainSizer->Add(m_dashboardPanel, 1, wxEXPAND | wxALL, 10); // 占用垂直空间

    // // 5. 初始化底部配置面板 (Config)
    // m_configPanel = new ConfigPanel(this);
    // mainSizer->Add(m_configPanel, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

    // mainSizer->Show(m_configPanel, false);

    /*************** Bind(frontend data to backend) *********************/
    // this->Bind(MOTOR_DATA, &MyFrame::OnBackendUpdate, this);

    // 6. 应用 Sizer
    SetSizer(mainSizer);
    Center(); // 窗口居中
}

void MainFrame::InitDebugPanel(wxSizer *parentSizer)
{
    // 1. 创建左侧：数据解析面板
    wxPanel* leftPanel = new wxPanel(this, wxID_ANY);
    // leftPanel->SetBackgroundColour(BG_MID); // 如果定义了颜色宏可以开启

    wxBoxSizer* leftPanelSizer = new wxBoxSizer(wxVERTICAL);

    // 标题
    wxStaticText* leftTitle = new wxStaticText(leftPanel, wxID_ANY, wxString::FromUTF8("● 数据解析"));
    leftTitle->SetFont(leftTitle->GetFont().Bold());
    leftPanelSizer->Add(leftTitle, 0, wxALL, 10);

    // 网格布局 (2列, 间距10)
    wxFlexGridSizer* leftGrid = new wxFlexGridSizer(2, 10, 10); 

    auto AddReadOnlyField = [&](const wxString& labelText, const wxString& valueText) {
        wxStaticText* label = new wxStaticText(leftPanel, wxID_ANY, labelText);
        wxStaticText* value = new wxStaticText(leftPanel, wxID_ANY, valueText);
        // value->SetForegroundColour(ACCENT_BLUE); // 数据值建议用不同颜色区分
        leftGrid->Add(label, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        leftGrid->Add(value, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
    };

    AddReadOnlyField(wxString::FromUTF8("电机ID:"), wxString::FromUTF8("0"));
    AddReadOnlyField(wxString::FromUTF8("转子位置:"), wxString::FromUTF8("0.00"));
    AddReadOnlyField(wxString::FromUTF8("电机模式:"), wxString::FromUTF8("0"));
    AddReadOnlyField(wxString::FromUTF8("电机温度:"), wxString::FromUTF8("正常"));
    AddReadOnlyField(wxString::FromUTF8("转子扭矩:"), wxString::FromUTF8("0.00"));
    AddReadOnlyField(wxString::FromUTF8("错误状态:"), wxString::FromUTF8("正常"));
    AddReadOnlyField(wxString::FromUTF8("转子速度:"), wxString::FromUTF8("0.00"));
    AddReadOnlyField(wxString::FromUTF8("传感器:"), wxString::FromUTF8("0"));

    leftPanelSizer->Add(leftGrid, 1, wxEXPAND | wxALL, 10);
    leftPanel->SetSizer(leftPanelSizer);

    // 2. 创建右侧：调试数据输入面板
    wxPanel* rightPanel = new wxPanel(this, wxID_ANY);
    // rightPanel->SetBackgroundColour(BG_MID);
    
    wxBoxSizer* rightPanelSizer = new wxBoxSizer(wxVERTICAL);

    // 标题
    wxStaticText* rightTitle = new wxStaticText(rightPanel, wxID_ANY, wxString::FromUTF8("● 调试数据输入"));
    rightTitle->SetFont(rightTitle->GetFont().Bold());
    rightPanelSizer->Add(rightTitle, 0, wxALL, 10);

    // 网格布局 (2列)
    wxFlexGridSizer* rightGrid = new wxFlexGridSizer(2, 10, 10);

    auto AddInputField = [&](const wxString& labelText, const wxString& placeholder) {
        wxStaticText* label = new wxStaticText(rightPanel, wxID_ANY, labelText);
        wxTextCtrl* input = new wxTextCtrl(rightPanel, wxID_ANY, placeholder, wxDefaultPosition, wxSize(120, -1));
        rightGrid->Add(label, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        rightGrid->Add(input, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
    };
    
    AddInputField(wxString::FromUTF8("电机ID(0-15):"), wxString::FromUTF8("0"));
    AddInputField(wxString::FromUTF8("转子位置(rad):"), wxString::FromUTF8("0"));
    AddInputField(wxString::FromUTF8("电机模式:"), wxString::FromUTF8("0"));
    AddInputField(wxString::FromUTF8("位置kp:"), wxString::FromUTF8("0"));
    AddInputField(wxString::FromUTF8("转子扭矩(Nm):"), wxString::FromUTF8("0"));
    AddInputField(wxString::FromUTF8("速度kd:"), wxString::FromUTF8("0"));
    AddInputField(wxString::FromUTF8("转子速度(rad/s):"), wxString::FromUTF8("0"));
    AddInputField(wxString::FromUTF8("通信频率(Hz):"), wxString::FromUTF8("1000"));

    rightPanelSizer->Add(rightGrid, 1, wxEXPAND | wxALL, 10);
    rightPanel->SetSizer(rightPanelSizer);

    // 3. 将两个面板加入 parentSizer（假设 parentSizer 是水平方向）
    // 使用 proportion=1 让两个面板各占一半宽度
    parentSizer->Add(leftPanel, 1, wxEXPAND | wxALL, 5);
    parentSizer->Add(rightPanel, 1, wxEXPAND | wxALL, 5);
}

void MainFrame::InitConfigPanel(wxSizer *parentSizer)
{
    wxBoxSizer *configSizer = new wxBoxSizer(wxHORIZONTAL);
    wxPanel *configPanel = new wxPanel(this, wxID_ANY);
    configPanel->SetBackgroundColour(wxColour(30, 30, 35));
    configPanel->SetFont(GetFont());

    // 在这里添加配置面板的控件
    wxStaticText *configLabel = new wxStaticText(configPanel, wxID_ANY, wxString::FromUTF8("配置面板"));
    configLabel->SetForegroundColour(*wxWHITE);
    configSizer->Add(configLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

    configPanel->SetSizerAndFit(configSizer);
    parentSizer->Add(configPanel, 0, wxEXPAND | wxALL, 5);
}

void MainFrame::InitMenuBar()
{
    // 菜单栏初始化代码 (如果需要)
    wxMenuBar *menuBar = new wxMenuBar();

    wxMenu *motorTypeMenu = new wxMenu();
    motorTypeMenu->AppendRadioItem(2001, "GO-M8010-6", "选择 GO-M8010-6 电机类型");
    // motorTypeMenu->AppendRadioItem(2002, "A1", "选择 A1 电机类型");

    menuBar->Append(motorTypeMenu, "&Motor Type");

    wxMenu *viewMenu = new wxMenu();

    // 使用 CheckItem，这样菜单上会有打钩状态
    viewMenu->AppendCheckItem(ID_VIEW_DEBUG, "调试视图 (Debug)\tCtrl+D");
    viewMenu->AppendCheckItem(ID_VIEW_CONFIG, "配置视图 (Config)\tCtrl+G");

    // 默认设置为选中状态
    viewMenu->Check(ID_VIEW_DEBUG, true);
    viewMenu->Check(ID_VIEW_CONFIG, true);

    menuBar->Append(viewMenu, "&View");

    // 5. 核心：将菜单栏安装到窗口上
    SetMenuBar(menuBar);

    // Bind(wxEVT_MENU, &MainFrame::OnToggleView, this, ID_VIEW_DEBUG);
    //     Bind(wxEVT_MENU, &MainFrame::OnToggleView, this, ID_VIEW_CONFIG);
}

void MainFrame::InitTopBar(wxSizer *parentSizer)
{
    wxBoxSizer *topSizer = new wxBoxSizer(wxHORIZONTAL);
    wxPanel *topBar = new wxPanel(this, wxID_ANY);
    wxSize boxSize(200, -1);
    wxComboBox *portBox = new wxComboBox(topBar, wxID_ANY, "/dev/cu.usbserial-FT53478H", wxDefaultPosition, boxSize);
    wxButton *startDebugButton = new wxButton(topBar, wxID_ANY, wxString::FromUTF8("开始调试"));
    wxButton *startConfigButton = new wxButton(topBar, wxID_ANY, wxString::FromUTF8("开始配置"));

    // topBar->SetBackgroundColour(TEXT_LIGHT);
    // 继承主窗口字体，确保字体可见
    topBar->SetFont(GetFont());
    topBar->SetBackgroundColour(wxColour(30, 30, 35));

    // 创建 ComboBox
    // 参数：父窗口, ID, 初始显示的文本, 位置, 大小, 预设选项数组, 样式

    portBox->Bind(wxEVT_COMBOBOX, [=](wxCommandEvent &event)
                  {
                      wxString selected = event.GetString();
                      // 处理选择逻辑...
                  });

    portBox->Bind(wxEVT_TEXT, [=](wxCommandEvent &event)
                  {
                      wxString input = event.GetString();
                      // 处理手动输入逻辑...
                  });

    // 将其添加到 Sizer 中
    topSizer->Add(portBox, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

 

    topSizer->AddStretchSpacer(1); // 弹性空间

    // --- 右侧：开始按钮 ---
    // 尝试设置蓝色背景 (在某些系统上 wxButton 的颜色定制能力有限)
    // startButton->SetBackgroundColour(ACCENT_BLUE);
    startDebugButton->SetForegroundColour(*wxWHITE);
    startDebugButton->SetMinSize(wxSize(100, -1));
    startDebugButton->Bind(wxEVT_BUTTON, [=](wxCommandEvent &event)
                      {
    // wxLogMessage 会根据平台自动处理：
    // 在调试模式下，它通常会输出到控制台（Terminal/Console）
    // 也可以直接使用标准 C++ 输出
    std::cout << "nihao" << std::endl; });

    startConfigButton->SetForegroundColour(*wxWHITE);
    startConfigButton->SetMinSize(wxSize(100, -1));
    startConfigButton->Bind(wxEVT_BUTTON, [=](wxCommandEvent &event)
                      {
    // wxLogMessage 会根据平台自动处理：
    // 在调试模式下，它通常会输出到控制台（Terminal/Console）
    // 也可以直接使用标准 C++ 输出
    std::cout << "nihao config" << std::endl; });

    topSizer->Add(startDebugButton, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    topSizer->Add(startConfigButton, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

    topBar->SetSizerAndFit(topSizer);
    parentSizer->Add(topBar, 0, wxEXPAND | wxALL, 5);
}
