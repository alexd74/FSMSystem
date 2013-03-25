/*! {{{
  \file ActiveObject.h

  \brief

  }}} */

#ifndef ACTIVEOBJECT_H
#define ACTIVEOBJECT_H

#include <boost/thread.hpp>

#include "Event.h"

namespace Event
{

class ActiveObject //{{{
{
public:
  ActiveObject( EventProcessor &aEventProcessor, int aPriority = 0 )
    : theEventProcessor( aEventProcessor ), thePriority( aPriority )
  {
  };

  void Start();
  void Stop();

  void PushEvent( const EventPointer & aEvent ) { theEventProcessor.PushEvent( aEvent ); }

private:
  EventProcessor &theEventProcessor;
  boost::thread theThread;

  int thePriority;
}; //}}}

}

#endif /* ifndef ACTIVEOBJECT_H */

/* {{{ Modeline for ViM
 * vim600:fdm=marker fdl=0 fdc=3:
 * }}} */

