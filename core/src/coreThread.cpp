#include"../include/coreThread.h"
#include <thread>
#include "../platform/linux/include/serialPort/SerialPort.h"
#include <wx/event.h>      // 核心事件系统
#include <wx/evtloop.h>    // 事件循环相关
#include <wx/app.h>        // wxTheApp
#include <wx/window.h>     // 窗口相关

CoreThread::CoreThread(std::string seria, std::function callback):serial_port(serial),core_thread(&CoreThread::serialReadandSend),callbackaction(callback)
{
    
}


//running in separate thread
void CoreThread::serialReadandSend(){
    SerialPort serial(serial_port);
    while(thread_alive){
        {
            std::shared_lock read_lock(rw_mutex_);
            serial.sendRecv(&motor_cmd,&motor_data);
            //callback
        }
        // wxQueueEvent(wxTheApp->GetTopWindow()->GetEventHandler(),
        //              new wxCommandEvent(wxEVT_COMMAND_MENU_SELECTED, wxID_ANY));
        
    }
}

CoreThread::~CoreThread(){
    if(core_thread.joinable()){
        core_thread.join();
    }
}