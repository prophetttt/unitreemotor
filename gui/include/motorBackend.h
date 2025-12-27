#ifndef MOTORBACKEND_H
#define MOTORBACKEND_H

#include "commandEvent.h"
#include "coreThread.h"
#include "unitreeMotor.h"
#include <string>
#include <iostream>

// Custom event for backend to frontend communication
class MotorBackend {
public:
  MotorBackend(wxEvtHandler *handler) : m_handler(handler) {}
  void setSerialportname(std::string serialport) { serial_name = serialport; }
  bool initializeDebugSerial(unsigned short freq) {
    m_coreThread = new CoreThread<MotorCmdGom, MotorDataGom>(
        serial_name, freq, [this](MotorCmdGom &cmd, MotorDataGom &data) {
          return this->SendStatusUpdate(cmd, data);
        });
    if (!m_coreThread->thread_active) {
      return false;
    }
    return true;
  }
  bool destoryDebugSerial() {
    if (m_coreThread == nullptr) {
      return false;
    }
    delete m_coreThread;
    return true;
  }
  bool setDebugSerialCmd(const unsigned short &ID, const unsigned short &MODE,
                         const unsigned short &seq, const float &T,
                         const float &W, const float &POS, const float &K_P,
                         const float &K_W) {
    MotorCmdGom Cmd;
    if (!initMotoCmdGom(&Cmd, &ID, &MODE, &T, &W, &POS, &K_P, &K_W)) {
      return false;
    }
    if (!m_coreThread->setfeq(seq)) {
      return false;
    }
    return m_coreThread->setCmd(ID, Cmd);
  }

private:
  std::string serial_name;
  wxEvtHandler *m_handler;
  CoreThread<MotorCmdGom, MotorDataGom> *m_coreThread;

  // callback in CoreThread
  bool SendStatusUpdate(MotorCmdGom &, MotorDataGom &Data) {
    for (int i = 0; i < 100; i++)
      ;
//    std::cout << "callback" << std::endl;
    if (m_handler) {
      unsigned short ID;
      unsigned short MODE;
      float T;
      float W;
      float POS;
      int TEMP;
      int ERROR;
      int FOOFORCE;
      depackMotoDataGom(&Data, &ID, &MODE, &T, &W, &POS, &TEMP, &ERROR,
                        &FOOFORCE);
      // 创建自定义事件对象（使用之前定义的 MyCustomEvent）
      // 注意：QueueEvent 会接管事件对象的指针，它会自动处理 delete
      InformationEvent *event = new InformationEvent(
          EVT_MOTOR_INFO, wxID_ANY, ID, MODE, T, W, TEMP, POS, ERROR, FOOFORCE);

      // 【关键】使用 QueueEvent，这是线程安全的
      m_handler->QueueEvent(event);
    }
    return true;
  }
};

#endif