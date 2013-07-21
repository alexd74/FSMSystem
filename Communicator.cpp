/*! {{{ File head comment
  \file Event.cpp

  \brief

  }}} */

#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/thread/locks.hpp>

#include <loki/Singleton.h>

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <stdexcept>
#include <utility>

#include <sys/poll.h>

#include "Event.h"
#include "Communicator.h"

using namespace boost::posix_time;
using namespace boost::lambda;

namespace Event
{
EventProcessor::EventResult Communicator::GetEvent( EventPointer & aEvent, long aMaxWaitTime /*= WAIT_FOREWER*/ ) //{{{
{
  DEBUG_TRACER;
  if( HasDataToProcess() )
    if( ReadFromFD( aEvent ) == EventPresent )
      return EventPresent;

  if( aMaxWaitTime != NO_WAIT )
  {
    pollfd pollFD[2];
    pollFD[0].fd = thePipeFDs[0];
    pollFD[0].events = POLLIN;

    int maxFD = 1;

    if( theReadFD >= 0 )
    {
      maxFD = 2;
      pollFD[1].fd = theReadFD;
      pollFD[1].events = POLLIN;
    }

    poll( pollFD, maxFD, aMaxWaitTime );

    if( theReadFD >= 0 && ( pollFD[1].revents & POLLIN ) )
    {
      if( ReadFromFD( aEvent ) == EventPresent )
        return EventPresent;
    }

    if( !( pollFD[0].revents & POLLIN ) )
      return EventTimeout;
  }

  boost::lock_guard<boost::mutex> guard( theLock );

  if( theEventQueue.empty() )
    return EventError;

  aEvent = theEventQueue.front();
  theEventQueue.pop_front();

  char ch = 'Z';
  if( 1 != read( thePipeFDs[0], &ch, 1 ) )
  {
    DBGOUT_FATAL( Debug::Prefix() << "Could not read from event pipe\n" );
    throw std::runtime_error( "EventProcessor: Could not read from event pipe" );
  }

  return EventPresent;
} //}}}


} // end namespace Event

/* {{{ Modeline for ViM
 * vim600:fdm=marker fdl=0 fdc=3:
 * }}} */
