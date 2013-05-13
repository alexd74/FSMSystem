/*! {{{ File head comment
  \file Debug.cpp

  \brief



  \author Alexey Dyachenko <alexd@inbox.ru>
  \date   2002-07-23 10:38

  Last modified: 2004-04-20 16:12
  }}} */

//{{{ Includes
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <getopt.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include <map>

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <execinfo.h>
#include <sys/time.h>
#include <pthread.h>
#include <boost/format.hpp>
//}}}

#ifndef DEBUG_ENABLED
#define DEBUG_ENABLED
#endif

//#include "Log.h"
#include "Debug.h"

namespace Debug
{

boost::recursive_mutex & GetLock()
{
  static boost::recursive_mutex mutex;

  return mutex;
};

//{{{ Program Options
//{{{ Options table
static struct option long_options[] = {
   {"enable-fatal"      , optional_argument , 0 , 0} ,
   {"disable-fatal"     , optional_argument , 0 , 0} ,
   {"enable-error"      , optional_argument , 0 , 0} ,
   {"disable-error"     , optional_argument , 0 , 0} ,
   {"enable-warning"    , optional_argument , 0 , 0} ,
   {"disable-warning"   , optional_argument , 0 , 0} ,
   {"enable-info"       , optional_argument , 0 , 0} ,
   {"disable-info"      , optional_argument , 0 , 0} ,
   {"enable-debug"      , optional_argument , 0 , 0} ,
   {"disable-debug"     , optional_argument , 0 , 0} ,
   {"enable-tracing"    , optional_argument , 0 , 0} ,
   {"disable-tracing"   , optional_argument , 0 , 0} ,
   {"enable-all-debug"  , optional_argument , 0 , 0} ,
   {"disable-all-debug" , optional_argument , 0 , 0} ,
   {"enable-cerr"       , optional_argument , 0 , 0} ,
   {"disable-cerr"      , optional_argument , 0 , 0} ,
   {"enable-level-prefix", 0 , 0 , 0} ,
   {"disable-level-prefix", 0, 0 , 0} ,
   {"enable-time-prefix", 0 , 0 , 0} ,
   {"disable-time-prefix", 0, 0 , 0} ,
   {"enable-profiling", 0 , 0 , 0} ,
   {"disable-profiling", 0, 0 , 0} ,
   {"enable-cout"       , optional_argument , 0 , 0} ,
   {"disable-cout"      , optional_argument , 0 , 0} ,
   {0                   , 0                 , 0 , 0}
};//}}}

typedef std::vector<std::string> TStringVector;

//{{{ String tokeniser
TStringVector StrTok( const std::string &aString, char aSymbol )
{
   TStringVector  aResult;

   std::string::const_iterator aTokenBegin = aString.begin(), aTokenEnd = aString.begin();

   while( aTokenBegin != aString.end() )
   {
      aTokenEnd = std::find( aTokenBegin, aString.end(), aSymbol );
      if( aTokenBegin != aTokenEnd )
         aResult.push_back( std::string( aTokenBegin, aTokenEnd ) );

      aTokenBegin = aTokenEnd;
      if( aTokenBegin != aString.end() )
         ++aTokenBegin;
   };
   return aResult;
};//}}}

//{{{ ParseDebugOptions
bool ParseDebugOptions( int argc, char ** argv )
{
   int c;

   // Enable fatal and error levels by default
   Debug::LogEnable( Debug::fatal, true );
   Debug::LogEnable( Debug::error, true );

   while (1) 
   {
      int option_index = 0;

      c = getopt_long (argc, argv, "",
            long_options, &option_index);

      if( c == -1 )
         break;

      if( c == 0 )
      {
         DBGOUT_DEBUG( Prefix( debug ) << "Debug::ParseDebugOptions  option " << long_options[option_index].name << std::endl );
         if (optarg)
            DBGOUT_DEBUG( Prefix( debug ) << "Debug::ParseDebugOptions   with arg " << optarg << "\n" );
         switch( option_index ) 
         {
            case 0:
               Debug::LogEnable( Debug::fatal, true );
               break;
            case 1:
               Debug::LogEnable( Debug::fatal, false );
               break;
            case 2:
               Debug::LogEnable( Debug::error, true );
               break;
            case 3:
               Debug::LogEnable( Debug::error, false );
               break;
            case 4:
               Debug::LogEnable( Debug::warning, true );
               break;
            case 5:
               Debug::LogEnable( Debug::warning, false );
               break;
            case 6:
               Debug::LogEnable( Debug::info, true );
               break;
            case 7:
               Debug::LogEnable( Debug::info, false );
               break;
            case 8:
               Debug::LogEnable( Debug::debug, true );
               break;
            case 9:
               Debug::LogEnable( Debug::debug, false );
               break;
            case 10:
               {
                  if( !optarg || ( strcmp( optarg, "all" ) == 0 ) )
                  {
                     Debug::LogEnable( Debug::tracing, true );
                     Debug::EnableAllTrace( true );
                  }
                  else
                     if( optarg )
                     {
                        TStringVector  aTokens = StrTok( std::string( optarg ), ',' );
                        for( TStringVector::const_iterator aCurrentToken = aTokens.begin();
                              aCurrentToken != aTokens.end(); ++aCurrentToken )
                           Debug::LogEnable( Debug::tracing, aCurrentToken->c_str() , true );

                     }
               };
               break;
            case 11:
               {
                  if( !optarg || ( strcmp( optarg, "all" ) == 0 ) )
                  {
                     Debug::LogEnable( Debug::tracing, false );
                     Debug::EnableAllTrace( false );
                  }
                  else
                     if( optarg )
                     {
                        TStringVector  aTokens = StrTok( std::string( optarg ), ',' );
                        for( TStringVector::const_iterator aCurrentToken = aTokens.begin();
                              aCurrentToken != aTokens.end(); ++aCurrentToken )
                           Debug::LogEnable( Debug::tracing, aCurrentToken->c_str() , false );

                     }
               };
               break;
            case 12:
               Debug::LogEnable( Debug::all, true );
               Debug::EnableAllTrace( true );
               Debug::EnableProfiling( true );
               break;
            case 13:
               Debug::LogEnable( Debug::all, false );
               Debug::EnableAllTrace( false );
               Debug::EnableProfiling( false );
               break;
            case 14:
               Debug::AttachStream( Debug::all, std::cerr );
               break;
            case 15:
               Debug::DetachStream( Debug::all, std::cerr );
               break;
            case 16:
               Debug::EnableLevelPrefix( true );
               break;
            case 17:
               Debug::EnableLevelPrefix( false );
               break;
            case 18:
               Debug::EnableTimePrefix( true );
               break;
            case 19:
               Debug::EnableTimePrefix( false );
               break;
            case 20:
               Debug::EnableProfiling( true );
               break;
            case 21:
               Debug::EnableProfiling( false );
               break;
            case 22:
               Debug::AttachStream( Debug::all, std::cout );
               break;
            case 23:
               Debug::DetachStream( Debug::all, std::cout );
               break;
         }
      }
      else
		   DBGOUT_ERROR( Prefix( error ) << "?? getopt returned character code " << static_cast<char>(c) << std::endl );
   };


   return !(optind < argc);
};//}}}
//}}}

//{{{Log
const char *INDENT = "  ";
const char *PREFIX = "*** ";
const char *TRUE_STRING = "true";
const char *FALSE_STRING = "false";
const unsigned int ALL_SOURCES_MASK = 0xff;

const size_t PRINTF_MAX_REPEAT_COUNT = 32;

struct CPeriodData
{
   size_t       theTriggersCount;
   std::clock_t theTriggeredTime;

   CPeriodData();
};


struct CSourcePosCompare
{
   bool operator() ( const Debug::CSourcePos & a, const Debug::CSourcePos & b ) const
   {
      if( a.theFileName == b.theFileName )
      {
         if( a.theFuncName == b.theFuncName )
         {
            return a.theLine < b.theLine;
         }
         else
         {
            return a.theFuncName < b.theFuncName;
         }
      }
      else
      {
         return a.theFileName < b.theFileName;
      }
   }
};

typedef  std::map < std::string, unsigned int > TSourceMap;
typedef  std::map < Debug::CSourcePos, CPeriodData, CSourcePosCompare > TPeriodMap;


unsigned int theIndentDepth  = 0;
std::string  theIndentPrefix = PREFIX;
bool         theLevelPrefix  = false;
bool         theTimePrefix   = false;
bool         theTimeUSPrefix = false;
std::clock_t theTimePeriod   = 0;
TSourceMap   theSources;
TPeriodMap   thePeriodMap;

void IncIndent()
{
	++theIndentDepth;
}


void  DecIndent()
{
	--theIndentDepth;
}

size_t GetIndentDepth()
{
  return theIndentDepth;
}

unsigned int GetThreadID()
{
  return  static_cast<unsigned long>( pthread_self() ) ; // % 100;
}


CPeriodData::CPeriodData() : theTriggersCount( 0 ),
theTriggeredTime( std::clock() - theTimePeriod * 2 )
{
};

// {{{ Print helpers

void PrintPos( std::ostream & aOut, const Debug::CSourcePos & aWhere )
{
   if( aWhere.theFileName )
   {
      if( aWhere.theFuncName )
      {
         aOut << "function: " << aWhere.theFuncName << ", ";
      }
      aOut << "line: " << aWhere.theLine << ", file: " << aWhere.theFileName;
   }
}

void PrintPosShort( std::ostream & aOut, const Debug::CSourcePos & aWhere )
{
   if( aWhere.theFileName )
   {
      if( aWhere.theFuncName )
      {
         aOut << aWhere.theFuncName << " (" << aWhere.theLine
            << " in " << aWhere.theFileName << ")";
      }
      else
      {
         aOut << "function at (" << aWhere.theLine
            << " in " << aWhere.theFileName << ")";
      }
   }
}

void PrintPeriodInfo( std::ostream & aOut, const Debug::CSourcePos & aWhere )
{
   if( theTimePeriod )
   {
      size_t theTriggersCount = thePeriodMap[aWhere].theTriggersCount;
      aOut << " (triggered " << theTriggersCount << " time";
      if( theTriggersCount > 1 )
         aOut << "s)";
      else
         aOut << ")";
   }
}


void DoPrefix( Debug::LogLevel aLevel, std::ostream & aOStream )
{
  aOStream << boost::format( " Thread %1$2d: " ) % GetThreadID();

   if( theTimePrefix )
   {
      std::time_t aTime = std::time( 0 );
      struct timeval aCurrentTime;
      gettimeofday( &aCurrentTime, 0 );

      if( aTime != -1 )
      {
         aOStream << std::string( std::ctime( &aTime ), 24 ) ;
         if( theTimeUSPrefix )
         {
            aOStream << boost::format( ", %1$6d us " ) % aCurrentTime.tv_usec;
         };
         aOStream << ": ";
      }
   }
   if( theLevelPrefix )
   {
      switch ( aLevel )
      {
         case Debug::info:
            {
               aOStream << "   info: ";
               break;
            }
         case Debug::warning:
            {
               aOStream << "warning: ";
               break;
            }
         case Debug::error:
            {
               aOStream << "  error: ";
               break;
            }
         case Debug::fatal:
            {
               aOStream << "  fatal: ";
               break;
            }
         case Debug::tracing:
            {
               aOStream << "  trace: ";
               break;
            }
         case Debug::debug:
            {
               aOStream << "  debug: ";
               break;
            }
         case Debug::none:
            {
               break;
            }
         case Debug::all:
            {
               aOStream << "    all: ";
               break;
            }
      }
   }
};

// }}}

unsigned int DebugSourceMask( Debug::LogLevel aLevel )
{
   return ( aLevel != Debug::all ) ? 1 << aLevel : ALL_SOURCES_MASK;
}

bool DebugSourceEnabled( Debug::LogLevel aLevel, Debug::TDebugSourceName aSource )
{
   if( aSource )
   {
      if( theSources.find( aSource ) != theSources.end() )
      {
         return ( theSources[aSource] & DebugSourceMask( aLevel ) ) != 0;
      }
      else
      {
         theSources[aSource] = 0;
         return false;
      }
   }
   else
   {
      return true;
   }
}

bool PeriodAllowsImpl( const Debug::CSourcePos & aWhere )
{
   CPeriodData & aData = thePeriodMap[aWhere];
   aData.theTriggersCount++;
   if( aData.theTriggeredTime < std::clock() - theTimePeriod )
   {
      aData.theTriggeredTime = std::clock();
      return true;
   }
   else
   {
      return false;
   }
}

inline bool PeriodAllows( const Debug::CSourcePos & aWhere )
{
   return !theTimePeriod || PeriodAllowsImpl( aWhere );
}

// {{{ CPrintfOStream
int CPrintfOStream::printf(const char *format, ...)
{
//  DBGOUT_CURR_POS;

  va_list aArgumentList;
  va_start( aArgumentList, format );

  size_t aPrintedCharCounter = 0;
  if( ( aPrintedCharCounter = vsnprintf( thePrintfBuffer, sizeof(thePrintfBuffer), format, aArgumentList ) ) 
        < sizeof(thePrintfBuffer) - 1 )
  {
//     DBGOUT_DEBUG( Debug::Prefix() << "aPrintedCharCounter = " << aPrintedCharCounter << std::endl );
     write(thePrintfBuffer, aPrintedCharCounter );
     return aPrintedCharCounter;
  }

  size_t aBufSize = 2 * sizeof(thePrintfBuffer);

  while( true )
  {
     char aTmpBuffer[aBufSize];

     va_start( aArgumentList, format );
     if( ( aPrintedCharCounter = vsnprintf( aTmpBuffer, aBufSize, format, aArgumentList ) ) 
           < aBufSize - 1 || aBufSize > PRINTF_MAX_REPEAT_COUNT * sizeof(thePrintfBuffer) ) 
     {
//        DBGOUT_DEBUG( Debug::Prefix() << "aPrintedCharCounter = " << aPrintedCharCounter << std::endl );
        write( aTmpBuffer, aPrintedCharCounter );
        break;

     }
     else 
        aBufSize *= 2;
  };

//  DBGOUT_DEBUG( Debug::Prefix() << "aPrintedCharCounter = " << aPrintedCharCounter << std::endl );
  return aPrintedCharCounter;
};

// }}}

// {{{ CDbgOStream 

CDbgOStream::CDbgOStream( CLogStreamBuf *aBuffer )
    :CPrintfOStream( /*static_cast<std::streambuf *> */( aBuffer ) ), 
     theBuffer( aBuffer ),theLevel( none )
{
   if( !theBuffer )
      throw AssertException( CURR_POS, "DbgOStream: null buffer" ); 

};
CDbgOStream::~CDbgOStream()
{

};


void CDbgOStream::SetLogLevel( LogLevel aLevel )
{
   theBuffer->SetBufLogLevel( aLevel );
   theLevel = aLevel;
};

// }}}

// {{{ Multiplexor streambuf
class MultiplexorStreambuf : public CLogStreamBuf 
{
public:

   MultiplexorStreambuf( std::vector < std::ostream * > &aOStreams, int aBufSize = 0 );
   ~MultiplexorStreambuf();

   virtual void SetBufLogLevel( LogLevel aLevel );

protected:

   int   overflow( int );
   int   sync();

private:

   void  put_buffer( void );
   void  put_char( int );

   std::vector < std::ostream * > & theOStreams;
   LogLevel theCurrentLevel;
};

MultiplexorStreambuf::MultiplexorStreambuf( std::vector < std::ostream * > &aOStreams,
      int aBufSize )
: theOStreams( aOStreams ), theCurrentLevel( none )
{
   if( aBufSize )
   {
      char  * aBuffer = new char[aBufSize];
      setp(  aBuffer,  aBuffer + aBufSize );
   }
   else
   {
      setp( 0, 0 );
   }
   setg( 0, 0, 0 );
};

MultiplexorStreambuf::~MultiplexorStreambuf()
{
   sync();
   delete[] pbase();
}

int  MultiplexorStreambuf::overflow( int c )
{
   put_buffer();
   if( c != EOF )
   {
      if( pbase() == epptr() )
      {
         put_char( c );
      }
      else
      {
         sputc( c );
      }
   }
   return 0;
}

int  MultiplexorStreambuf::sync()
{
   put_buffer();
   return 0;
}

void  MultiplexorStreambuf::put_buffer( void )
{
   if( pbase() != pptr() )
   {
      std::vector < std::ostream * >::iterator i = theOStreams.begin();
      while( i != theOStreams.end() )
      {
         ( *i )->write( pbase(), pptr() - pbase() );
         ++i;
      }
      setp( pbase(), epptr() );
   }
}

void MultiplexorStreambuf::put_char( int c )
{
   std::vector < std::ostream * >::iterator i = theOStreams.begin();
   while( i != theOStreams.end() )
   {
      ( **i ) << static_cast < char >( c );
      i++;
   }
}
void MultiplexorStreambuf::SetBufLogLevel( LogLevel aLevel )
{
   // TODO work with levels streambufs
   theCurrentLevel = aLevel;
};

// }}}

// {{{ NullStreambuf
class  CNullStreambuf:public CLogStreamBuf 
{
public:

   CNullStreambuf()
   { }

   ~CNullStreambuf()
   { }

   virtual void SetBufLogLevel( LogLevel ){ };

protected:

   int overflow( int )
   {
      return 0;
   }
   int sync()
   {
      return 0;
   }
};
// }}}

void AddOStreamToVector( std::vector<std::ostream*> &aStreamsVector, std::ostream * aStream )
{
   if( std::find( aStreamsVector.begin(), aStreamsVector.end(),  aStream ) ==
         aStreamsVector.end() )
   {
      aStreamsVector.push_back(  aStream );
   }
}

void RemoveOStreamFromVector( std::vector<std::ostream*> &aStreamsVector, std::ostream * aStream )
{
   std::vector < std::ostream * >::iterator aFoundedStream =
      std::find( aStreamsVector.begin(), aStreamsVector.end(), aStream );
   if( aFoundedStream != aStreamsVector.end() )
   {
      aStreamsVector.erase( aFoundedStream );
   }
};

/*
 * ostream vectors
 */
std::vector<std::ostream*>  theInfoStreams;
std::vector<std::ostream*>  theWarningStreams;
std::vector<std::ostream*>  theErrorStreams;
std::vector<std::ostream*>  theFatalStreams;
std::vector<std::ostream*>  theTracingStreams;
std::vector<std::ostream*>  theDebugStreams;

/*
 * ostreams
 */
CDbgOStream InfoStream(    new MultiplexorStreambuf( theInfoStreams ) );
CDbgOStream WarningStream( new MultiplexorStreambuf( theWarningStreams ) );
CDbgOStream ErrorStream(   new MultiplexorStreambuf( theErrorStreams ) );
CDbgOStream FatalStream(   new MultiplexorStreambuf( theFatalStreams ) );
CDbgOStream TracingStream( new MultiplexorStreambuf( theTracingStreams ) );
CDbgOStream DebugStream(   new MultiplexorStreambuf( theDebugStreams ) );
CDbgOStream NullStream(    new CNullStreambuf() );

//CDbgOStream InfoStream(    new MultiplexorStreambuf( theInfoStreams     , 22048) );
//CDbgOStream WarningStream( new MultiplexorStreambuf( theWarningStreams  , 22048) );
//CDbgOStream ErrorStream(   new MultiplexorStreambuf( theErrorStreams    , 22048) );
//CDbgOStream FatalStream(   new MultiplexorStreambuf( theFatalStreams    , 22048) );
//CDbgOStream TracingStream( new MultiplexorStreambuf( theTracingStreams  , 22048) );
//CDbgOStream DebugStream(   new MultiplexorStreambuf( theDebugStreams    , 22048) );
//CDbgOStream NullStream(    new CNullStreambuf() );

/*
 * enables
 */
bool FatalLevelEnabled   = true;
bool ErrorLevelEnabled   = true;
bool WarningLevelEnabled = false;
bool InfoLevelEnabled    = false;
bool DebugLevelEnabled   = false;
bool TracingLevelEnabled = false;

const char * GetLevelName( LogLevel aLevel )
{
   switch( aLevel ) 
   {
      case fatal:    return "fatal";   break;
      case error:    return "error";   break;
      case warning:  return "warning"; break;
      case info:     return "info";    break;
      case debug:    return "debug";   break;
      case tracing:  return "tracing"; break;
      case none:     return "none";    break;
      case all:      return "all";     break;
      default :      return "unknown"; break;
   }
   return "unknown";
};


//{{{ Log enable/disable
void LogEnable( LogLevel aLevel, bool aEnabled )
{
   DebugStream << Prefix( debug ) << "Debug::LogEnable(" << GetLevelName( aLevel )
      << "," << ( aEnabled ? TRUE_STRING : FALSE_STRING ) << ")\n";

   static bool initialised = false;
   if( !initialised )
   {
      AddOStreamToVector( theErrorStreams, &std::cerr );
      AddOStreamToVector( theFatalStreams, &std::cerr );
      initialised = true;
   }

   if( aLevel == Debug::info || aLevel == Debug::all )
   {
      InfoLevelEnabled = aEnabled;
   }
   if( aLevel == Debug::warning || aLevel == Debug::all )
   {
      WarningLevelEnabled = aEnabled;
   }
   if( aLevel == Debug::error || aLevel == Debug::all )
   {
      ErrorLevelEnabled = aEnabled;
   }
   if( aLevel == Debug::fatal || aLevel == Debug::all )
   {
      FatalLevelEnabled = aEnabled;
   }
   if( aLevel == Debug::debug || aLevel == Debug::all )
   {
      DebugLevelEnabled = aEnabled;
   }
   if( aLevel == Debug::tracing || aLevel == Debug::all )
   {
      TracingLevelEnabled = aEnabled;
   }
};

void LogEnable( LogLevel aLevel, TDebugSourceName aSource,
      bool aEnabled )
{
   DebugStream << Prefix( debug ) << "Debug::LogEnable(" << GetLevelName( aLevel )
      << ",\"" << aSource << "\","
      << ( aEnabled ? TRUE_STRING : FALSE_STRING ) << ")\n";

   if( DebugSourceEnabled( aLevel, aSource ) )
   {
      if( !aEnabled )
         theSources[aSource] &= ~DebugSourceMask( aLevel );
   }
   else
   {
      if( aEnabled )
         theSources[aSource] |= DebugSourceMask( aLevel );
   }
}


void LogEnableAll( LogLevel aLevel, bool aEnabled )
{
   DebugStream << Prefix( debug ) << "Debug::LogEnableAll("
      << GetLevelName( aLevel ) << ","
      << ( aEnabled ? TRUE_STRING : FALSE_STRING ) << ")\n";

   TSourceMap::iterator i = theSources.begin();
   while( i != theSources.end() )
   {
      i->second &= ~DebugSourceMask( aLevel );
      if( aEnabled )
      {
         i->second |= DebugSourceMask( aLevel );
      }
      i++;
   }
}//}}}

CDbgOStream & DBGOUT( LogLevel aLevel, TDebugSourceName aSource )
{
   if( !DebugSourceEnabled( aLevel, aSource ) )
   {
      aLevel = none;
   }

   switch ( aLevel )
   {
      case info:
         {
            if( InfoLevelEnabled )
            {
               return InfoStream;
            }
            else
            {
               return NullStream;
            }
            break;
         }
      case warning:
         {
            if( WarningLevelEnabled )
            {
               return WarningStream;
            }
            else
            {
               return NullStream;
            }
            break;
         }
      case error:
         {
            if( ErrorLevelEnabled )
            {
               return ErrorStream;
            }
            else
            {
               return NullStream;
            }
            break;
         }
      case fatal:
         {
            if( FatalLevelEnabled )
            {
               return FatalStream;
            }
            else
            {
               return NullStream;
            }
            break;
         }
      case tracing:
         {
            if( TracingLevelEnabled )
            {
               return TracingStream;
            }
            else
            {
               return NullStream;
            }
            break;
         }
      case debug:
         {
            if( DebugLevelEnabled )
            {
               return DebugStream;
            }
            else
            {
               return NullStream;
            }
            break;
         }
      case none: // FALL THROUGH
      default:
         {
            return NullStream;
            break;
         }
   }
}


void AttachStream( LogLevel aLevel, std::ostream &aStream )
{
   DebugStream << Prefix( debug ) << "Debug::AttachStream("
      << GetLevelName( aLevel ) << ",ostream)\n";

   if( aLevel == Debug::info || aLevel == Debug::all )
   {
      AddOStreamToVector( theInfoStreams, &aStream );
   }
   if( aLevel == Debug::warning || aLevel == Debug::all )
   {
      AddOStreamToVector( theWarningStreams, &aStream );
   }
   if( aLevel == Debug::error || aLevel == Debug::all )
   {
      AddOStreamToVector( theErrorStreams, &aStream );
   }
   if( aLevel == Debug::fatal || aLevel == Debug::all )
   {
      AddOStreamToVector( theFatalStreams, &aStream );
   }
   if( aLevel == Debug::tracing || aLevel == Debug::all )
   {
      AddOStreamToVector( theTracingStreams, &aStream );
   }
   if( aLevel == Debug::debug || aLevel == Debug::all )
   {
      AddOStreamToVector( theDebugStreams, &aStream );
   }
}


void DetachStream( LogLevel aLevel, std::ostream & aStream )
{
   if( aLevel == Debug::info || aLevel == Debug::all )
   {
      RemoveOStreamFromVector( theInfoStreams, &aStream );
   }
   if( aLevel == Debug::warning || aLevel == Debug::all )
   {
      RemoveOStreamFromVector( theWarningStreams, &aStream );
   }
   if( aLevel == Debug::error || aLevel == Debug::all )
   {
      RemoveOStreamFromVector( theErrorStreams, &aStream );
   }
   if( aLevel == Debug::fatal || aLevel == Debug::all )
   {
      RemoveOStreamFromVector( theFatalStreams, &aStream );
   }
   if( aLevel == Debug::tracing || aLevel == Debug::all )
   {
      RemoveOStreamFromVector( theTracingStreams, &aStream );
   }
   if( aLevel == Debug::debug || aLevel == Debug::all )
   {
      RemoveOStreamFromVector( theDebugStreams, &aStream );
   }
}


void DetachAllStreams( LogLevel aLevel )
{
   DebugStream << Prefix( debug ) << "Debug::DetachAllStreams("
      << GetLevelName( aLevel ) << ")\n";

   if( aLevel == Debug::info || aLevel == Debug::all )
   {
      theInfoStreams.clear();
   }
   if( aLevel == Debug::warning || aLevel == Debug::all )
   {
      theWarningStreams.clear();
   }
   if( aLevel == Debug::error || aLevel == Debug::all )
   {
      theErrorStreams.clear();
   }
   if( aLevel == Debug::fatal || aLevel == Debug::all )
   {
      theFatalStreams.clear();
   }
   if( aLevel == Debug::tracing || aLevel == Debug::all )
   {
      theTracingStreams.clear();
   }
   if( aLevel == Debug::debug || aLevel == Debug::all )
   {
      theDebugStreams.clear();
   }
}

//{{{ Output formatting

void SetPrefix( const char * aPrefix )
{
   DebugStream << Prefix( debug ) << "Debug::SetPrefix(" << aPrefix << ")\n";

   theIndentPrefix = aPrefix;
}


void EnableLevelPrefix( bool aEnabled )
{
   DebugStream << Prefix( debug ) << "Debug::EnabletheLevelPrefix("
      << ( aEnabled ? TRUE_STRING : FALSE_STRING ) << ")\n";

   theLevelPrefix = aEnabled;
}


void EnableTimePrefix( bool aEnabled )
{
   DebugStream << Prefix( debug ) << "Debug::EnabletheTimePrefix("
      << ( aEnabled ? TRUE_STRING : FALSE_STRING ) << ")\n";

   theTimePrefix = aEnabled;
}

void EnableTimePrefixUS( bool aEnabled )
{
   DebugStream << Prefix( debug ) << "Debug::EnabletheTimePrefixUS("
      << ( aEnabled ? TRUE_STRING : FALSE_STRING ) << ")\n";

   theTimeUSPrefix = aEnabled;
}



std::ostream & operator<<( std::ostream & aStream, const Prefix & aPrefix )
{
   aStream << theIndentPrefix.c_str();
   DoPrefix( aPrefix.theLogLevel, aStream );
   return aStream;
}


std::ostream & operator<<( std::ostream & aStream, const Indent & aIndent )
{
   aStream << theIndentPrefix.c_str();
   DoPrefix( aIndent.theLogLevel, aStream );

   const size_t indentDepth = GetIndentDepth();
   for( unsigned int n = 0; n < indentDepth; n++ )
      aStream << INDENT;
   return aStream;
}


std::ostream & operator<<( std::ostream & aStream, const CSourcePos & aPos )
{
   PrintPos( aStream, aPos );
   return aStream;
}

// }}}

// {{{ Backtrace

Backtrace::Backtrace( int aStartSkipLevel /* = 0 */, int aEndSkipLevel /* = 2 */ ) : 
   theBacktraceSymbols( 0 ), theReallyLength( 0 ),
   theStartSkipLevel( aStartSkipLevel) , theEndSkipLevel( aEndSkipLevel )
{
   theReallyLength = ::backtrace( theBacktraceArray, sizeof(theBacktraceArray) );

   theBacktraceSymbols = ::backtrace_symbols( theBacktraceArray, theReallyLength );
};

Backtrace::~Backtrace()
{
   free( theBacktraceSymbols );
};

std::ostream &operator<<( std::ostream &aStream, const Backtrace & aBacktrace )
{
   char ** aSymbols = aBacktrace.Symbols();
   if( aSymbols )
   {
      for(  int aCounter = aBacktrace.GetStartSkipLevel(); 
            aCounter < aBacktrace.ReallyLength() - aBacktrace.GetEndSkipLevel(); 
            aCounter++ )
         aStream << aSymbols[aCounter] << "\n";
   }

   return aStream;
};
// }}}
//}}}

//{{{ Tracer
//bool DebugSourceEnabled( Debug::LogLevel lvl, Debug::TDebugSourceName aSource );
//void PrintPosShort( std::ostream & aOut, const Debug::CSourcePos & aWhere );
//extern unsigned int theIndentDepth;

//const char *TRACE_IN  = "-> Call function: ";
//const char *TRACE_OUT = "<- Return from:   ";
const char *TRACE_IN  = "-> Call:   ";
const char *TRACE_OUT = "<- Return: ";

static bool TraceAll     = false;
static bool UseProfiling = false;

void EnableAllTrace( bool aEnable )
{
   TraceAll = aEnable;
};

void EnableProfiling( bool aEnable )
{
   UseProfiling = aEnable;
};

Debug::CTracer::CTracer( TFunctionName aName )
   : theSourceName( 0 ), theName( aName ), theSourcePos( CURR_POS ), theTriggered( false )
{
	if( TraceAll || Debug::DebugSourceEnabled( tracing, theSourceName ) )
	{
		StartTrace();
	}
}


Debug::CTracer::CTracer( TDebugSourceName aSource, TFunctionName aName )
   : theSourceName( aSource ), theName( aName ), theSourcePos( CURR_POS ), theTriggered( false )
{
	if( TraceAll || DebugSourceEnabled( tracing, theSourceName ) )
	{
		StartTrace();
	}
}


Debug::CTracer::CTracer(const CSourcePos &aSourcePos)
   :theSourceName( 0 ), theName( 0 ), theSourcePos( aSourcePos ), theTriggered( false )
{
	if( TraceAll || DebugSourceEnabled( tracing, theSourceName ) )
	{
		StartTrace();
	}
}


Debug::CTracer::CTracer( TDebugSourceName aSource, const CSourcePos &aSourcePos )
   :theSourceName( aSource ), theName( 0 ), theSourcePos( aSourcePos ), theTriggered( false )
{
	if( TraceAll || DebugSourceEnabled( tracing, theSourceName ) )
	{
		StartTrace();
	}
}


Debug::CTracer::~CTracer()
{
	if( theTriggered )
	{
		StopTrace();
	}
}


void Debug::CTracer::StartTrace()
{
  DEBUG_LOCK;
   if( UseProfiling )
      gettimeofday( &theStartTime, 0 );
//      theStartTime = clock();

   CDbgOStream & aStreamRef = DBGOUT( tracing );

	aStreamRef << Indent( tracing );

  IncIndent();

	aStreamRef << TRACE_IN;
	if( theName )
	{
		aStreamRef << theName;
	}
	else
	{
		PrintPosShort( aStreamRef, theSourcePos );
	}
	if( theSourceName && !TraceAll )
	{
		aStreamRef << " (for \"" << theSourceName << "\")";
	}
	aStreamRef << std::endl;

	theTriggered = true;
}


void Debug::CTracer::StopTrace()
{
  DEBUG_LOCK;
//   clock_t aStopTime = 0;
  struct timeval    aStopTime;

  if( UseProfiling )
    gettimeofday( &aStopTime, 0 );
//      aStopTime = clock();

  DecIndent();

   CDbgOStream & aStreamRef = DBGOUT( tracing );

	aStreamRef << Indent( tracing );
	aStreamRef << TRACE_OUT;
	if( theName )
	{
		aStreamRef << theName;
	}
	else
	{
      if( theSourcePos.theFuncName )
         aStreamRef << theSourcePos.theFuncName;
      else
         PrintPosShort( aStreamRef, theSourcePos );
	}
	if( theSourceName && !TraceAll )
	{
		aStreamRef << " (for \"" << theSourceName << "\")";
	}
	if( !DebugSourceEnabled( tracing, theSourceName ) && !TraceAll )
	{
		aStreamRef << " (note: this tracing level is disabled)";
	}
   if( UseProfiling )
   {
      long sec  = aStopTime.tv_sec  - theStartTime.tv_sec; 
      long usec = aStopTime.tv_usec - theStartTime.tv_usec;
      if( usec < 0 )
      {
         usec += 1000000;
         --sec;
      };
		aStreamRef << " time = " << sec << "s, " << usec << "us";
   };

	aStreamRef << std::endl;
}
//}}}

//{{{ Assert
//const char * GetLevelName( Debug::LogLevel aLevel );
//bool DebugSourceEnabled( Debug::LogLevel aLevel, Debug::TDebugSourceName aSource );
//void PrintPos( std::ostream & aOut, const Debug::CSourcePos & aWhere );

//{{{ Assert constans
Debug::AssertMode AssertModes[Debug::all + 1] =
{
   ASSERT_DIE,
   ASSERT_DIE,
   ASSERT_DIE,
   ASSERT_DIE,
   ASSERT_DIE,
   ASSERT_DIE,
   ASSERT_DIE, 
   ASSERT_DIE 
};

const char *BEHAVIOUR_NAMES[] = 
{
   "ASSERT_DIE",
   "ASSERT_THROW",
   "ASSERT_CONTINUE",
   "ASSERT_CONTINUE_CORE"
};
//}}}

// {{{ SetAssertionMode
void SetAssertionMode( LogLevel aLogLevel, AssertMode aMode )
{
   DBGOUT_DEBUG(  Prefix(debug) << "Debug::SetAssertionMode("
         << GetLevelName( aLogLevel ) << "," << BEHAVIOUR_NAMES[aMode] << ")\n" );

   if( aLogLevel < Debug::all )
   {
      AssertModes[aLogLevel] = aMode;
   }
   else
   {
      for( int n = 0; n < Debug::all; n++ )
      {
         AssertModes[n] = aMode;
      }
   }
};//}}}

//{{{ ostream operator
std::ostream &operator<<( std::ostream &aStream, const AssertException & aAssertExp )
{
   aStream << "Assertion failure: \"" << aAssertExp.theText << "\" ";
   aStream << "at ";
   PrintPos( aStream, aAssertExp.thePosition );

   return aStream;
};//}}}

//{{{ Assert realisation
void __TextAssertion( const char * aPrefixText, const char * aText, LogLevel aLevel, 
      TDebugSourceName aSourceName, TLineNumber aLine, 
      TFunctionName aFuncName, TFileName aFileName )
{
   if( aSourceName == 0 || DebugSourceEnabled( aLevel, aSourceName ) ) //&& !info.asserted && period_allows(info) )
   {
      DBGOUT( aLevel ) << Debug::Indent( aLevel ) << aPrefixText;
      if( aText )
         DBGOUT( aLevel ) << " \"" << aText << "\" ";
      if( aSourceName )
      {
         DBGOUT( aLevel ) << "for \"" << aSourceName << "\" ";
      }
      DBGOUT( aLevel ) << "at ";
      CSourcePos aSourcePos( aLine, aFuncName, aFileName );
      PrintPos( DBGOUT( aLevel ), aSourcePos );
      DBGOUT( aLevel ) << "\n";
      DBGOUT( aLevel ) << "Backtrace:\n";
      DBGOUT( aLevel ) << Backtrace( 2 );

      switch( aLevel != Debug::fatal ? AssertModes[aLevel] : Debug::ASSERT_DIE )
      {
         case Debug::ASSERT_DIE:
            {
               DBGOUT( aLevel ) << "exiting\n";
               abort();
               break;
            }
         case Debug::ASSERT_THROW:
            {
               throw  Debug::AssertException( aSourcePos, aText );
               break;
            }
         case Debug::ASSERT_CONTINUE:
            break;
         case Debug::ASSERT_CONTINUE_CORE:
            DBGOUT( aLevel ) << "make core\n";
            if( fork() == 0 )
               abort();
            break;
         default:
            break;
      }
   }
}; //  END: void Debug::TextAssertion

void __Assertion(  const char * aText, 
      LogLevel aLevel, TDebugSourceName aSourceName, 
      TLineNumber aLine , TFunctionName aFuncName, 
      TFileName aFileName )
{
   __TextAssertion( "Assertion failure: ", aText, aLevel, aSourceName, aLine, aFuncName, aFileName );
};
//}}}
//}}}

//{{{ Syslog
CSyslog::CSyslog(void): CLogStreamBuf(),
         CDbgOStream( static_cast<CLogStreamBuf *>( this ) ),
   theOptions( 0 ), theFacility( SYSLOG_USER ), theLevel( SYSLOG_DEBUG ),
   ptr( buf ), len( 0 )
{

};

CSyslog::~CSyslog()
{
   Close();
};

// {{{ void CSyslog::Open( const char * aIdent, int aOptions, SyslogFacility aFacility );
void CSyslog::Open( const char * aIdent, int aOptions, SyslogFacility aFacility )
{
   theOptions  = aOptions;
   theFacility = aFacility;
	::openlog( aIdent, theOptions, theFacility );
}; // }}} END: void CSyslog::Open( const char * aIdent, int aOptions, SyslogFacility aFacility = SYSLOG_USER )

// {{{ void CSyslog::Close(void)
void CSyslog::Close(void)
{
   flush();
	::closelog();
}; // }}} END: void CSyslog::Close(void)

int CSyslog::overflow(int c) 
{
   if( c == '\n' || c == EOF ) 
   {
      *ptr++ = '\n';
      *ptr = '\0';
      syslog( theLevel, buf );
      ptr = buf;
      len = 0;
      return 0;
   };
   if( ( len + 2 ) >= sizeof(buf) )
      return EOF;
   *ptr++ = c;
   len++;
   return 0;
};

CSyslog &CSyslog::operator()(const char * aIdent, int aOptions, SyslogFacility aFaciliry )
{
	Open( aIdent, aOptions, aFaciliry );
	return *this;
}


// {{{ Access functions
#if defined(__GNUC__) && ( __GNUC__< 3 )                   
static CSyslog staticSyslog;
CSyslog & Syslog( Debug::LogLevel aLevel )
{
#else
CSyslog & Syslog( Debug::LogLevel aLevel )
{
   static CSyslog staticSyslog;
#endif

   staticSyslog.SetLogLevel( aLevel );
   return staticSyslog;
}

CSyslog & Syslog(const char * aIdent, int aOptions, SyslogFacility aFaciliry )
{
   return Syslog()( aIdent, aOptions, aFaciliry );
};

void CSyslog::SetBufLogLevel( LogLevel aLevel )
{ 
   switch( aLevel ) 
   {
      case all:
         theLevel = SYSLOG_EMERG;
         break;
      case none:
         theLevel = SYSLOG_NONE;
         break;
      case tracing:
         theLevel = SYSLOG_DEBUG;
         break;
      case fatal:
         theLevel = SYSLOG_EMERG;
         break;
      case error:
         theLevel = SYSLOG_ERR;
         break;
      case warning:
         theLevel = SYSLOG_WARNING;
         break;
      case info:
         theLevel = SYSLOG_INFO;
         break;
      case debug:
         theLevel = SYSLOG_DEBUG;
         break;
      default :
         theLevel = SYSLOG_EMERG;
         break;
   };
};
// }}}

//}}}

}; // END: namespace Debug

/* {{{ Modeline for ViM
 * vim600:fdm=marker fdl=0 fdc=3:
 * }}} */
