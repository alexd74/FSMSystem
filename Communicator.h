/*! {{{
  \file EventProcessor.h

  \brief

  }}} */

#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#include "Event.h"

namespace Event
{

class Communicator: public EventProcessor //{{{
{
public:
  Communicator( unsigned int aID ) : EventProcessor( aID ),  theReadFD( -1 ) {}
  virtual ~Communicator();

//  void PushEvent( const EventPointer & aEvent );

//  void Run();

//  bool IsEventOfInteres( const EventPointer & aEvent ) const;



  void SetFD( int FD ) { theReadFD = FD; }
  int GetFD() const { return theReadFD; }
protected:


//  enum { EVENTQ_MAX_SIZE = 256 };
//  enum EventResult { EventPresent, EventTimeout, EventError };
//
  EventResult GetEvent( EventPointer & aEvent, long aMaxWaitTime = WAIT_FOREWER );

  virtual EventResult ReadFromFD( EventPointer & aEvent, long aMaxWaitTime = WAIT_FOREWER );
//
  virtual bool HasDataToProcess();

//  virtual void OnEvent( const EventPointer & aEvent );
//  virtual bool IsUserEventOfInteres( const EventPointer & /*aEvent*/ ) const
//  {
//    return true;
//  }

private:
  int theReadFD;
}; //}}}

}

#endif /* ifndef EVENT_H */

/* {{{ Modeline for ViM
 * vim600:fdm=marker fdl=0 fdc=3:
 * }}} */

