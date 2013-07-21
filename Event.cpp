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

using namespace boost::posix_time;
using namespace boost::lambda;

namespace Event
{

class EventProcessorCollection //{{{
{
public:
  void RegisterProcessor( EventProcessor* aProcessor );
  void UnRegisterProcessor( EventProcessor* aProcessor );

  void Broadcast( const EventPointer & aEvent );

private:

  typedef std::vector<EventProcessor*> ProcessorStorage;
  ProcessorStorage theProcessors;

  boost::mutex theLock;
}; //}}}

void EventProcessorCollection::RegisterProcessor( EventProcessor* aProcessor ) //{{{
{
  DEBUG_TRACER;
  boost::lock_guard<boost::mutex> guard( theLock );

  ProcessorStorage::const_iterator it = std::find( theProcessors.begin(), theProcessors.end(), aProcessor );
  if( it != theProcessors.end() )
  {
    DBGOUT_FATAL( Debug::Prefix() << "Error in RegisterProcessor\n" );
    throw std::runtime_error( "RegisterProcessor: object already present" );
  }

  theProcessors.push_back( aProcessor );
} //}}}

void EventProcessorCollection::UnRegisterProcessor( EventProcessor* aProcessor ) //{{{
{
  DEBUG_TRACER;
  boost::lock_guard<boost::mutex> guard( theLock );

  ProcessorStorage::iterator it = std::find( theProcessors.begin(), theProcessors.end(), aProcessor );
  if( it == theProcessors.end() )
  {
    DBGOUT_FATAL( Debug::Prefix() << "Error in UnRegisterProcessor\n" );
    throw std::runtime_error( "RegisterProcessor: object not found" );
  }

  theProcessors.erase( it );
} //}}}

void EventProcessorCollection::Broadcast( const EventPointer & aEvent ) //{{{
{
  DEBUG_TRACER;
  boost::lock_guard<boost::mutex> guard( theLock );

  for( ProcessorStorage::iterator it = theProcessors.begin(); it != theProcessors.end(); ++ it )
  {
    if( (*it)->IsEventOfInteres( aEvent ) )
      (*it)->PushEvent( aEvent );
  }
} //}}}

typedef Loki::SingletonHolder< EventProcessorCollection > ProcessorsSingleton;

void SendEvent( const EventPointer & aEvent ) //{{{
{
  DEBUG_TRACER;
  ProcessorsSingleton::Instance().Broadcast( aEvent );
} //}}}

EventProcessor::EventProcessor( unsigned int aID ) //{{{
  : theID( aID ), theEventQueue( EVENTQ_MAX_SIZE )
{
  if( -1 == pipe( thePipeFDs ) )
  {
    DBGOUT_FATAL( Debug::Prefix() << "Could not create event pipe\n" );
    throw std::runtime_error( "EventProcessor: Could not create event pipe" );
  }

  ProcessorsSingleton::Instance().RegisterProcessor( this );
}//}}}

EventProcessor::~EventProcessor() //{{{
{
  ProcessorsSingleton::Instance().UnRegisterProcessor( this );
} //}}}

void EventProcessor::PushEvent( const EventPointer & aEvent ) //{{{
{
  DEBUG_TRACER;
  boost::lock_guard<boost::mutex> guard( theLock );

  Assert( !theEventQueue.full() );

  theEventQueue.push_back( aEvent );

  char ch = 'A';
  if( 1 != write( thePipeFDs[1], &ch, 1 ) )
  {
    DBGOUT_FATAL( Debug::Prefix() << "Could not write to event pipe\n" );
    throw std::runtime_error( "EventProcessor: Could not write to event pipe" );
  }
} //}}}

EventProcessor::EventResult EventProcessor::GetEvent( EventPointer & aEvent, long aMaxWaitTime /*= WAIT_FOREWER*/ ) //{{{
{
  DEBUG_TRACER;

  if( aMaxWaitTime != NO_WAIT )
  {
    pollfd pollFD;
    pollFD.fd = thePipeFDs[0];
    pollFD.events = POLLIN;


    poll( &pollFD, 1, aMaxWaitTime );


    if( !( pollFD.revents & POLLIN ) )
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

void EventProcessor::Run() //{{{
{
  DEBUG_TRACER;

  long timeToWait = -1;
  while( true )
  {
    EventPointer event;
    std::pair<bool,long int> needWait = GetMaxWaitTime();
    if( needWait.first )
      timeToWait = needWait.second;
    else
      timeToWait = -1;

    DBGOUT_DEBUG( Debug::Prefix() << "EventProcessor(" << GetID() << ")::Run needWait " << needWait.first << ", " << needWait.second << " timeToWait " << timeToWait << std:: endl );

    if( GetEvent( event, timeToWait ) == EventPresent )
    {
      OnEvent( event );

      if( event->ID() == EVENT_FINISH )
        break;
    }

    std::pair<bool,unsigned char> timer = GetNextTimer();
    DBGOUT_DEBUG( Debug::Prefix() << "EventProcessor(" << GetID() << ")::Run timer " << timer.first << ", " << static_cast<int>( timer.second ) << std:: endl );
    if( timer.first )
    {
      EventPointer ptr = EventPointer( new Event( TIMER_ELAPSED( timer.second ) ) );
      OnEvent( ptr );
    }
  }
} //}}}

void EventProcessor::OnEvent( const EventPointer & aEvent ) //{{{
{
  DEBUG_TRACER;

  DBGOUT_DEBUG( Debug::Prefix() << "EventProcessor(" << GetID() << ")::OnEvent event id " << std::hex << aEvent->ID() << std::dec << std:: endl );
} //}}}

bool EventProcessor::IsEventOfInteres( const EventPointer & aEvent ) const //{{{
{
  return IsSystemEvent( aEvent ) || IsUserEventOfInteres( aEvent );
} //}}}

bool EventProcessor::IsSystemEvent( const EventPointer & aEvent ) const //{{{
{
  switch( aEvent->ID() )
  {
    case EVENT_INIT: //FALL THROW
    case EVENT_ENTRY: //FALL THROW
    case EVENT_EXIT: //FALL THROW
    case EVENT_START: //FALL THROW
    case EVENT_FINISH:
      return true;
  }

  return false;
} //}}}

} // end namespace Event

/* {{{ Modeline for ViM
 * vim600:fdm=marker fdl=0 fdc=3:
 * }}} */
