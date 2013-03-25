/*! {{{
  \file TimerSystem.h

  \brief

  }}} */

#ifndef TIMERSYSTEM_H
#define TIMERSYSTEM_H

#include <boost/circular_buffer.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace Event
{

class TimerSystem
{
public:
  TimerSystem();

protected:
  enum { TIMERQ_MAX_SIZE = 256 };

  void StartTimer( unsigned char aID, unsigned long aDelay );
  void StartZyclicTimer( unsigned char aID, unsigned long aDelay );

  void StopTimer( unsigned char aID );
  bool PauseTimer( unsigned char aID );
  bool ContinueTimer( unsigned char aID );

  bool IsTimerActive( unsigned char aID );
  bool IsTimerPaused( unsigned char aID );


  //! \brief get maximal wait time for next timer
  //! \return 'first' == false -- no wait, 'first' == true, wait maximal 'second' time
  std::pair<bool,long int> GetMaxWaitTime() const;

  //! \brief get next timer, restart cyclic timer
  //! \return 'first' == false -- no active timer, 'first' == true, in 'second' is timer id
  std::pair<bool,unsigned char> GetNextTimer();

private:
  struct TimerStorage
  {
    TimerStorage( unsigned int aID, boost::posix_time::ptime aTime, unsigned long aDelay,  bool aCyclic )
      : theID( aID ), theCyclic( aCyclic ), theDelay( aDelay ), theTime( aTime ), theMillisecondsToActivate( 0 )
    {
    }
    unsigned char theID;
    bool theCyclic;
    unsigned long theDelay;
    boost::posix_time::ptime theTime;
    unsigned long theMillisecondsToActivate;


    bool operator<( const TimerStorage & other ) const { return theTime < other.theTime; }
  };

  void PushTimer( const TimerStorage & aTimer );

  boost::circular_buffer< TimerStorage > theTimerQueue;
  boost::circular_buffer< TimerStorage > thePausedTimerQueue;
};

}

#endif /* ifndef TIMERSYSTEM_H */

/* {{{ Modeline for ViM
 * vim600:fdm=marker fdl=0 fdc=3:
 * }}} */

