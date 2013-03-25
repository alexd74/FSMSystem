/*! {{{ File head comment
  \file main.cpp

  \brief

  }}} */

#include <iostream>
#include <unistd.h>

#include "Debug.h"
#include "Event.h"
#include "ActiveObject.h"
#include "hfsm.hpp"
#include "TestFSM.hpp"

using namespace Debug;
using namespace Event;

enum UserEvents //{{{
{
  UserEvent1 = 0x00000001,
  UserEvent2 = 0x00000002,
  UserEvent3 = 0x00000003,
  UserSignalA = 'a',
  UserSignalB = 'b',
  UserSignalC = 'c',
  UserSignalD = 'd',
  UserSignalE = 'e',
  UserSignalF = 'f',
  UserSignalG = 'g',
  UserSignalH = 'h'
}; //}}}

//{{{ EventTimedProcessor
class EventTimedProcessor : public EventProcessor
{
public:
  EventTimedProcessor( unsigned int aID ) : EventProcessor( aID ), theCounter( 0 )
  {
  }

protected:
  virtual void OnEvent( const EventPointer & aEvent );
  virtual bool IsUserEventOfInteres( const EventPointer & aEvent ) const;

private:
  int theCounter;
};


void EventTimedProcessor::OnEvent( const EventPointer & aEvent ) //{{{
{
  DEBUG_TRACER;
  EventProcessor::OnEvent( aEvent );

  switch( aEvent->ID() )
  {
    case EVENT_INIT:
      theCounter = 0;
      StartZyclicTimer( 2, 3000 );
      break;

    case UserEvent3:
      break;


    case TIMER_ELAPSED( 1 ):
      SendEvent( EventPointer( new Event::Event( UserEvent1 ) ) );
      break;

    case TIMER_ELAPSED( 2 ):
      ++theCounter;
      DBGOUT_DEBUG( Debug::Prefix() << "EventProcessor(" << GetID() << ")::timer 2 elapsed, counter " << theCounter << std::endl );
      SendEvent( EventPointer( new Event::Event( UserEvent2 ) ) );
      switch( theCounter )
      {
        case 3:
          StartTimer( 1, 8000 );
          break;

        case 5:
          PauseTimer( 1 );
          break;

        case 7:
          ContinueTimer( 1 );
          break;

        case 10:
          theCounter = 0;
          break;
      }
      break;

  }

} //}}}

bool EventTimedProcessor::IsUserEventOfInteres( const EventPointer & aEvent ) const
{
  return aEvent->ID() == UserEvent3;
}
//}}}

//{{{ EventProcessorReciever
class EventProcessorReciever : public EventProcessor
{
public:
  EventProcessorReciever( unsigned int aID ) : EventProcessor( aID )
  {
  }
protected:
  virtual void OnEvent( const EventPointer & aEvent );
  virtual bool IsUserEventOfInteres( const EventPointer & aEvent ) const;
};

void EventProcessorReciever::OnEvent( const EventPointer & aEvent ) //{{{
{
  DEBUG_TRACER;
  EventProcessor::OnEvent( aEvent );

  switch( aEvent->ID() )
  {
    case EVENT_INIT:
      break;

    case UserEvent1:
      SendEvent( EventPointer( new Event::Event( UserEvent3 ) ) );
      break;

    case UserEvent2:
      break;
  }

} //}}}

bool EventProcessorReciever::IsUserEventOfInteres( const EventPointer & aEvent ) const
{
  return aEvent->ID() == UserEvent1 || aEvent->ID() == UserEvent2;
}
//}}}

//{{{ EventProcessorTestFSM
class EventProcessorTestFSM : public EventProcessor
{
public:
  EventProcessorTestFSM( unsigned int aID ) : EventProcessor( aID )
  {
  }
protected:
  virtual void OnEvent( const EventPointer & aEvent );
  virtual bool IsUserEventOfInteres( const EventPointer & aEvent ) const;

private:
  TestHSM theTestFSM;
};

void EventProcessorTestFSM::OnEvent( const EventPointer & aEvent ) //{{{
{
  DEBUG_TRACER;
  EventProcessor::OnEvent( aEvent );

  theTestFSM.dispatch( (Signal)( aEvent->ID() - 'a' ) );
} //}}}

bool EventProcessorTestFSM::IsUserEventOfInteres( const EventPointer & aEvent ) const
{
  return aEvent->ID() >= UserSignalA && aEvent->ID() <= UserSignalH;
}
//}}}

int main( int argc, char *argv[] ) //{{{
{
  try
  {

    ParseDebugOptions( argc, argv );

    LogEnable( all, true );
    LogEnable( tracing, true );
    EnableAllTrace( true );

    AttachStream( all, std::cout );

    SetAssertionMode( all, ASSERT_CONTINUE_CORE );

    EnableTimePrefix( true );
    EnableTimePrefixUS( true );

    DEBUG_TRACER;

//    Event::EventProcessor processor( 1 );

    EventTimedProcessor processor2( 2 );
    EventProcessorReciever processor3( 3 );

    EventProcessorTestFSM processor4( 4 );

    ActiveObject object2( processor2 );
    ActiveObject object3( processor3 );
    ActiveObject object4( processor4 );

    object2.Start();
    object3.Start();
    object4.Start();

    DBGOUT_CURR_POS;

    sleep( 1 );

    object2.PushEvent( EventPointer( new Event::Event( EVENT_INIT ) ) );
    object3.PushEvent( EventPointer( new Event::Event( EVENT_INIT ) ) );
    object4.PushEvent( EventPointer( new Event::Event( EVENT_INIT ) ) );

    DBGOUT_CURR_POS;

    sleep( 1 );

//    processor.PushEvent( EventPointer( new Event::Event( EVENT_INIT ) ) );

//    processor.PushEvent( EventPointer( new Event::Event( EVENT_START ) ) );
//    processor.PushEvent( EventPointer( new Event::Event( EVENT_FINISH ) ) );

    DBGOUT_CURR_POS;

    object2.PushEvent( EventPointer( new Event::Event( EVENT_START ) ) );
    object3.PushEvent( EventPointer( new Event::Event( EVENT_START ) ) );
    object4.PushEvent( EventPointer( new Event::Event( EVENT_START ) ) );

    DBGOUT_CURR_POS;

//    processor.Run();

//    DBGOUT_CURR_POS;

//    sleep( 5 );
    for(;;)
    {
      printf("\nSignal<-");

      char c = getc(stdin);
      getc( stdin ); // discard '\n'

      if( c<'a' || 'h'<c )
      {
        break;
      }

//      test.dispatch((Signal)(c-'a'));
      SendEvent( EventPointer( new Event::Event( c ) ) );
    }

    object2.Stop();
    object3.Stop();
    object4.Stop();

  }
  catch ( const std::exception & ex )
  {
    std::cout << "Error: " << ex.what() << std::endl;
  }
  catch ( ... )
  {
    std::cout << "unknown error" << std::endl;
  }

  return 0;
} //}}}

/* {{{ Modeline for ViM
 * vim600:fdm=marker fdl=0 fdc=3:
 * }}} */
