/*! {{{ File head comment
  \file ActiveObject.c

  \brief

 }}} */

#include <stdio.h>
#include "ActiveObject.h"

#include "Event.h"
#include "Debug.h"

namespace Event
{

void ActiveObject::Start() //{{{
{
  DEBUG_TRACER;

  struct callable //{{{
  {
    callable( EventProcessor & aProcessor ) : theProcessor( aProcessor ) {}

    void operator()()
    {
      theProcessor.Run();
    }
    private:

    EventProcessor &theProcessor;
  }; //}}}

  callable cl( theEventProcessor );

  boost::thread th( cl );

  theThread.swap( th );
} //}}}

void ActiveObject::Stop() //{{{
{
  DEBUG_TRACER;

  theEventProcessor.PushEvent( EventPointer( new Event( EVENT_FINISH ) ) );

  theThread.join();
} //}}}

} // end namespace Event

/* {{{ Modeline for ViM
 * vim600:fdm=marker fdl=0 fdc=3:
 * }}} */
