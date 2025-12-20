#ifndef MOTORBACKEND_H
#define MOTORBACKEND_H

#include "coreThread.h"
#include "commandEvent.h"
#include "unitreeMotor.h"

// Custom event for backend to frontend communication
class motorBackend
{
public:
    motorBackend(wxEvtHandler *handler) : m_handler(handler) {}

private:
    wxEvtHandler *m_handler;
    CoreThread *m_coreThread;

    void SendStatusUpdate(const wxString &msg, int progress)
    {
        if (m_handler)
        {
            // 创建自定义事件对象（使用之前定义的 MyCustomEvent）
            // 注意：QueueEvent 会接管事件对象的指针，它会自动处理 delete
            InformationEvent *event = new InformationEvent(EVT_MOTOR_INFO, wxID_ANY, msg, progress);

            // 【关键】使用 QueueEvent，这是线程安全的
            m_handler->QueueEvent(event);
        }
    }
};
