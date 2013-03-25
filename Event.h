/*! {{{
  \file Event.h

  \brief

  }}} */

#ifndef EVENT_H
#define EVENT_H


#include <boost/shared_ptr.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/utility.hpp>
#include <boost/thread/mutex.hpp>

#include "Debug.h"


#include "TimerSystem.h"

namespace Event
{

class Event //{{{
{
public:
  Event( unsigned long aID, int aParam = 0 ) : theID( aID ), theParam( aParam ) {};
  virtual ~Event() {};

  unsigned int ID() const { return theID; };
  int Param() const { return theParam; };

private:
  unsigned long theID;
  int theParam;
}; //}}}

typedef boost::shared_ptr<Event> EventPointer;

enum ReservedEvents
{
  EVENT_INIT    = 0x01000000,
  EVENT_ENTRY   = 0x02000000,
  EVENT_EXIT    = 0x03000000,
  EVENT_TIMEOUT = 0x04000000,
  EVENT_START   = 0x05000000,
  EVENT_FINISH  = 0x06000000
};

// TODO constexpr
#define TIMER_ELAPSED( id ) ( EVENT_TIMEOUT + (id) )

enum { NO_WAIT = 0, WAIT_FOREWER = -1 };

class EventProcessor : protected TimerSystem, protected boost::noncopyable //{{{
{
public:
  EventProcessor( unsigned int aID );
  ~EventProcessor();

  void PushEvent( const EventPointer & aEvent );

  void Run();

  inline unsigned int GetID() const { return theID; }

  bool IsEventOfInteres( const EventPointer & aEvent ) const;
protected:
  enum { EVENTQ_MAX_SIZE = 256 };
  enum EventResult { EventPresent, EventTimeout, EventError };

  EventResult GetEvent( EventPointer & aEvent, long aMaxWaitTime = WAIT_FOREWER );

  virtual void OnEvent( const EventPointer & aEvent );
  virtual bool IsUserEventOfInteres( const EventPointer & /*aEvent*/ ) const
  {
    return true;
  }

private:
  unsigned int theID;
  int thePipeFDs[2];

  boost::circular_buffer< EventPointer > theEventQueue;
  boost::mutex theLock;

  bool IsSystemEvent( const EventPointer & aEvent ) const;
}; //}}}


void SendEvent( const EventPointer & aEvent );
}

#endif /* ifndef EVENT_H */

/* {{{ Modeline for ViM
 * vim600:fdm=marker fdl=0 fdc=3:
 * }}} */

