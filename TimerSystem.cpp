/*! {{{ File head comment
  \file TimerSystem.c

  \brief

 }}} */

#include "TimerSystem.h"
#include "Debug.h"

#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>

using namespace boost::posix_time;
using namespace boost::lambda;

namespace Event
{

TimerSystem::TimerSystem(): theTimerQueue( TIMERQ_MAX_SIZE ), thePausedTimerQueue( TIMERQ_MAX_SIZE )
{

}

void TimerSystem::StartTimer( unsigned char aID, unsigned long aDelay ) //{{{
{
  DEBUG_TRACER;

  StopTimer( aID );

  PushTimer( TimerStorage( aID, microsec_clock::local_time() + millisec( aDelay ), aDelay, false ) );
} //}}}

void TimerSystem::StartZyclicTimer( unsigned char aID, unsigned long aDelay ) //{{{
{
  DEBUG_TRACER;

  StopTimer( aID );

  PushTimer( TimerStorage( aID, microsec_clock::local_time() + millisec( aDelay ), aDelay, true ) );
} //}}}

void TimerSystem::PushTimer( const TimerStorage & aTimer ) //{{{
{
  DEBUG_TRACER;

  Assert( !theTimerQueue.full() );

  if( theTimerQueue.empty() )
  {
    theTimerQueue.push_back( aTimer );
  }
  else
  {
    boost::circular_buffer< TimerStorage >::iterator pos =
      std::upper_bound( theTimerQueue.begin(), theTimerQueue.end() , aTimer );

    theTimerQueue.insert( pos, aTimer );
  }
} //}}}

void TimerSystem::StopTimer( unsigned char aID ) //{{{
{
  DEBUG_TRACER;

  boost::circular_buffer< TimerStorage >::iterator it = std::find_if( theTimerQueue.begin(), theTimerQueue.end(),
    bind( &TimerStorage::theID, boost::lambda::_1 ) == aID );

  if( it != theTimerQueue.end() )
  {
    DBGOUT_DEBUG( Debug::Prefix() << "StopTimer with id " << static_cast<int>( aID ) << std::endl );
    theTimerQueue.erase( it );
  }

  boost::circular_buffer< TimerStorage >::iterator pit = std::find_if( thePausedTimerQueue.begin(), thePausedTimerQueue.end(),
    bind( &TimerStorage::theID, boost::lambda::_1 ) == aID );

  if( pit != thePausedTimerQueue.end() )
  {
    thePausedTimerQueue.erase( pit );
  }
} //}}}

bool TimerSystem::PauseTimer( unsigned char aID ) //{{{
{
  DEBUG_TRACER;

  boost::circular_buffer< TimerStorage >::iterator it = std::find_if( theTimerQueue.begin(), theTimerQueue.end(),
    bind( &TimerStorage::theID, boost::lambda::_1 ) == aID );

  if( it != theTimerQueue.end() )
  {
    time_duration timeToActivate = it->theTime - microsec_clock::local_time();
    it->theMillisecondsToActivate = timeToActivate.total_milliseconds();

    thePausedTimerQueue.push_back( *it );
    theTimerQueue.erase( it );

    return true;
  }

  return false;
} //}}}

bool TimerSystem::ContinueTimer( unsigned char aID ) //{{{
{
  DEBUG_TRACER;

  boost::circular_buffer< TimerStorage >::iterator it = std::find_if( thePausedTimerQueue.begin(), thePausedTimerQueue.end(),
    bind( &TimerStorage::theID, boost::lambda::_1 ) == aID );

  if( it != thePausedTimerQueue.end() )
  {
    it->theTime = microsec_clock::local_time() + millisec( it->theMillisecondsToActivate );

    PushTimer( *it );
    thePausedTimerQueue.erase( it );

    return true;
  }

  return false;
} //}}}

bool TimerSystem::IsTimerActive( unsigned char aID ) //{{{
{
  DEBUG_TRACER;

  boost::circular_buffer< TimerStorage >::const_iterator it = std::find_if( theTimerQueue.begin(), theTimerQueue.end(),
    bind( &TimerStorage::theID, boost::lambda::_1 ) == aID );

  return ( it != theTimerQueue.end() );
} //}}}

bool TimerSystem::IsTimerPaused( unsigned char aID ) //{{{
{
  DEBUG_TRACER;

  boost::circular_buffer< TimerStorage >::const_iterator it = std::find_if( thePausedTimerQueue.begin(), thePausedTimerQueue.end(),
    bind( &TimerStorage::theID, boost::lambda::_1 ) == aID );

  return ( it != thePausedTimerQueue.end() );
} //}}}

std::pair<bool,long int> TimerSystem::GetMaxWaitTime() const //{{{
{
  DEBUG_TRACER;

  if( theTimerQueue.empty() )
    return std::make_pair( false, 0L );

  time_duration timeToActivate = theTimerQueue.front().theTime - microsec_clock::local_time();

  long milliseconds = timeToActivate.total_milliseconds();

  if( milliseconds <= 0 )
    return std::make_pair( true, 0L );

  return std::make_pair( true, milliseconds );
} //}}}

std::pair<bool,unsigned char> TimerSystem::GetNextTimer() //{{{
{
  DEBUG_TRACER;

  if( theTimerQueue.empty() )
  {
    DBGOUT_DEBUG( Debug::Prefix() << "TimerSystem::GetNextTimer queue empty " << std::endl );
    return std::make_pair( false, 0 );
  }

  std::pair<bool,long int> needWait = GetMaxWaitTime();
  if( needWait.first && needWait.second == 0 )
  {
    unsigned char id = theTimerQueue.front().theID;
    if( theTimerQueue.front().theCyclic )
    {
      unsigned long delay = theTimerQueue.front().theDelay;
      theTimerQueue.pop_front();
      StartZyclicTimer( id, delay );
    }
    else
      theTimerQueue.pop_front();

    return std::make_pair( true, id );
  }

  return std::make_pair( false, 0 );
} //}}}


} // end namespace Event

/* {{{ Modeline for ViM
 * vim600:fdm=marker fdl=0 fdc=3:
 * }}} */
