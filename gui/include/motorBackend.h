#ifndef MOTORBACKEND_H
#define MOTORBACKEND_H



class motorBackend{
    wxEvtHandler* m_handler;
    motorBackend(wxEvtHandler* handler) : m_handler(handler) {}
}

// Backend.h
class DebugBackend {
public:
    // 构造时传入 UI 窗口的指针作为事件接收者
    DebugBackend(wxEvtHandler* handler) : m_handler(handler) {}

    void RunTask() {
        // ... 执行某些逻辑 ...

        // 逻辑完成后，发送自定义事件
        SendStatusUpdate("Task Finished", 100);
    }

private:
    wxEvtHandler* m_handler; // 指向前端窗口或组件

    void SendStatusUpdate(const wxString& msg, int progress) {
        if (m_handler) {
            // 创建自定义事件对象（使用之前定义的 MyCustomEvent）
            // 注意：QueueEvent 会接管事件对象的指针，它会自动处理 delete
            MyCustomEvent* event = new MyCustomEvent(wxEVT_MY_CUSTOM_COMMAND, wxID_ANY, msg, progress);
            
            // 【关键】使用 QueueEvent，这是线程安全的
            m_handler->QueueEvent(event); 
        }
    }
};