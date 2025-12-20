


#ifndef MOTOR_EVENTS_H
#define MOTOR_EVENTS_H


#include <wx/event.h>

// ============================================================================
// 1. 电机控制事件 (前端 -> 后端)
// ============================================================================
class ControlEvent : public wxCommandEvent {
public:
    // 完整参数构造函数
    ControlEvent(wxEventType commandType, int id, 
                 unsigned short motorId, unsigned short mode, 
                 float t, float w, float pos, float kp, float kw, unsigned short freq)
        : wxCommandEvent(commandType, id), 
          m_ID(motorId), m_MODE(mode), m_T(t), m_W(w), 
          m_POS(pos), m_K_P(kp), m_K_W(kw), m_frequency(freq) {}

    // 默认构造函数
    ControlEvent(wxEventType commandType, int id = 0)
        : wxCommandEvent(commandType, id), 
          m_ID(0), m_MODE(0), m_T(0.0f), m_W(0.0f), 
          m_POS(0.0f), m_K_P(0.0f), m_K_W(0.0f), m_frequency(0) {}
    
    // 拷贝构造函数 (供 Clone 使用)
    ControlEvent(const ControlEvent& event)
        : wxCommandEvent(event), 
          m_ID(event.m_ID), m_MODE(event.m_MODE), m_T(event.m_T), 
          m_W(event.m_W), m_POS(event.m_POS), m_K_P(event.m_K_P), 
          m_K_W(event.m_K_W), m_frequency(event.m_frequency) {}
    
    virtual wxEvent* Clone() const override { 
        return new ControlEvent(*this); 
    }

    // 数据获取接口 (Getter)
    unsigned short GetMotorID() const { return m_ID; }
    unsigned short GetMode()    const { return m_MODE; }
    float GetT()                const { return m_T; }
    float GetW()                const { return m_W; }
    float GetPOS()              const { return m_POS; }
    unsigned short GetFreq()    const { return m_frequency; }
    
private:
    unsigned short m_ID;
    unsigned short m_MODE;
    float m_T;
    float m_W;
    float m_POS;
    float m_K_P;
    float m_K_W;
    unsigned short m_frequency; 
};

// ============================================================================
// 2. 电机反馈事件 (后端 -> 前端)
// ============================================================================
class InformationEvent : public wxCommandEvent {
public:
    // 完整参数构造函数
    InformationEvent(wxEventType commandType, int id,
                     unsigned short motorId, unsigned short mode, 
                     float t, float w, int temp, float pos, int error, int fooforce)
        : wxCommandEvent(commandType, id), 
          m_ID(motorId), m_MODE(mode), m_T(t), m_W(w), 
          m_TEMP(temp), m_POS(pos), m_ERROR(error), m_FOOFORCE(fooforce) {}

    // 默认构造函数
    InformationEvent(wxEventType commandType, int id = 0)
        : wxCommandEvent(commandType, id), 
          m_ID(0), m_MODE(0), m_T(0.0f), m_W(0.0f), 
          m_TEMP(0), m_POS(0.0f), m_ERROR(0), m_FOOFORCE(0) {}
    
    // 拷贝构造函数
    InformationEvent(const InformationEvent& event)
        : wxCommandEvent(event), 
          m_ID(event.m_ID), m_MODE(event.m_MODE), m_T(event.m_T), 
          m_W(event.m_W), m_TEMP(event.m_TEMP), m_POS(event.m_POS), 
          m_ERROR(event.m_ERROR), m_FOOFORCE(event.m_FOOFORCE) {}
    
    virtual wxEvent* Clone() const override { 
        return new InformationEvent(*this); 
    }

    // 数据获取接口 (Getter)
    unsigned short GetMotorID() const { return m_ID; }
    float GetT()                const { return m_T; }
    float GetW()                const { return m_W; }
    int   GetTemp()             const { return m_TEMP; }
    float GetPos()              const { return m_POS; }
    int   GetError()            const { return m_ERROR; }

private:
    unsigned short m_ID;
    unsigned short m_MODE;
    float m_T;
    float m_W;
    int   m_TEMP;
    float m_POS;
    int   m_ERROR;
    int   m_FOOFORCE;
};

// 声明全局事件类型
wxDECLARE_EVENT(EVT_MOTOR_CONTROL, ControlEvent);
wxDECLARE_EVENT(EVT_MOTOR_INFO, InformationEvent);

#endif // MOTOR_EVENTS_H




