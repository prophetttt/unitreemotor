#ifndef COMMANDEVENT
#define COMMANDEVENT

#include <wx/event.h>
#include "unitreeMotor.h"


wxDECLARE_EVENT(EVT_MOTOR_CONTROL, wxCommandEvent);
wxDECLARE_EVENT(EVT_MOTOR_INFO, wxCommandEvent);


class ControlEvent : public wxCommandEvent {
public:
    ControlEvent(wxEventType commandType = EVT_MOTOR_CONTROL, int id = 0)
        : wxCommandEvent(commandType, id) {}
    
    ControlEvent(const ControlEvent& event)
        : wxCommandEvent(event), m_data(event.m_data) {}
    
    virtual wxEvent* Clone() const override { 
        return new ControlEvent(*this); 
    }
    
private:
    MotorCmdGom m_data;
};

class InformationEvent : public wxCommandEvent {
public:
    InformationEvent(wxEventType commandType = EVT_MOTOR_INFO, int id = 0)
        : wxCommandEvent(commandType, id) {}
    
    // InformationEvent(const ControlEvent& event)
    //     : wxCommandEvent(event), m_data(event.m_data) {}
    
    virtual wxEvent* Clone() const override { 
        return new InformationEvent(*this); 
    }
    
private:
    MotorDataGom m_data;
};






#endif
