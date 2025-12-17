#ifndef COMMANDEVENT
#define COMMANDEVENT

#include <wx/event.h>
#include "unitreeMotor.h"

class ControlEvent : public wxCommandEvent {
public:
    ControlEvent(wxEventType commandType = MY_DATA_EVENT, int id = 0)
        : wxCommandEvent(commandType, id) {}
    
    ControlEvent(const ControlEvent& event)
        : wxCommandEvent(event), m_data(event.m_data) {}
    
    virtual wxEvent* Clone() const override { 
        return new ControlEvent(*this); 
    }
    
private:
    Motorcmd m_data;
};

class InformationEvent : public wxCommandEvent {
public:
    InformationEvent(wxEventType commandType = MY_DATA_EVENT, int id = 0)
        : wxCommandEvent(commandType, id) {}
    
    InformationEvent(const ControlEvent& event)
        : wxCommandEvent(event), m_data(event.m_data) {}
    
    virtual wxEvent* Clone() const override { 
        return new InformationEvent(*this); 
    }
    
private:
    MotorData m_data;
};




#endif
