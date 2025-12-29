#include "MainFrame.h"
#include "commandEvent.h"
#include "serial_initalize.h"
#include "wx/button.h"
#include "wx/choice.h"
#include "wx/menu.h"
#include "wx/panel.h"
#include "wx/sizer.h"
#include "wx/stattext.h"
#include <iostream>
#include <wx/log.h>
#include <wx/tglbtn.h>

//// 普通信息
// wxLogMessage("当前位置: x=%f, y=%f", Pos, W);
//
//// 警告信息（通常会带黄色图标）
// wxLogWarning("温度过高: %d °C", Temp);
//
//// 错误信息（在某些配置下会弹出错误对话框）
// wxLogError("电机通讯失败，错误码: %d", Error);
//
//// 调试信息（仅在 Debug 模式下有效，Release 模式会被忽略）
// wxLogDebug("内部变量 T 的值: %f", T);
//
//// 状态栏简短信息
// wxLogStatus("系统运行中...");

// MainFrame 实现
MainFrame::MainFrame(const wxString &title, const wxPoint &pos,
                     const wxSize &size)
    : wxFrame(NULL, wxID_ANY, title, pos, size), backendThread(this) {
  // this->SetBackgroundColour(wxColour(18, 18, 20));
  wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
  InitMenuBar();

  InitTopBar(mainSizer);

  InitDebugPanel(mainSizer);

  InitConfigPanel(mainSizer);

  /*************** Bind(backend data to frontend) *********************/
  this->Bind(EVT_MOTOR_INFO, &MainFrame::UpdateDataDisplay, this);

  SetSizer(mainSizer);
  Center(); // 窗口居中
}

void MainFrame::InitMenuBar() {
  wxMenuBar *menuBar = new wxMenuBar();
  wxMenu *motorTypeMenu = new wxMenu();
  motorTypeMenu->AppendRadioItem(2001, "GO-M8010-6",
                                 "选择 GO-M8010-6 电机类型");
  // motorTypeMenu->AppendRadioItem(2002, "A1", "选择 A1 电机类型");

  menuBar->Append(motorTypeMenu, "&Motor Type");

  SetMenuBar(menuBar);

  // Bind(wxEVT_MENU, &MainFrame::OnToggleView, this, ID_VIEW_DEBUG);
  // Bind(wxEVT_MENU, &MainFrame::OnToggleView, this, ID_VIEW_CONFIG);
}

void MainFrame::InitTopBar(wxSizer *parentSizer) {
  wxBoxSizer *topSizer = new wxBoxSizer(wxHORIZONTAL);
  wxPanel *topBar = new wxPanel(this, wxID_ANY);
  wxSize boxSize(200, -1);
  // 参数：父窗口, ID, 初始显示的文本, 位置, 大小, 预设选项数组, 样
  portBox = new wxComboBox(topBar, wxID_ANY, "/dev/cu.usbserial-FT53478H",
                           wxDefaultPosition, boxSize);
  startDebugButton =
      new wxToggleButton(topBar, wxID_ANY, wxString::FromUTF8("开始调试"));
  startConfigButton =
      new wxToggleButton(topBar, wxID_ANY, wxString::FromUTF8("开始配置"));

  //  topBar->SetFont(GetFont());
  //  topBar->SetBackgroundColour(wxColour(30, 30, 35));

  portBox->Bind(wxEVT_COMBOBOX, [=](wxCommandEvent &event) {
    wxString selected = event.GetString();
    // 处理选择逻辑...
  });

  portBox->Bind(wxEVT_TEXT, [=](wxCommandEvent &event) {
    wxString input = event.GetString();
    // 处理手动输入逻辑...
  });

  // 将其添加到 Sizer 中
  topSizer->Add(portBox, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

  topSizer->AddStretchSpacer(1); // 弹性空间

  startDebugButton->SetForegroundColour(*wxWHITE);
  startDebugButton->SetMinSize(wxSize(100, -1));
  startDebugButton->Bind(wxEVT_TOGGLEBUTTON, [this](wxCommandEvent &event) {
    if (startDebugButton->GetValue()) {
      /**************************button down***********************/
      unsigned int freq;
      wxString selected_string = portBox->GetValue();
      backendThread.setSerialportname(selected_string.ToStdString());
      auto it = m_dataInputs.find("set_freq");
      if (it != m_dataInputs.end()) {
        wxString currentText = it->second->GetValue();

        currentText.ToUInt(&freq);
        //        std::cout << currentText.ToStdString() << std::endl;
      } else {
        startDebugButton->SetValue(false);
      }

      if (backendThread.initializeDebugSerial(freq)) {
        sendDebugButton->Enable(true);
        startConfigButton->SetValue(false);
      } else {
        startDebugButton->SetValue(false);
      }
    } else {
      /**************************button up***********************/
      // GetValue() 为 false 表示按钮弹回来了
      if (!startConfigButton->GetValue()) {
        sendDebugButton->Enable(false);

        backendThread.destoryDebugSerial();
        UpdateDataDisplayNone();
      }
    }
  });

  startConfigButton->SetForegroundColour(*wxWHITE);
  startConfigButton->SetMinSize(wxSize(100, -1));
  startConfigButton->Bind(wxEVT_TOGGLEBUTTON, [this](wxCommandEvent &event) {
    if (startConfigButton->GetValue()) {
      // GetValue() 为 true 表示按钮被按下去了
      // sendDebugButton->Enable(true);
      startDebugButton->SetValue(false);
    } else {
      // GetValue() 为 false 表示按钮弹回来了
      if (!startDebugButton->GetValue())
        ;
      // sendDebugButton->Enable(false);
    }
  });

  topSizer->Add(startDebugButton, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
  topSizer->Add(startConfigButton, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

  // startDebugButton->Bind(wxEVT_BUTTON,EnableDebugPanel);
  // startConfigButton->Bind(wxEVT_BUTTON,EnableConfigPanel);

  topBar->SetSizerAndFit(topSizer);
  parentSizer->Add(topBar, 0, wxEXPAND | wxALL, 5);
}

void MainFrame::InitDebugPanel(wxSizer *parentSizer) {
  // 创建水平容器面板
  std::string input[8];
  input[0] = std::string("set_id");
  input[1] = std::string("set_mod");
  input[2] = std::string("set_freq");
  input[6] = std::string("set_kp");
  input[7] = std::string("set_kd");
  input[5] = std::string("set_pos");
  input[3] = std::string("set_T");
  input[4] = std::string("set_w");

  wxPanel *middleContainer = new wxPanel(this, wxID_ANY);
  //  middleContainer->SetBackgroundColour(wxColour(25, 25, 28));
  wxBoxSizer *middleSizer = new wxBoxSizer(wxHORIZONTAL);

  wxPanel *leftPanel = new wxPanel(middleContainer, wxID_ANY);
  wxBoxSizer *leftPanelSizer = new wxBoxSizer(wxVERTICAL);

  wxStaticText *leftTitle =
      new wxStaticText(leftPanel, wxID_ANY, wxString::FromUTF8("● 数据解析"));
  //  leftTitle->SetForegroundColour(wxColour(100, 200, 255)); // 浅蓝色标题
  leftTitle->SetFont(leftTitle->GetFont().Bold());
  leftPanelSizer->Add(leftTitle, 0, wxALL, 10);

  wxFlexGridSizer *leftGrid =
      new wxFlexGridSizer(2, 10, 25); // 2列，行间距10，列间距25

  // 匿名函数：添加只读字段并保存指针
  auto AddReadOnlyField = [&](const wxString &id, const wxString &labelText) {
    wxStaticText *label = new wxStaticText(leftPanel, wxID_ANY, labelText);
    label->SetForegroundColour(wxColour(180, 180, 180));

    wxStaticText *value = new wxStaticText(leftPanel, wxID_ANY, "---");
    //    value->SetForegroundColour(wxColour(255, 255, 255)); // 白色数值

    leftGrid->Add(label, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
    leftGrid->Add(value, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);

    m_dataDisplays[id] = value; // 保存指针以便后续更新
  };

  AddReadOnlyField("motor_id", wxString::FromUTF8("电机ID:"));
  AddReadOnlyField("pos", wxString::FromUTF8("转子位置:"));
  AddReadOnlyField("mode", wxString::FromUTF8("电机模式:"));
  AddReadOnlyField("temp", wxString::FromUTF8("电机温度:"));
  AddReadOnlyField("torque", wxString::FromUTF8("转子扭矩:"));
  AddReadOnlyField("error", wxString::FromUTF8("错误状态:"));
  AddReadOnlyField("velocity", wxString::FromUTF8("角速度："));

  leftPanelSizer->Add(leftGrid, 1, wxEXPAND | wxALL, 10);
  leftPanel->SetSizer(leftPanelSizer);

  // --- 右侧：调试数据输入面板 ---
  wxPanel *rightPanel = new wxPanel(middleContainer, wxID_ANY);
  wxBoxSizer *rightPanelSizer = new wxBoxSizer(wxVERTICAL);

  wxStaticText *rightTitle = new wxStaticText(
      rightPanel, wxID_ANY, wxString::FromUTF8("● 调试数据输入"));
  rightTitle->SetForegroundColour(wxColour(255, 150, 50)); // 橙色标题
  rightTitle->SetFont(rightTitle->GetFont().Bold());
  rightPanelSizer->Add(rightTitle, 0, wxALL, 10);

  wxFlexGridSizer *rightGrid = new wxFlexGridSizer(2, 10, 25);

  // 匿名函数：添加输入字段并保存指针
  auto AddInputField = [&](const wxString &id, const wxString &labelText,
                           const wxString &defaultValue) {
    wxStaticText *label = new wxStaticText(rightPanel, wxID_ANY, labelText);
    //    label->SetForegroundColour(wxColour(180, 180, 180));

    wxTextCtrl *input =
        new wxTextCtrl(rightPanel, wxID_ANY, defaultValue, wxDefaultPosition,
                       wxSize(100, -1), wxTE_PROCESS_ENTER);
    //    input->SetBackgroundColour(wxColour(40, 40, 45));
    //    input->SetForegroundColour(wxColour(255, 255, 255));

    rightGrid->Add(label, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
    rightGrid->Add(input, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);

    m_dataInputs[id] = input; // 保存指针以便读取用户输入
  };

  AddInputField(input[0], wxString::FromUTF8("设置ID:"), "0");
  AddInputField(input[1], wxString::FromUTF8("模式mod:"), "0");
  AddInputField(input[6], wxString::FromUTF8("位置Kp:"), "0");

  AddInputField(input[7], wxString::FromUTF8("速度Kd:"), "0");
  AddInputField(input[5], wxString::FromUTF8("位置:"), "0");
  AddInputField(input[3], wxString::FromUTF8("力矩:"), "0");

  AddInputField(input[4], wxString::FromUTF8("速度:"), "0");
  AddInputField(input[2], wxString::FromUTF8("频率(Hz):"), "1000");

  rightPanelSizer->Add(rightGrid, 1, wxEXPAND | wxALL, 10);
  rightPanelSizer->AddStretchSpacer(1);
  sendDebugButton =
      new wxButton(rightPanel, wxID_ANY, wxString::FromUTF8("发送数据"));
  sendDebugButton->SetForegroundColour(*wxWHITE);
  sendDebugButton->SetMinSize(wxSize(100, -1));
  sendDebugButton->Enable(false);

  sendDebugButton->Bind(wxEVT_BUTTON, [=](wxCommandEvent &event) {
    unsigned int argv_short[3];
    double argv_float[5];
    for (int i = 0; i < 3; i++) {
      auto it = m_dataInputs.find(input[i]);
      if (it != m_dataInputs.end()) {
        wxString currentText = it->second->GetValue();

        currentText.ToUInt(&(argv_short[i]));
        //        std::cout << currentText.ToStdString() << std::endl;
      }
    }
    for (int i = 3; i < 8; i++) {
      auto it = m_dataInputs.find(input[i]);
      if (it != m_dataInputs.end()) {
        wxString currentText = it->second->GetValue();

        currentText.ToDouble(&argv_float[i - 3]);
        //        std::cout << currentText.ToStdString() << std::endl;
      }
    }

    if (!backendThread.setDebugSerialCmd(
            argv_short[0], argv_short[1], argv_short[2], argv_float[0],
            argv_float[1], argv_float[2], argv_float[3], argv_float[4])) {
      startDebugButton->SetValue(false);
    };
  });

  rightPanelSizer->Add(sendDebugButton, 0, wxALL , 5);

  rightPanel->SetSizer(rightPanelSizer);

  // 组装容器
  middleSizer->Add(leftPanel, 1, wxEXPAND | wxALL, 5);
  middleSizer->Add(rightPanel, 1, wxEXPAND | wxALL, 5);
  middleContainer->SetSizer(middleSizer);

  parentSizer->Add(middleContainer, 1, wxEXPAND | wxALL, 10);
}
void MainFrame::InitConfigPanel(wxSizer *parentSizer) {
  // 1. 创建一个容器 Panel，确保所有控件背景一致
  wxPanel *containerPanel = new wxPanel(this, wxID_ANY);
  wxBoxSizer *containerSizer = new wxBoxSizer(wxVERTICAL);

  // 标题 - 挂载到 containerPanel 上
  wxStaticText *title = new wxStaticText(containerPanel, wxID_ANY,
                                         wxString::FromUTF8("● 配置工具"));
  title->SetFont(title->GetFont().Bold());
  containerSizer->Add(title, 0, wxALL, 10);

  // 2. 按钮布局区域
  wxPanel *buttonPanel = new wxPanel(containerPanel, wxID_ANY);
  // 定义 0 行 5 列 (0 表示行数自动计算)
  wxFlexGridSizer *buttonSizer = new wxFlexGridSizer(0, 5, 8, 8);

  // 设置列的增长比例，让这 5 列平分空间
  for (int i = 0; i < 5; ++i) {
    buttonSizer->AddGrowableCol(i, 1);
  }

  auto AddToGrid = [&](wxWindow *win) {
    buttonSizer->Add(win, 0, wxEXPAND | wxALL, 2);
  };

  // 第1行
  AddToGrid(new wxButton(buttonPanel, wxID_ANY, wxString::FromUTF8("查询ID")));
  AddToGrid(new wxButton(buttonPanel, wxID_ANY, wxString::FromUTF8("修改ID")));
  AddToGrid(new wxTextCtrl(buttonPanel, wxID_ANY, "0", wxDefaultPosition,
                           wxSize(60, -1)));
  AddToGrid(
      new wxButton(buttonPanel, wxID_ANY, wxString::FromUTF8("恢复电机模式")));
  AddToGrid(
      new wxButton(buttonPanel, wxID_ANY, wxString::FromUTF8("查询版本号")));

  // 第2行
  AddToGrid(
      new wxButton(buttonPanel, wxID_ANY, wxString::FromUTF8("开启自动引导")));
  AddToGrid(
      new wxButton(buttonPanel, wxID_ANY, wxString::FromUTF8("关闭自动引导")));
  AddToGrid(new wxStaticText(buttonPanel, wxID_ANY, "")); // 占位
  AddToGrid(
      new wxButton(buttonPanel, wxID_ANY, wxString::FromUTF8("临时自动引导")));
  AddToGrid(
      new wxButton(buttonPanel, wxID_ANY, wxString::FromUTF8("退出自动引导")));

  // 第3行
  AddToGrid(
      new wxButton(buttonPanel, wxID_ANY, wxString::FromUTF8("点击上传")));
  AddToGrid(
      new wxButton(buttonPanel, wxID_ANY, wxString::FromUTF8("更新固件")));

  buttonPanel->SetSizer(buttonSizer);

  // 3. 组装：将按钮面板放入容器 Sizer
  containerSizer->Add(buttonPanel, 1, wxEXPAND | wxALL, 5);
  containerPanel->SetSizer(containerSizer);

  // 4. 将整个容器放入外部传入的 parentSizer
  parentSizer->Add(containerPanel, 1, wxEXPAND | wxALL, 5);
}

void MainFrame::UpdateDataDisplay(InformationEvent &event) {
  float T = event.GetT();
  float W = event.GetW();
  float Pos = event.GetPos();
  int Temp = event.GetTemp();
  int Error = event.GetError();

  int GetFooForce = event.GetFooForce();
  int ID = event.GetMotorID();
  int Mode = event.GetMode();
  //  wxLogDebug("内部变量 ERROR %d", Error);

  wxStaticText *valueid = m_dataDisplays["motor_id"];
  wxStaticText *valuepos = m_dataDisplays["pos"];
  wxStaticText *valuemode = m_dataDisplays["mode"];
  wxStaticText *valuetemp = m_dataDisplays["temp"];
  wxStaticText *valuetorque = m_dataDisplays["torque"];
  wxStaticText *valueerror = m_dataDisplays["error"];
  wxStaticText *valuevelocity = m_dataDisplays["velocity"];

  valuetorque->SetLabel(wxString::Format("%.2f N·m", T));
  valuetemp->SetLabel(wxString::Format("%d °C", Temp));
  valueid->SetLabel(wxString::Format("%d", ID));
  valuepos->SetLabel(wxString::Format("%.3f", Pos));
  valuemode->SetLabel(wxString::Format("%d", Mode));
  valueerror->SetLabel(wxString::Format("%d", Error));
  valuevelocity->SetLabel(wxString::Format("%.3f", W));
}
void MainFrame::UpdateDataDisplayNone() {
  wxStaticText *valueid = m_dataDisplays["motor_id"];
  wxStaticText *valuepos = m_dataDisplays["pos"];
  wxStaticText *valuemode = m_dataDisplays["mode"];
  wxStaticText *valuetemp = m_dataDisplays["temp"];
  wxStaticText *valuetorque = m_dataDisplays["torque"];
  wxStaticText *valueerror = m_dataDisplays["error"];
  wxStaticText *valuevelocity = m_dataDisplays["velocity"];

  valuetorque->SetLabel(wxString::Format("---"));
  valuetemp->SetLabel(wxString::Format("---"));
  valueid->SetLabel(wxString::Format("---"));
  valuepos->SetLabel(wxString::Format("---"));
  valuemode->SetLabel(wxString::Format("---"));
  valueerror->SetLabel(wxString::Format("---"));
  valuevelocity->SetLabel(wxString::Format("---"));
}
