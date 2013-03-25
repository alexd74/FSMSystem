// {{{ Library documentation
//  \file Debug.h

//  \brief Main header for Debug library.
#ifndef RUS_DOC

/*! \mainpage

  \section intro Introduction

   The Debug library is a collection of C++ classes, functions and macros
   that provides support to easy develop and debug the C++ programs.

   The library provides the following possibilities for debugging.
   - Logging support.
      -# Logging support with some predefined levels of importance.
      -# Using of several output streams for each debug levels.
      -# Connection and disconnection of output streams at runtime.
      -# Ostream and printf syntax.
      -# A class for function call tracing and profiling.
   - Assertion macros with extended capabilities.
      -# Information about assert reason, function name, file name 
         and line number is printed.
      -# Customization of program behaviour at assert function call for 
         each debug level.
         - Print information and die with core file creation.
         - Print information and throw an exception.
         - Print information and continue execution.
         - Print information, core file creation and continue execution.
      -# Eiffel-style check -- REQUIRE, ENSURE and CHECK_INVARIANT macros
         for class invariant check.
   - Restricted types
      -# Unchanged object.
      -# Nonnull pointer.
   - Logging and assertion behavior control from command-line parameters.


   Based on dbg library (c) Pete Goodliffe 2001 (pete.goodliffe@pace.co.uk)
   and invariant library (с) Alex Solomonoff ( asolomon@bbn.com )

   \author Alexey Dyachenko <dyachenko@fromru.com>
   \date   2002-07-17 15:31

   \section libuse Library use

   To use the library with your program you must include <Debug.h> and
   compile with defined DEBUG_ENABLED macros ( gcc parameter -DDEBUG_ENABLED ).

   To use Eiffel-style check you must define EIFFEL_CHECK macros 
   ( gcc parameter -DEIFFEL_CHECK ).

   If you build without DEBUG_ENABLED or with defined NDEBUG macros you will have no 
   debugging support. 

   \subsection libuselogging Logging

   The logging mechanisms works with several output streams for each predefined debug levels.
   Before sending debug output to some stream, you must connect this stream to logging
   mechanism with LogEnable or LogEnableAll functions.

   For logging with stream syntax you can use DBGOUT* macros,
   for logging with printf syntax you can use DBGPRINT* macros.

   An example of logging functions:
   \code
   // messages with debug priority go to std::cout 
   Debug::AttachStream( Debug::debug, std::cout );

   Debug::LogEnable( Debug::debug, true );

   DBGOUT_DEBUG( "Debug information\n" );

   DBGOUT_FATAL( Debug::Prefix() << "Fatal error information " << std::endl );

   DBGPRINT_DEBUG( "Debug info with %s syntax. X = %d \n", "printf", 1 );

   DBGOUT_TRACING( Debug::Indent() << "This is output at 'tracing' level, indented "
                                     << "to the same level as the current tracing "
                                     << "indent.\n" );

   \endcode

   Produce this output:
   <pre>
   Debug information
   * * * Fatal error information 
   Debug info with printf syntax. X = 1 
   * * * This is output at 'tracing' level, indented to the same level as the current tracing indent.
   </pre>


   The logging mechanisms provide you with the ability to prepend to all
   diagnostic output a standard prefix (see @ref Debug::SetPrefix), and
   also to add the diagnostic level and current time to the prefix (see
   @ref Debug::EnableLevelPrefix and @ref Debug::EnableTimePrefix).

   \subsection libuseatrace Function tracing

   The DEBUG_TRACER macros is used for functions call tracing.
   This macro creates temporary object of type Debug::CTracer.
   For usage of profiling you must call EnableProfiling function
   or use \b --enable-profiling command-line parameter.

   \code
   void test_func2()
   {
      DEBUG_TRACER;
      DBGOUT_INFO( Debug::Prefix() << "we are in test_func2\n" );
   };

   void test_func( int param = 0 )
   {
      DEBUG_TRACER;
      DBGOUT_INFO( Debug::Prefix() << "Param = " << param << std::endl );
      DBGOUT_INFO( Debug::Prefix() << "we are in test_func\n" );
   
      usleep( 1000000 );
      test_func2();
   };

   test_func( 4 );
   \endcode

   Result:
   <pre>
   * * *   -> Call:   void test_func(int) (66 in main.cpp)
   * * * Param = 4
   * * * we are in test_func
   * * *     -> Call:   void test_func2() (59 in main.cpp)
   * * * we are in test_func2
   * * *     <- Return: void test_func2() time = 0s, 511us
   * * *   <- Return: void test_func(int) time = 1s, 8202us
   </pre>



   \subsection libuseassert Assert macro 
 
   The Assert macro  is used to test conditions in the program.
   If result of condition is false -- the program prints information about
   condition, function name, file name and line number.
   After that, depening on assert level, the program can die with core file
   creation, throw an exception, continue execution or continue execution
   with core file creation. This reaction is controlled
   by @ref Debug::SetAssertionMode function.

   An example of using @ref Assert:
   \code
   void AssertTest()
   {
      int num = 0;
      Debug::SetAssertionMode( Debug::all, Debug::ASSERT_CONTINUE );

      Assert( num > 0 );

      try
      {
         Debug::SetAssertionMode( Debug::all, Debug::ASSERT_THROW );
         LevelAssert( num > 0, Debug::warning );
      }
      catch( Debug::AssertException & exp )
      {
         DBGOUT_WARNING( "got exception " << exp << std::endl );
      };
      Debug::SetAssertionMode( Debug::all, Debug::ASSERT_DIE );

      Assert( num > 0 );
   };
   \endcode

   The result:
   <pre>

   * * * Debug::SetAssertionMode(all,ASSERT_CONTINUE)
   * * * Assertion failure:  "num > 0" at function: void AssertTest(), line: 64, file: main.cpp
   * * * Debug::SetAssertionMode(all,ASSERT_THROW)
   * * * Assertion failure:  "num > 0" at function: void AssertTest(), line: 70, file: main.cpp
   got exception Assertion failure: "num > 0" at function: void AssertTest(), line: 70, file: main.cpp
   * * * Debug::SetAssertionMode(all,ASSERT_DIE)
   * * * Assertion failure:  "num > 0" at function: void AssertTest(), line: 97, file: main.cpp
   exiting
   Aborted (core dumped)
   </pre>

   If you wish to use Assert under the name  assert(), 
   you must define the preprocessor 
   variable USE_DEBUG_ASSERT. 

   \subsection libuseeiffel Eiffel-style check

   The Debug library supports Eiffel-style class invariant tests ( www.eiffel.com ).

   For some class you can create special
   \code
   bool invariant()
   \endcode
   function, that returns class invariant state ( true == no error ).

   At the beginning of each public function of this class you can
   write special CHECK_INVARIANT macro. This macro creates temporary object,
   which calls \b invariant() function at it's creation and destruction and
   calls Assert, if necessary.

   The library provides ENSURE, REQUIRE and INVARIANT macros.

   Macro: REQUIRE( exprn )
   
   Called at the beginning of each method to check its precondition (requirements). 
   
   For example:                                                                                 

   \code
   void Stack::push(int n) 
   {                                                                          
      REQUIRE( !full() ); // stack has space for push                                                    
      ...                                                                                              
   }                                                                                                  
   \endcode
   
   Macro: ENSURE( exprn )
   
   Called at the end of each method. It checks the postcondition for a method.

   \code
   void Stack::push(int n) 
   {                                                                          
      ...                                                                                              
         ENSURE( !empty() ); // it can't be empty after a push!                                             
   }
   \endcode

   Macro: INVARIANT( exprn )
   
   Immediately checks class invariant without waiting of current function return.
   
   Example of EIFFEL-style checking:
   \code

   class post_test
   {
   public:
      post_test() : a(10) {};

      void do_test()
      {
         DEBUG_TRACER;
         CHECK_INVARIANT;
         REQUIRE( a == 10 );
         DBGOUT_INFO( Debug::Prefix() << "hello\n" );
         a = 9;

         ENSURE( a == 9 );
      };

   #ifdef DEBUG_ENABLED
      bool invariant()
      {
         return a == 10;
      };
   #endif
      private:
         int a;
   };

   std::cout << "Testing Debug::pre_post\n";
   {

      post_test test_obj;
      test_obj.do_test();
   }
   \endcode

   Results:
   \code
   Testing Debug::pre_post
   * * *   -> Call:   void post_test::do_test() (109 in main.cpp)
   * * * hello
   * * *     Invariant check failed at function return: at function: 
   void post_test::do_test(), line: 0, file: main.cpp
   exiting
   Aborted (core dumped)
   \endcode

   \subsection libuseunchanged Unchanged object.
   \b unchanged<Type>  objects enforce constancy of another object. They are
   used as follows:
   \code
       Type t;                // object that is not supposed to change.
       {
           ...
           unchanged<Type> ut(t); // object that will enforce this constancy.
           ...
           ut.check();            // checking for any change since ut was created
           ...
       }                          // final check when ut goes out of scope.
   \endcode
   If the object is found to have changed, an assertion fires.
   The unchangedness is checked using Type::operator==, so that member function
   has to exist for this to work.

   Example:
   \code
    std::cout << "Testing Debug::unchangeg\n";
    {

       int val = 5;
       UNCHANGED( val );

       val = 6;

       CHECK_UNCHANGED( val );

    }
   \endcode
   Result:
   \code
   Testing Debug::unchanged
   * * *   Assertion failure:  "curr == orig" at function: void unchanged<type>::check() 
   const [with type = int], line: 24, file: ../restricted_types.h
   make core
   \endcode

   \subsection libusenonnulptr Nonnull pointer

   In some situations, pointers are allowed to be NULL. Their NULLness acts as a flag
   that some action is not supposed to happen or that something doesn't exist.
   In other situations, a NULL pointer means a bug. But checking pointers for NULLness
   all over the code is tedious and few programmers do it consistently. @ref Nonnull_arr_ptr
   and @ref nonnull_ptr are two smart pointer classes that mimic normal pointers fairly 
   closely except that they check for NULLness every time they are assigned. 

   An ordinary pointer will seg fault if it is NULL and you dereference it
   (on most modern operating systems), but
   this could happen far downstream of the original bug, which happened at or before
   the time that the pointer was last assigned. A nonnull_ptr will fire an assertion
   when it is assigned to NULL, which is probably closer to the location of the bug.

   Example code:
   \code
   int f = 0;
   nonnull_ptr<int> a = &f;
   DBGOUT_DEBUG( "before null\n" );

   a = 0;

   DBGOUT_DEBUG( "after null\n" );
   \endcode

   Result:
   <pre>
   before null
   * * *   Assertion failure:  "ptr != ((T*)0)" at function: void nonnull_ptr<T>::check() 
      const [with T = int], line: 71, file: ../nonnull_ptr.h
   make core
   after null
   </pre>


   \subsection libusedefaultstate Default state
 
   When your program first starts up,
   the error and fatal levels are 
   switched on and the std::cerr stream is connected to these levels.
   All other debug levels are switched off.
   You can enable these levels with functions LogEnable, LogEnableAll and
   connect some output streams with AttachStream function.

   \subsection libusecommandline Command-line parameters

   You can control the behaviour of debug library with command-line parameters.
   For using this possibility you must call Debug::ParseDebugOptions function
   from function \b main.

   An example:
   \code
   #ifdef DEBUG_ENABLED
   using namespace Debug;
   #endif

   int main( int argc, char ** argv )
   {
   
   #ifdef DEBUG_ENABLED
      ParseDebugOptions( argc, argv );
   #endif

      DEBUG_TRACER;

      return 0;
   };
   \endcode


   In the current library version the following command-line parameters are 
   implemented
      - --enable-fatal[=all,name]
      - --disable-fatal[=all,name]
      - --enable-error[=all,name]
      - --disable-error[=all,name]
      - --enable-warning[=all,name]
      - --disable-warning[=all,name]
      - --enable-info[=all,name]
      - --disable-info[=all,name]
      - --enable-debug[=all,name]
      - --disable-debug[=all,name]
      - --enable-tracing[=all,name]
      - --disable-tracing[=all,name]
      - --enable-all-debug[=all,name]
      - --disable-all-debug[=all,name]
      - --enable-cerr[=all,name]
      - --disable-cerr[=all,name]
      - --enable-level-prefix
      - --disable-level-prefix
      - --enable-time-prefix
      - --disable-time-prefix
      - --enable-profiling
      - --disable-profiling
      - --enable-cout[=all,name]
      - --disable-cout[=all,name]
  */
#else
/*! \mainpage

  \section intro Введение

   Библиотека Debug -- это коллекция взаимосвязанных С++ классов, функций и
   макросов для облегчения разработки и отладки С++ программ.

   Библиотека предоставляет следующие возможности для отладки.
   - Поддержка отладочной печати.
      -# Отладочная печать с поддержкой нескольких уровней важности.
      -# Возможность подключения нескольких потоков вывода для каждого из уровней отладочной печати.
      -# Возможность динамически (in runtime) включать и выключать потоки и уровни отладочной печать.
      -# Использование синтаксиса ostream и printf.
      -# Класс для отслеживания вызовов функций ( function call tracing ) и измерения времени
         работы функции.
   - Assertion macros с расширенными возможностями.
      -# Печатается информация о причине assert, имя функции, имя файла и номер строки.
      -# Настраиваемое поведение программы при вызове функции assert для каждого уровня отладки
         - напечатать информацию и закончить программу с созданием файла core
         - напечатать информацию и бросить исключение
         - напечатать информацию и продолжить работу
         - напечатать информацию и продолжить работу с созданием файла core
      -# Проверка в стиле Eiffel -- макросы REQUIRE, ENSURE и CHECK_INVARIANT для 
         проверки инварианта класса.
   - Защищенные типы
      -# Неизменяемый объект
      -# Ненулевой указатель
   - Возможность управлять уровнем отладочной печати и поведением функции assert из командной строки.



   Библиотека основана на 
   dbg library (c) Pete Goodliffe 2001 (pete.goodliffe@pace.co.uk)
   и invariant library (с) Alex Solomonoff ( asolomon@bbn.com ).


   \author Alexey Dyachenko <dyachenko@fromru.com>
   \date   2002-07-17 15:31

   \section libuse Использование библиотеки

   Для использования библиотеки надо включить заголовочный файл <Debug.h> и
   производить компиляцию с определенным макросом DEBUG_ENABLED ( параметр
   gcc -DDEBUG_ENABLED )

   Для включения проверки в стиле Eiffel необходимо дополнительно определить
   макрос EIFFEL_CHECK ( gcc -DEIFFEL_CHECK )

   Если программа комилируется без определения этих макросов или при определенном макросе
   NDEBUG то поддержка отладки не включается.

   \subsection libuselogging Отладочная печать

   Механизм отладочной печати использует для каждого уровня печати свой поток вывода.
   Чтобы происходил вывод в поток его необходимо сначала подключить к системе логгинга
   с помощью функии LogEnable или LogEnableAll;

   Для отладочной печати с помощью синтаксиса stream используются макросы DBGOUT*,
   для использования функции printf используются макросы DBGPRINT*.

   Пример использования отладочной печати.
   \code
   // messages with debug priority go to std::cout 
   Debug::AttachStream( Debug::debug, std::cout );

   Debug::LogEnable( Debug::debug, true );

   DBGOUT_DEBUG( "Debug information\n" );

   DBGOUT_FATAL( Debug::Prefix() << "Fatal error information " << std::endl );

   DBGPRINT_DEBUG( "Debug info with %s syntax. X = %d \n", "printf", 1 );

   DBGOUT_TRACING( Debug::Indent() << "This is output at 'tracing' level, indented "
                                     << "to the same level as the current tracing "
                                     << "indent.\n" );

   \endcode
   Напечатает следующий текст
   <pre>
   Debug information
   * * * Fatal error information 
   Debug info with printf syntax. X = 1 
   * * * This is output at 'tracing' level, indented to the same level as the current tracing indent.
   </pre>

   Механизм отладочной печати предоставляет возможность предварять каждый отладочный
   вывод стандартным префиксом ( смотри @ref Debug::SetPrefix ) и печатать
   название уровня диагностики и текущее время ( смотри 
   @ref Debug::EnableLevelPrefix и @ref Debug::EnableTimePrefix ).

   \subsection libuseatrace Отслеживание вызовов функций (tracing)

   Для ослеживания вызовов функций используется макрос DEBUG_TRACER,
   который создает временный объект типа Debug::CTracer.
   Чтобы включить измерение времени работы функции, необходимо
   вызвать функцию EnableProfiling или задать в командной строке 
   параметр \b --enable-profiling.

   \code
   void test_func2()
   {
      DEBUG_TRACER;
      DBGOUT_INFO( Debug::Prefix() << "we are in test_func2\n" );
   };

   void test_func( int param = 0 )
   {
      DEBUG_TRACER;
      DBGOUT_INFO( Debug::Prefix() << "Param = " << param << std::endl );
      DBGOUT_INFO( Debug::Prefix() << "we are in test_func\n" );
   
      usleep( 1000000 );
      test_func2();
   };

   test_func( 4 );
   \endcode

   Результат работы:
   <pre>
   * * *   -> Call:   void test_func(int) (66 in main.cpp)
   * * * Param = 4
   * * * we are in test_func
   * * *     -> Call:   void test_func2() (59 in main.cpp)
   * * * we are in test_func2
   * * *     <- Return: void test_func2() time = 0s, 511us
   * * *   <- Return: void test_func(int) time = 1s, 8202us
   </pre>



   \subsection libuseassert Макрос assert
 
   Макрос Assert служит для проверки правильности условий при работе программы.
   Если условие не выполняется -- программа печатает информацию об этом, имя
   функции, имя файла и номер строки, в которой производилось проверка условия.
   После этого в зависимости от уровня assert программа может завершить свою работу
   с созданием файла core, бросить исключение, просто продолжить работу и 
   продолжить работу с созданием файла core. Реакция на невыполненное условие
   настраивается с помощью функции @ref Debug::SetAssertionMode.

   Пример использования @ref Assert:
   \code
   void AssertTest()
   {
      int num = 0;
      Debug::SetAssertionMode( Debug::all, Debug::ASSERT_CONTINUE );

      Assert( num > 0 );

      try
      {
         Debug::SetAssertionMode( Debug::all, Debug::ASSERT_THROW );
         LevelAssert( num > 0, Debug::warning );
      }
      catch( Debug::AssertException & exp )
      {
         DBGOUT_WARNING( "got exception " << exp << std::endl );
      };
      Debug::SetAssertionMode( Debug::all, Debug::ASSERT_DIE );

      Assert( num > 0 );
   };
   \endcode

   Результат работы:
   <pre>

   * * * Debug::SetAssertionMode(all,ASSERT_CONTINUE)
   * * * Assertion failure:  "num > 0" at function: void AssertTest(), line: 64, file: main.cpp
   * * * Debug::SetAssertionMode(all,ASSERT_THROW)
   * * * Assertion failure:  "num > 0" at function: void AssertTest(), line: 70, file: main.cpp
   got exception Assertion failure: "num > 0" at function: void AssertTest(), line: 70, file: main.cpp
   * * * Debug::SetAssertionMode(all,ASSERT_DIE)
   * * * Assertion failure:  "num > 0" at function: void AssertTest(), line: 97, file: main.cpp
   exiting
   Aborted (core dumped)
   </pre>

   При компиляции с определенным макросом USE_DEBUG_ASSERT Assert используется
   вместо стандартной функции assert.

   \subsection libuseeiffel Проверки в стиле Eiffel

   Библиотека Debug поддерживает проверки в стиле Eiffel ( www.eiffel.com ).
   Для класса можно задать public функцию bool invariant(), в которая для этого класса
   вычисляет значение его инварианта. Функция возвращает true при нормальном состоянии
   объекта и false при нарушении инварианта.
   В начале каждой из public функций этого класса пишется макрос CHECK_INVARIANT,
   который создает временный объект, вызывающий функцию проверки инварианта при
   его создании и удалении и при необходимости вызывает Assert.

   Библиотека предоставляет также макросы ENSURE, REQUIRE и INVARIANT.

   Макрос REQUIRE вызывается в начале функции для проверке предусловий
   ( требований ) функции.

   \code
   void Stack::push(int n) 
   {                                                                          
      REQUIRE( !full() ); // stack has space for push                                                    
      ...                                                                                              
   }                                                                                                  
   \endcode
   
   Макрос ENSURE вызывается в конце функции для проверки постусловий правильности
   выполнения функции.

   \code
   void Stack::push(int n) 
   {                                                                          
      ...                                                                                              
         ENSURE( !empty() ); // it can't be empty after a push!                                             
   }
   \endcode

   Макрос INVARIANT производит немедленную проверку инварианта класса, без
   ожидания выхода из текущей функции.

   Пример использования:
   \code
   class post_test
   {
   public:
      post_test() : a(10) {};

      void do_test()
      {
         DEBUG_TRACER;
         CHECK_INVARIANT;
         REQUIRE( a == 10 );
         DBGOUT_INFO( Debug::Prefix() << "hello\n" );
         a = 9;

         ENSURE( a == 9 );
      };

   #ifdef DEBUG_ENABLED
      bool invariant()
      {
         return a == 10;
      };
   #endif
      private:
         int a;
   };

   std::cout << "Testing Debug::pre_post\n";
   {

      post_test test_obj;
      test_obj.do_test();
   }
   \endcode

   Результат работы:
   \code
   Testing Debug::pre_post
   * * *   -> Call:   void post_test::do_test() (109 in main.cpp)
   * * * hello
   * * *     Invariant check failed at function return: at function: 
   void post_test::do_test(), line: 0, file: main.cpp
   exiting
   Aborted (core dumped)
   \endcode

   \subsection libuseunchanged Неизменяемый объект
   \b unchanged<Type>  objects enforce constancy of another object. They are
   used as follows:
   \code
       Type t;                // object that is not supposed to change.
       {
           ...
           unchanged<Type> ut(t); // object that will enforce this constancy.
           ...
           ut.check();            // checking for any change since ut was created
           ...
       }                          // final check when ut goes out of scope.
   \endcode
   If the object is found to have changed, an assertion fires.
   The unchangedness is checked using Type::operator==, so that member function
   has to exist for this to work.

   Example:
   \code
    std::cout << "Testing Debug::unchangeg\n";
    {

       int val = 5;
       UNCHANGED( val );

       val = 6;

       CHECK_UNCHANGED( val );

    }
   \endcode
   Result:
   \code
   Testing Debug::unchanged
   * * *   Assertion failure:  "curr == orig" at function: void unchanged<type>::check() 
   const [with type = int], line: 24, file: ../restricted_types.h
   make core
   \endcode

   \subsection libusenonnulptr Nonnull pointer

   In some situations, pointers are allowed to be NULL. Their NULLness acts as a flag
   that some action is not supposed to happen or that something doesn't exist.
   In other situations, a NULL pointer means a bug. But checking pointers for NULLness
   all over the code is tedious and few programmers do it consistently. @ref Nonnull_arr_ptr
   and @ref nonnull_ptr are two smart pointer classes that mimic normal pointers fairly 
   closely except that they check for NULLness every time they are assigned. 

   An ordinary pointer will seg fault if it is NULL and you dereference it
   (on most modern operating systems), but
   this could happen far downstream of the original bug, which happened at or before
   the time that the pointer was last assigned. A nonnull_ptr will fire an assertion
   when it is assigned to NULL, which is probably closer to the location of the bug.

   Example code:
   \code
   int f = 0;
   nonnull_ptr<int> a = &f;
   DBGOUT_DEBUG( "before null\n" );

   a = 0;

   DBGOUT_DEBUG( "after null\n" );
   \endcode

   Result:
   <pre>
   before null
   * * *   Assertion failure:  "ptr != ((T*)0)" at function: void nonnull_ptr<T>::check() 
      const [with T = int], line: 71, file: ../nonnull_ptr.h
   make core
   after null
   </pre>



 
   \subsection libusedefaultstate Состояние по умолчанию
 
   При старте программы уровни error и fatal включены и к ним подключен вывод
   в поток cerr. Все остальные уровни печати выключены.
   Вы можете включить их с помощью функций LogEnable, LogEnableAll и подключить
   поток вывода функцией AttachStream.

   \subsection libusecommandline Параметры командной строки

   При сборке программы с библиотекой Debug возможно управление поведением
   библиотеки с помощью параметров командной строки.
   Для этого в функцию main необходимо добавить вызов функции 
   Debug::ParseDebugOptions.

   Пример вызова:
   \code
   #ifdef DEBUG_ENABLED
   using namespace Debug;
   #endif

   int main( int argc, char ** argv )
   {
   
   #ifdef DEBUG_ENABLED
      ParseDebugOptions( argc, argv );
   #endif

      DEBUG_TRACER;

      return 0;
   };
   \endcode


   Параметры командой строки, работающие в текущей версии библиотеки:
      - --enable-fatal[=all,name]
      - --disable-fatal[=all,name]
      - --enable-error[=all,name]
      - --disable-error[=all,name]
      - --enable-warning[=all,name]
      - --disable-warning[=all,name]
      - --enable-info[=all,name]
      - --disable-info[=all,name]
      - --enable-debug[=all,name]
      - --disable-debug[=all,name]
      - --enable-tracing[=all,name]
      - --disable-tracing[=all,name]
      - --enable-all-debug[=all,name]
      - --disable-all-debug[=all,name]
      - --enable-cerr[=all,name]
      - --disable-cerr[=all,name]
      - --enable-level-prefix
      - --disable-level-prefix
      - --enable-time-prefix
      - --disable-time-prefix
      - --enable-profiling
      - --disable-profiling
      - --enable-cout[=all,name]
      - --disable-cout[=all,name]

  */
#endif

  // La ast modified: 2004-04-20 14:55

  // }}}

#ifndef DEBUG_H
#  define DEBUG_H

//{{{ Includes
#include <time.h>
#include <sys/time.h>
#include <stdio.h>

#include <iostream>
#include <exception>

#include <ctime>
#include <syslog.h>

#include <boost/thread/recursive_mutex.hpp>

//}}}

namespace Debug
{
#define NULL_EXPR ((void)0)

//{{{ Check defines
#undef DEBUG_WORK

#ifndef NDEBUG
#ifdef DEBUG_ENABLED
#define DEBUG_WORK
#endif
#endif
//}}}

// macros from ScopeGuard by Andrei Alexandrescu
#define CONCATENATE_DIRECT(s1, s2) s1##s2
#define CONCATENATE(s1, s2) CONCATENATE_DIRECT(s1, s2)
#define ANONYMOUS_VARIABLE(str) CONCATENATE(str, __LINE__)

//{{{ Extra functions
void EnableAllTrace( bool aEnable );

void EnableProfiling( bool aEnable );
//}}}

//{{{ Log

    /**
     * Predefined debugging levels.
     */
enum LogLevel
{
   fatal   = LOG_EMERG,   //!< system is unusable
   error   = LOG_ERR,     //!< error conditions
   warning = LOG_WARNING, //!< warning conditions
   info    = LOG_INFO,    //!< informational messages
   debug   = LOG_DEBUG,   //!< debug-level messages
   tracing,               //!< programm tracing messages
   none,
   all                    //!< enable or disable all levels
};

//{{{ Source code position
    /**
     * typedef for a string that describes the source of a diagnostic. If you
     * are working on a large project with many small code modules you may
     * only want to enable debugging from particular modules. This
     * typedef facilitiates this.
     *
     * Depending on the desired granularity of your Debug sources you will use
     * different naming conventions. For example, your aSource might
     * be filenames, that way you can switch off all debugging output from
     * a particular file quite easily. It might be device driver names,
     * component names, library names, or even function names. It's up to you.
     *
     * @see Debug::LogEnable( LogLevel aLevel , TDebugSourceName aSource, bool aEnabled )
     * @see Debug::LogEnableAll
     */
typedef const char * TDebugSourceName;

    /**
     * Typedef used in the @ref CSourcePos data structure.
     *
     * Describes a line number in a source file.
     *
     * @see Debug::CSourcePos
     */
typedef const unsigned int TLineNumber;

    /**
     * Typedef used in the @ref CSourcePos data structure.
     *
     * Describes a function name in a source file. (Can be zero to
     * indicate the function name cannot be assertained on this compiler).
     *
     * @see Debug::CSourcePos
     */
typedef const char * TFunctionName;

    /**
     * Typedef used in the @ref CSourcePos data structure.
     *
     * Describes a filename.
     *
     * @see Debug::CSourcePos
     */
typedef const char * TFileName;

    /**
     * Data structure describing a position in the source file. That is,
     *   @li The line number
     *   @li The function name (if the compiler supports this)
     *   @li The filename
     *
     * To create a CSourcePos for the current position, you can use
     * the CURR_POS convenience macro.
     *
     * There is an empty constructor that allows you to create a CSourcePos
     * that represents 'no position specified'.
     *
     */
struct CSourcePos
{
   TLineNumber    theLine;
   TFunctionName  theFuncName;
   TFileName      theFileName;

      /**
       * Creates a CSourcePos struct. Use the CURR_POS macro to
       * call this constructor conveniently.
       */
   CSourcePos( TLineNumber aLine, TFunctionName aFuncName, TFileName aFileName )
      : theLine( aLine ), theFuncName( aFuncName ), theFileName( aFileName )
      {};

      /**
       * A 'null' CSourcePos for 'no position specified'
       */
   CSourcePos()
      : theLine( 0 ), theFuncName( 0 ), theFileName( 0 )
      {};
};

    /**
     * This is called when you send a @ref CSourcePos to a diagnostic output.
     * You can use this to easily check the flow of execcution in your
     * program.
     *
     * For example,
     * \code
     *     DBGOUT_TRACING( CURR_POS << std::endl );
     * \endcode
     *
     * Take care that you only send CURR_POS to diagnostic outputs (obtained
     * with @ref Debug::DBGOUT). In non debug builds, CURR_POS is a "no-op" and
     * so no useful output will be produced.
     *
     * @internal
     * @see Debug::indent
     */
std::ostream &operator<<( std::ostream &aStream, const CSourcePos &aSourcePos );
//}}}

#ifdef DEBUG_ENABLED

    /*
     * DBG_FUNCTION is defined to be a macro that expands to the name of
     * the current function, or zero if the compiler is unable to supply that
     * information. It's sad that this wasn't included in the C++ standard
     * from the very beginning.
     */
#if defined(__GNUC__)
//    #define DBG_FUNCTION __FUNCTION__
#define DBG_FUNCTION __PRETTY_FUNCTION__
#else
#define DBG_FUNCTION 0
#endif

#define CURR_POS \
(::Debug::CSourcePos(__LINE__, DBG_FUNCTION, __FILE__))

//{{{ Log level enable/disable functions
    /**
     * Enables or disables a particular debugging level.
     *
     * If you enable a debugging level twice you only need to disable it once.
     *
     * All diagnostic output is initially disabled. You can easily enable
     * output in your main() thus:
     * \code
     *   Debug::LogEnable( Debug::all, true );
     * \endcode
     *
     * Note that if Debug library calls specify a @ref TDebugSourceName then you
     * will also need to enable output for that particular source, with the
     * overloaded version of LogEnable.
     *
     * This enabling affects both constraint checking and diagnostic log
     * output.
     *
     * @param aLevel     Diagnostic level to enable/disable
     * @param aEnabled true to enable this diagnostic level, false to disable it
     * @see   Debug::LogEnableAll
     * @see   Debug::DBGOUT
     * @see   Debug::AttachStream
     */
void LogEnable( LogLevel aLevel, bool aEnabled );

    /**
     * In addition to the above LogEnable function, this overloaded version is
     * used when you use Debug APIs with a @ref TDebugSourceName specified. For these
     * versions of the APIs no debugging will be performed unless you
     * enable it with this API.
     *
     * To enable debugging for the "foobar" diagnostic source at the info
     * level you need to do the following:
     * \code
     *     Debug::LogEnable( Debug::info, true );
     *     Debug::LogEnable( Debug::info, "foobar", true );
     * \endcode
     *
     * If you enable a level for a particular @ref TDebugSourceName twice you only
     * need to disable it once.
     *
     * @param aLevel     Diagnostic level to enable/disable for the @ref TDebugSourceName
     * @param aSource    String describing the diagnostic source
     * @param aEnabled  true to enable this diagnostic level, false to disable it
     * @see   Debug::DBGOUT
     */
void LogEnable( LogLevel aLevel , TDebugSourceName aSource, bool aEnabled );

    /**
     * You may not know every single @ref TDebugSourceName that is generating
     * debugging in a particular code base. However, using this function
     * you can enable diagnostic levels for all sources in one fell swoop.
     *
     * For example,
     * \code
     *     Debug::LogEnableAll( Debug::all, true );
     * \endcode
     */
void LogEnableAll( LogLevel aLevel, bool aEnabled );
//}}}

//{{{ Stream attach/detach functions
    /**
     * Attaches the specified ostream to the given diagnostic level. Now
     * when diagnostics are produced at that level, this ostream will recieve
     * a copy.
     *
     * You can attach multiple ostreams to a diagnostic level. Be careful
     * that they don't go to the same place (e.g. cout and cerr both going
     * to your console) - this might confuse you!
     *
     * If you attach a ostream mutiple times it will only receive one
     * copy of the diagnostics, and you will only need to call
     * @ref DetachStream once.
     *
     * @param aLevel Diagnostic level
     * @param aStream   ostream to attach
     * @see   Debug::DetachStream
     * @see   Debug::DetachAllStreams
     */
void AttachStream( LogLevel aLevel, std::ostream &aStream );

    /**
     * Detaches the specified ostream from the given diagnostic level.
     *
     * If the ostream was not attached then no error is generated.
     *
     * @param aLevel Diagnostic level
     * @param aStream   ostream to dettach
     * @see   Debug::AttachStream
     * @see   Debug::DetachAllStreams
     */
void DetachStream( LogLevel aLevel, std::ostream &aStream );

    /**
     * Detaches all attached ostreams from the specified diagnostic level.
     *
     * @param aLevel Diagnostic level
     * @see   Debug::AttachStream
     * @see   Debug::DetachStream
     */
void DetachAllStreams( LogLevel aLevel );
//}}}

//{{{ Prefix
    /**
     * Sets the debugging prefix - the characters printed before any
     * diagnostic output. Defaults to "*** ".
     *
     * @param aPrefix New prefix string
     * @see Debug::Prefix
     * @see Debug::EnableLevelPrefix
     * @see Debug::EnableTimePrefix
     */
void SetPrefix( const char *aPrefix );

    /**
     * The Debug library can add to the @ref Prefix the name of the used
     * diagnostic level (e.g. info, fatal, etc).
     *
     * By default, this facility is disabled. This function allows you to
     * enable the facility.
     *
     * @param aEnabled true to enable level prefixing, false to disable
     * @see   Debug::SetPrefix
     * @see   Debug::EnableTimePrefix
     */
void EnableLevelPrefix( bool aEnabled );

    /**
     * The Debug library can add to the @ref Prefix the current time. This
     * can be useful when debugging systems which remain active for long
     * periods of time.
     *
     * By default, this facility is disabled. This function allows you to
     * enable the facility.
     *
     * The time is produced in the format of the standard library ctime
     * function.
     *
     * @param aEnabled true to enable time prefixing, false to disable
     * @see   Debug::SetPrefix
     * @see   Debug::EnableLevelPrefix
     */
void EnableTimePrefix( bool aEnabled );
void EnableTimePrefixUS( bool aEnabled );

    /**
     * Used so that you can produce a prefix in your diagnostic output in the
     * same way that the debugging library does.
     *
     * You can use it in one of two ways: with or without a diagnostic
     * @ref LogLevel. For the latter, if level prefixing is enabled (see
     * @ref EnableLevelPrefix) then produces a prefix including the
     * specified diagnostic level text.
     *
     * Examples of use:
     *
     * \code
     *     DBGOUG_INFO( Debug::Prefix() << "A Bad Thing happened\n" );
     *
     *     DBGOUT_INFO( Debug::Prefix( Debug::info ) << "A Bad Thing happened\n" );
     * \endcode
     *
     * @see Debug::Indent
     * @see Debug::SetPrefix
     * @see Debug::EnableLevelPrefix
     * @see Debug::EnableTimePrefix
     */
struct Prefix
{
        /**
         * Creates a prefix with no specified diagnostic @ref LogLevel.
         * No diagnostic level text will be included in the prefix.
         */
   Prefix() : theLogLevel( none ) {};

        /**
         * @param aLevel Diagnostic @ref LogLevel to include in prefix
         */
   Prefix( LogLevel aLevel ) : theLogLevel( aLevel ) {};

   LogLevel theLogLevel;
};

    /**
     * This is called when you use the @ref Prefix stream manipulator.
     *
     * @internal
     * @see Debug::Prefix
     */
std::ostream &operator<<( std::ostream &aStream, const Prefix &aPrefix );
//}}}

//{{{ Indent
    /**
     * Used so that you can indent your diagnostic output to the same level
     * as the debugging library. This also produces the @ref Prefix output.
     *
     * Examples of use:
     *
     * \code
     *     DBGOUT_INFO( Debug::Indent() << "A Bad Thing happened\n" );
     *
     *     DBGOUT_INFO( Debug::Indent( Debug::info ) << "A Bad Thing happened\n" );
     * \endcode
     *
     * @see Debug::Prefix
     * @see Debug::SetPrefix
     * @see Debug::EnableLevelPrefix
     * @see Debug::EnableTimePrefix
     */
struct Indent
{
        /**
         * Creates a indent with no specified diagnostic @ref LogLevel.
         * No diagnostic level text will be included in the @ref Prefix part.
         */
   Indent() : theLogLevel( none ) {}

        /**
         * @param aLevel Diagnostic level to include in prefix
         */
   Indent( LogLevel aLevel ) : theLogLevel( aLevel ) {}

   LogLevel theLogLevel;
};

    /**
     * This is called when you use the @ref Indent stream manipulator.
     *
     * @internal
     * @see Debug::Indent
     */
std::ostream &operator<<( std::ostream &aStream, const Indent &aIndent );
//}}}

//{{{ Backtrace

const int BacktraceSize = 30;

//! Class to read and print backtrace information
/*! 
  In current version is only glibc support, but it don't work.
  May be in next version.
  */
class Backtrace
{
   void * theBacktraceArray[BacktraceSize];

   char ** theBacktraceSymbols;

   int theReallyLength;

   int theStartSkipLevel, theEndSkipLevel;

   Backtrace( const Backtrace & );

   Backtrace & operator=( const  Backtrace & );
public:
   Backtrace( int aStartSkipLevel = 0, int aEndSkipLevel = 2 );
   ~Backtrace();

   inline char ** Symbols() const
   { return theBacktraceSymbols; };

   inline int ReallyLength() const
   { return theReallyLength; };
   inline int GetStartSkipLevel()const
   { return theStartSkipLevel; };
   inline int GetEndSkipLevel()const
   { return theEndSkipLevel; };

};

std::ostream &operator<<( std::ostream &aStream, const Backtrace &aBacktrace );

//}}}

//{{{ Debug stream
//! Interface class for buffer, that support differen message levels
class CLogStreamBuf : public std::streambuf
{
public:
   virtual void SetBufLogLevel( LogLevel aLevel )=0;
};

//! Class with printf fuction support
class CPrintfOStream : public std::ostream
{
   char thePrintfBuffer[1024];
public:
   inline CPrintfOStream( std::streambuf *aBuffer ): std::ostream( aBuffer ) {};
   inline ~CPrintfOStream() {};

   int printf(const char *format, ...);
};

//! Stream with differen message levels support
class CDbgOStream : public CPrintfOStream
{
   CLogStreamBuf *theBuffer;
   LogLevel theLevel;
public:
   CDbgOStream( CLogStreamBuf *aBuffer );
   ~CDbgOStream();
   void SetLogLevel( LogLevel aLevel );
};

    /**
     * Returns an CDbgOStream suitable for sending diagnostic messages to.
     * Each diagnostic level has a different logging ostream which can be
     * enabled/disabled independantly. In addition, each @ref TDebugSourceName
     * has separate enables/disables for each diagnostic level.
     *
     * This overloaded version of out is used when you are creating diagnostics
     * that are tied to a particular @ref TDebugSourceName.
     *
     * It allows you to write code like this:
     * \code
     *   Debug::DBGOUT( Debug::info, "foobar" ) << "The foobar is flaky\n";
     * \endcode
     *
     * If you want to prefix your diagnostics with the standard Debug library
     * prefix (see @ref SetPrefix) then use the @ref Prefix or @ref Indent
     * stream manipulators.
     *
     * @param aLevel  Diagnostic level get get ostream for
     * @param aSource String describing the diagnostic source
     */
Debug::CDbgOStream &DBGOUT( LogLevel aLevel, TDebugSourceName aSource );

    /**
     * Returns an CDbgOStream suitable for sending diagnostic messages to.
     * Each diagnostic level has a different logging ostream which can be
     * enabled/disabled independantly.
     *
     * You use this version of out when you are creating diagnostics
     * that aren't tidied to a particular @ref TDebugSourceName.
     *
     * It allows you to write code like this:
     * \code
     *   DBGOUT_INFO( "The code is flaky\n" );
     * \endcode
     *
     * If you want to prefix your diagnostics with the standard Debug library
     * prefix (see @ref SetPrefix) then use the @ref Prefix or @ref Indent
     * stream manipulators.
     *
     * @param aLevel Diagnostic level get get ostream for
     */
inline Debug::CDbgOStream &DBGOUT( LogLevel aLevel )
{
   return DBGOUT( aLevel, 0 );
}
//}}}

#else
//{{{ Non-debug log stub versions

    /**
     * With debugging switched off we generate null versions of the above
     * definitions.
     *
     * Given a good compiler and a strong prevailing headwind, these will
     * optimise away to nothing.
     */


#define CURR_POS         ((void*)0)

class CNullStream
{
public:
#ifdef _MSC_VER
   CNullStream &operator<<( void * )        { return *this; }
   CNullStream &operator<<( const void * )  { return *this; }
   CNullStream &operator<<( long )          { return *this; }
#else
   template <class TType>
      CNullStream &operator<<( const TType & ) { return *this; }
#endif

   template <class TType>
      CNullStream &operator<<( TType & )       { return *this; }

   int printf(const char *format, ...)       { return 0; };

};

struct CPrefix { CPrefix() {} CPrefix( LogLevel ) {} };
struct CIndent { CIndent() {} CIndent( LogLevel ) {} };

inline void        LogEnable( LogLevel, bool )                   {}
inline void        LogEnable( LogLevel, TDebugSourceName, bool ) {}
inline void        LogEnableAll( LogLevel, bool )                {}
inline CNullStream DBGOUT( LogLevel, TDebugSourceName )          {return CNullStream();}
inline CNullStream DBGOUT( LogLevel )                            {return CNullStream();}
inline void        AttachStream( LogLevel, std::ostream & )      {}
inline void        DetachStream( LogLevel, std::ostream & )      {}
inline void        DetachAllStreams( LogLevel )                  {}
inline void        SetPrefix( const char * )                     {}
inline void        EnableLevelPrefix( bool )                     {}
inline void        EnableTimePrefix( bool )                      {}

//}}}
#endif
//}}}

//{{{ Assert
//{{{ Assert Mode
enum AssertMode
{
   ASSERT_DIE,             //!< print info and exit
   ASSERT_THROW,           //!< print info and throw exception
   ASSERT_CONTINUE,        //!< print info and continue work
   ASSERT_CONTINUE_CORE    //!< print info, create core and continue work
};//}}}

//{{{ Exceptions

    /**
     * The base type of exception thrown by Debug assertions (and other Debug
     * library constraint checks) if the @ref AssertMode is set to
     * assertions_throw.
     *
     * The exception keeps a record of the source position of the trigger
     * for this exception.
     */
struct DebugException : public std::exception
{
   DebugException( const CSourcePos &aSourcePos, const char * aText = 0 )
      : thePosition( aSourcePos ), theText( aText )
      {}
   const CSourcePos thePosition;
   const char * const theText;
};

struct AssertException : public DebugException
{
   AssertException( const CSourcePos &aSourcePos, const char * aText = 0 )
      : DebugException( aSourcePos, aText) {}
};

std::ostream &operator<<( std::ostream &aStream, const AssertException & aAssertExp );
//}}}

//{{{ Assertion realisation
    /**
     * Used to assert a constraint in your code.
     *
     *
     * To use assertion for a @ref TDebugSourceName "foobar" you write code like:
     * \code
     *     int i = 0;
     *     LevelSourceAssert( i !=0, info, "foobar" );
     * \endcode
     *
     * If you build with debugging enabled the program will
     * produce diagnostic output to the relevant output stream if the
     * constraint fails, and the appropriate @ref AssertMode
     * is enacted.
     *
     */

//! Internal assertion function with special text preffix
void __TextAssertion(  const char * aPrefixText, const char * aText = 0,
      LogLevel aLevel = Debug::error, TDebugSourceName aSourceName = 0,
      TLineNumber aLine = 0 , TFunctionName aFuncName = 0,
      TFileName aFileName = 0 );

//! Internal assertion function
void __Assertion(  const char * aText = 0,
      LogLevel aLevel = Debug::fatal, TDebugSourceName aSourceName = 0,
      TLineNumber aLine = 0 , TFunctionName aFuncName = 0,
      TFileName aFileName = 0 );
//}}}

//{{{ Assertion behaviour

    /**
     * Sets what happens when assertions (or other constraints) trigger. There
     * will always be diagnostic ouput. Assertions have 'abort' behaviour by
     * default - like the ISO C standard, they cause an abort.
     *
     * If an assertion is encountered at the fatal level, the debugging library
     * will abort the program regardless of this behaviour setting.
     *
     * If a diagnostic level is not enabled (see @ref LogEnable) then the
     * @ref SetAssertionMode is not enacted.
     *
     * @param aLogLevel       Diagnostic level to set behaviour for
     * @param aMode        Assertion mode
     * @see   Debug::LogEnable

     */
void SetAssertionMode( LogLevel aLogLevel, AssertMode aMode );
//}}}

// {{{ Invariant guard
class CGuard
{
};

inline void DummyFunc( CGuard const & )
{};


template< class CObject >
class CInvariantGuard : public  CGuard
{
   CObject & theObjRef;
   int theLine;
   const char * theFuncName;
   const char * theFileName;
protected:
public:
   CInvariantGuard( CObject& aObject, int aLine, const char * aFuncName,
         const char * aFileName )
      : theObjRef( aObject ), theLine( aLine), theFuncName( aFuncName ), theFileName( aFileName )
   {
      if( !theObjRef.invariant() )
          __TextAssertion( "Invariant check failed at function call: ", 0,
                Debug::fatal, 0, theLine, theFuncName, theFileName );

   };
public:
   ~CInvariantGuard() throw()
   {
      try
      {
         if( !theObjRef.invariant() )
            __TextAssertion( "Invariant check failed at function return: ", 0,
                Debug::fatal, 0, 0, theFuncName, theFileName );
      }
      catch(...)
      {
      }
   };
private:
   CInvariantGuard &operator=( const CInvariantGuard & ) {};
};

template <class CObject>
inline CInvariantGuard<CObject> MakeInvGuard( CObject& aObject , int aLine,
      const char * aFuncName, const char * aFileName )
{
	return CInvariantGuard<CObject>( aObject, aLine, aFuncName, aFileName );
}
//}}}
//}}}

//{{{ Syslog
// {{{ Syslog enums
enum SyslogOptions
{
   SYSLOG_CONS   = LOG_CONS,   // Write directly to system console
   // if there is an error while sending to system logger.
   SYSLOG_NDELAY = LOG_NDELAY, // Open  the connection immediately
   // (normally, the connection is opened when
   // the first message is logged).
   SYSLOG_NOWAIT = LOG_NOWAIT, // Don't wait for child processes that may have been
   // created while  logging	the message.	 (The GNU C library does not create a child process, so this option has no effect on Linux.)
   SYSLOG_ODELAY = LOG_ODELAY, // The converse of LOG_NDELAY;                                                                                                                                                          opening of the connection is delayed	until  sys- log() is called.	(This is the default, and need not be specified.)
   SYSLOG_PERROR = LOG_PERROR, // (Not in SUSv3.) Print to stderr as well.
   SYSLOG_PID    = LOG_PID     // Include PID with each message.
};
enum SyslogLevel
{
   SYSLOG_EMERG   = LOG_EMERG,   // system is unusable
   SYSLOG_ALERT   = LOG_ALERT,   // action must be taken immediately
   SYSLOG_CRIT    = LOG_CRIT,    // critical conditions
   SYSLOG_ERR     = LOG_ERR,     // error conditions
   SYSLOG_WARNING = LOG_WARNING, // warning conditions
   SYSLOG_NOTICE  = LOG_NOTICE,  // normal, but significant, condition
   SYSLOG_INFO    = LOG_INFO,    // informational message
   SYSLOG_DEBUG   = LOG_DEBUG,   // debug-level message
   SYSLOG_NONE
};
enum SyslogFacility
{
   SYSLOG_AUTH     = LOG_AUTH,     // security/authorization messages
   // (DEPRECATED	Use  LOG_AUTHPRIV instead)
   SYSLOG_AUTHPRIV = LOG_AUTHPRIV, // security/authorization messages (private)
   SYSLOG_CRON     = LOG_CRON,     // clock daemon (cron and at)
   SYSLOG_DAEMON   = LOG_DAEMON,   // system daemons without separate facility value
   SYSLOG_FTP      = LOG_FTP,      // ftp daemon
   SYSLOG_KERN     = LOG_KERN,     // kernel messages
   SYSLOG_LOCAL0   = LOG_LOCAL0,   // reserved for local use
   SYSLOG_LOCAL1   = LOG_LOCAL1,   // reserved for local use
   SYSLOG_LOCAL2   = LOG_LOCAL2,   // reserved for local use
   SYSLOG_LOCAL3   = LOG_LOCAL3,   // reserved for local use
   SYSLOG_LOCAL4   = LOG_LOCAL4,   // reserved for local use
   SYSLOG_LOCAL5   = LOG_LOCAL5,   // reserved for local use
   SYSLOG_LOCAL6   = LOG_LOCAL6,   // reserved for local use
   SYSLOG_LOCAL7   = LOG_LOCAL7,   // reserved for local use
   SYSLOG_LPR      = LOG_LPR,      // line printer subsystem
   SYSLOG_MAIL     = LOG_MAIL,     // mail subsystem
   SYSLOG_NEWS     = LOG_NEWS,     // USENET news subsystem
   SYSLOG_SYSLOG   = LOG_SYSLOG,   // messages generated internally by syslogd
   SYSLOG_USER     = LOG_USER,     // (default) generic user-level messages
   SYSLOG_UUCP     = LOG_UUCP      // UUCP subsystem
}; // }}}

// {{{ class Syslog : virtual public CLogStreamBuf, virtual public CDbgOStream

class CSyslog : virtual public CLogStreamBuf, virtual public CDbgOStream
{
   int            theOptions;
   SyslogFacility theFacility;
   SyslogLevel    theLevel;

   /**
    * Pointer to next char in buffer.
    */
   char *ptr;

   /**
    * Current length of buffer.
    */
   unsigned int len;

   /**
    * The log level to use with syslog.
    */
   int level;

   /**
    * The internal buffer for holding
    * messages.
    */
   char buf[1024];

   protected:
   int overflow( int c = EOF );


   public:
   /**
    * Default (and only) constructor.  The default log level is set to
    * SYSLOG_DEBUG.  There is no default log facility set.  One should be
    * set before attempting any output.  This is done by the <code>open()</code> or the
    * <code>operator()(const char*, slog_class_t, slog_level_t)</code>
    * functions.
    */
   CSyslog();

   virtual ~CSyslog();


   /**
    * opens the output stream.
    * @param aIdent      The identifier portion of the message sent to the syslog daemon.
    * @param aOptions    Syslog options
    * @param aFacility   Syslog facility, defalult SYSLOG_USER
    */
   void Open( const char * aIdent, int aOptions = 0, SyslogFacility aFacility = SYSLOG_USER );

   void Close(void);

   /**
    * Sets the logging level.
    * @param aLevel is the logging level to use for further output
    */
   virtual void SetBufLogLevel( LogLevel aLevel );

   CSyslog & operator()(const char * aIdent, int aOptions, SyslogFacility aFaciliry );
   inline CSyslog & operator()( LogLevel aLevel )
   { SetLogLevel( aLevel ); return *this; };
   inline CSyslog & operator()( SyslogLevel aLevel )
   { theLevel = aLevel; return *this; };

}; // }}} END: class Syslog

// {{{ Access functions
CSyslog & Syslog( Debug::LogLevel aLevel = debug );
CSyslog & Syslog( const char * aIdent, int aOptions = 0, SyslogFacility aFaciliry = SYSLOG_USER );
// }}}

//}}}

#ifdef DEBUG_WORK
//{{{ Debug mode
bool ParseDebugOptions( int argc, char ** argv );

boost::recursive_mutex & GetLock();

#define DEBUG_LOCK boost::lock_guard<boost::recursive_mutex> guard( Debug::GetLock() )

#       define DEBUG_CODE(expr)     expr

//{{{ Log
#       define DBGOUT_FATAL(expr)   { DEBUG_LOCK; Debug::DBGOUT( Debug::fatal   ) << expr ;}
#       define DBGOUT_ERROR(expr)   { DEBUG_LOCK; Debug::DBGOUT( Debug::error   ) << expr ;}
#       define DBGOUT_WARNING(expr) { DEBUG_LOCK; Debug::DBGOUT( Debug::warning ) << expr ;}
#       define DBGOUT_INFO(expr)    { DEBUG_LOCK; Debug::DBGOUT( Debug::info    ) << expr ;}
#       define DBGOUT_DEBUG(expr)   { DEBUG_LOCK; Debug::DBGOUT( Debug::debug   ) << expr ;}
#       define DBGOUT_TRACING(expr) { DEBUG_LOCK; Debug::DBGOUT( Debug::tracing ) << expr ;}
#       define DBGOUT_NONE(expr)    { DEBUG_LOCK; Debug::DBGOUT( Debug::none    ) << expr ;}
#       define DBGOUT_ALL(expr)     { DEBUG_LOCK; Debug::DBGOUT( Debug::all     ) << expr ;}
#       define DBGOUT_CURR_POS      { DEBUG_LOCK; Debug::DBGOUT( Debug::tracing ) << CURR_POS << "\n" ;}

#if defined(__GNUC__) && ( __GNUC__< 3 )
#       define DBGPRINT_FATAL(format)   { DEBUG_LOCK; Debug::DBGOUT( Debug::fatal   ).printf( format ) ;}
#       define DBGPRINT_ERROR(format)   { DEBUG_LOCK; Debug::DBGOUT( Debug::error   ).printf( format ) ;}
#       define DBGPRINT_WARNING(format) { DEBUG_LOCK; Debug::DBGOUT( Debug::warning ).printf( format ) ;}
#       define DBGPRINT_INFO(format)    { DEBUG_LOCK; Debug::DBGOUT( Debug::info    ).printf( format ) ;}
#       define DBGPRINT_DEBUG(format)   { DEBUG_LOCK; Debug::DBGOUT( Debug::debug   ).printf( format ) ;}
#       define DBGPRINT_TRACING(format) { DEBUG_LOCK; Debug::DBGOUT( Debug::tracing ).printf( format ) ;}
#       define DBGPRINT_NONE(format)    { DEBUG_LOCK; Debug::DBGOUT( Debug::none    ).printf( format ) ;}
#       define DBGPRINT_ALL(format)     { DEBUG_LOCK; Debug::DBGOUT( Debug::all     ).printf( format ) ;}

#       define DBGPRINT_FATAL(format,p1)   { DEBUG_LOCK; Debug::DBGOUT( Debug::fatal   ).printf( format,p1 ) ;}
#       define DBGPRINT_ERROR(format,p1)   { DEBUG_LOCK; Debug::DBGOUT( Debug::error   ).printf( format,p1 ) ;}
#       define DBGPRINT_WARNING(format,p1) { DEBUG_LOCK; Debug::DBGOUT( Debug::warning ).printf( format,p1 ) ;}
#       define DBGPRINT_INFO(format,p1)    { DEBUG_LOCK; Debug::DBGOUT( Debug::info    ).printf( format,p1 ) ;}
#       define DBGPRINT_DEBUG(format,p1)   { DEBUG_LOCK; Debug::DBGOUT( Debug::debug   ).printf( format,p1 ) ;}
#       define DBGPRINT_TRACING(format,p1) { DEBUG_LOCK; Debug::DBGOUT( Debug::tracing ).printf( format,p1 ) ;}
#       define DBGPRINT_NONE(format,p1)    { DEBUG_LOCK; Debug::DBGOUT( Debug::none    ).printf( format,p1 ) ;}
#       define DBGPRINT_ALL(format,p1)     { DEBUG_LOCK; Debug::DBGOUT( Debug::all     ).printf( format,p1 ) ;}
#else
#       define DBGPRINT_FATAL(format, ...)   { DEBUG_LOCK; Debug::DBGOUT( Debug::fatal   ).printf( format, ##__VA_ARGS__ ) ;}
#       define DBGPRINT_ERROR(format, ...)   { DEBUG_LOCK; Debug::DBGOUT( Debug::error   ).printf( format, ##__VA_ARGS__ ) ;}
#       define DBGPRINT_WARNING(format, ...) { DEBUG_LOCK; Debug::DBGOUT( Debug::warning ).printf( format, ##__VA_ARGS__ ) ;}
#       define DBGPRINT_INFO(format, ...)    { DEBUG_LOCK; Debug::DBGOUT( Debug::info    ).printf( format, ##__VA_ARGS__ ) ;}
#       define DBGPRINT_DEBUG(format, ...)   { DEBUG_LOCK; Debug::DBGOUT( Debug::debug   ).printf( format, ##__VA_ARGS__ ) ;}
#       define DBGPRINT_TRACING(format, ...) { DEBUG_LOCK; Debug::DBGOUT( Debug::tracing ).printf( format, ##__VA_ARGS__ ) ;}
#       define DBGPRINT_NONE(format, ...)    { DEBUG_LOCK; Debug::DBGOUT( Debug::none    ).printf( format, ##__VA_ARGS__ ) ;}
#       define DBGPRINT_ALL(format, ...)     { DEBUG_LOCK; Debug::DBGOUT( Debug::all     ).printf( format, ##__VA_ARGS__ ) ;}
#endif
//}}}

//{{{ Tracer
#   define DEBUG_TRACER Debug::CTracer ANONYMOUS_VARIABLE(CurrentFunctionTracer) ( __FILE__, CURR_POS );

    /**************************************************************************
     * Tracing
     *************************************************************************/

    /*!
     * The CTracer class allows you to easily produce tracing diagnostics.
     *
     * When the CTracer constructor is called, it prints "->" and the name of the
     * function, increasing the indent level. When the object is deleted
     * it prints "<-" followed again by the name of the function.
     *
     * Diagnostics are produced at the tracing @ref LogLevel.
     *
     * For example, if you write the following code:
     *
     * \code
         void test_func2()
         {
            DEBUG_TRACER;
            DBGOUT_INFO( Debug::Prefix() << "we are in test_func2\n" );
         };

         void test_func( int param = 0 )
         {
            DEBUG_TRACER;
            DBGOUT_INFO( Debug::Prefix() << "Param = " << param << std::endl );
            DBGOUT_INFO( Debug::Prefix() << "we are in test_func\n" );
         
            usleep( 1000000 );
            test_func2();
         };
     * \endcode
     *
     * You will get the following tracing information:
      <pre>
      * * *   -> Call:   void test_func(int) (66 in main.cpp)
      * * * Param = 4
      * * * we are in test_func
      * * *     -> Call:   void test_func2() (59 in main.cpp)
      * * * we are in test_func2
      * * *     <- Return: void test_func2() time = 0s, 511us
      * * *   <- Return: void test_func(int) time = 1s, 8202us
      </pre>
     *
     *
       For easy creation CTracer object, the Debug library provides
       DEBUG_TRACER macros.
     *
     * If you disable the tracing diagnostic @ref LogLevel before the trace
     * object's destructor is called you will still get the closing trace
     * output. This is important, otherwise the indentation level of the
     * library would get out of sync. In this case, the closing diagnostic
     * output will have a "note" attached to indicate what has happened.
     *
     * Similarly, if tracing diagnostics are off when the trace object is
     * created, yet subsequencently enabled before the destructor there will
     * be no closing tracing ouput.
     */
class CTracer
{
public:
   /**
    * Provide the function name, or some other tracing string.
    *
    * This will not tie the trace object to a particular
    * @ref TDebugSourceName.
    *
    * @param aName Tracing block name
    */
   CTracer( TFunctionName aName );

   /**
    * @param aSource  String describing the diagnostic source
    * @param aName    Tracing block name
    */
   CTracer( TDebugSourceName aSource, TFunctionName aName );

   CTracer(const CSourcePos &aSourcePos);

   CTracer( TDebugSourceName aSource, const CSourcePos &aSourcePos );

   ~CTracer();

private:

   CTracer( const CTracer& );
   CTracer &operator=( const CTracer& );

   void StartTrace();
   void StopTrace();

   TDebugSourceName  theSourceName;
   const char       *theName; 
   const CSourcePos  theSourcePos;
   bool              theTriggered;
   struct timeval    theStartTime;
};
//}}}

//{{{ Assert
#ifdef USE_DEBUG_ASSERT
#  undef  assert
#	define assert(cond) Assert(cond)
#endif

#	define Assert(EX) ((EX)? NULL_EXPR : __Assertion( #EX, Debug::error, 0, \
         __LINE__, DBG_FUNCTION, __FILE__ ))
#	define LevelAssert(EX,LEV) ((EX)? NULL_EXPR : __Assertion( #EX, LEV, 0, \
         __LINE__, DBG_FUNCTION, __FILE__ ))
#	define SourceAssert(EX,SRC) ((EX)? NULL_EXPR : __Assertion( #EX, Debug::error, SRC, \
         __LINE__, DBG_FUNCTION, __FILE__ ))
#	define LevelSourceAssert(EX,LEV,SRC) ((EX)? NULL_EXPR : __Assertion( #EX, LEV, SRC, \
         __LINE__, DBG_FUNCTION, __FILE__ ))
#  define NOT_REACHED  __TextAssertion( "Reached unreachable code ", 0, Debug::error, 0, \
         __LINE__, DBG_FUNCTION, __FILE__ )

#  ifdef EIFFEL_CHECK
#     define REQUIRE(EX) ((EX)? NULL_EXPR : __TextAssertion( "REQUIRE assertion failed: ", #EX, Debug::fatal, 0, \
         __LINE__, DBG_FUNCTION, __FILE__ ))
#     define ENSURE(EX) ((EX)? NULL_EXPR : __TextAssertion( "ENSURE assertion failed: ", #EX, Debug::fatal, 0, \
         __LINE__, DBG_FUNCTION, __FILE__ ))
#     define INVARIANT (( this->invariant() )? NULL_EXPR : __TextAssertion( "INVARIANT check failed: ", 0, Debug::fatal, 0, \
         __LINE__, DBG_FUNCTION, __FILE__ ))
#     define CHECK_INVARIANT Debug::CGuard const & ANONYMOUS_VARIABLE(CInvariantGuard) = \
      Debug::MakeInvGuard( *this, __LINE__, DBG_FUNCTION, __FILE__ ); DummyFunc( ANONYMOUS_VARIABLE(CInvariantGuard) );
#  else
#     define REQUIRE(EX)      NULL_EXPR;
#     define ENSURE(EX)       NULL_EXPR;
#     define INVARIANT        NULL_EXPR;
#     define CHECK_INVARIANT  NULL_EXPR;
#  endif
//}}}
//}}}
#else
//{{{ Release mode
bool ParseDebugOptions( int /*argc*/, char ** /*argv*/ );

#       define DEBUG_CODE(expr)     NULL_EXPR                                                        

//{{{ Log
#       define DBGOUT_FATAL(expr)   NULL_EXPR
#       define DBGOUT_ERROR(expr)   NULL_EXPR
#       define DBGOUT_WARNING(expr) NULL_EXPR
#       define DBGOUT_INFO(expr)    NULL_EXPR
#       define DBGOUT_DEBUG(expr)   NULL_EXPR
#       define DBGOUT_TRACING(expr) NULL_EXPR
#       define DBGOUT_NONE(expr)    NULL_EXPR
#       define DBGOUT_ALL(expr)     NULL_EXPR
#       define DBGOUT_CURR_POS      NULL_EXPR

#if defined(__GNUC__) && ( __GNUC__< 3 )                   
#       define DBGPRINT_FATAL(format)   NULL_EXPR
#       define DBGPRINT_ERROR(format)   NULL_EXPR
#       define DBGPRINT_WARNING(format) NULL_EXPR
#       define DBGPRINT_INFO(format)    NULL_EXPR
#       define DBGPRINT_DEBUG(format)   NULL_EXPR
#       define DBGPRINT_TRACING(format) NULL_EXPR
#       define DBGPRINT_NONE(format)    NULL_EXPR
#       define DBGPRINT_ALL(format)     NULL_EXPR

#       define DBGPRINT_FATAL(format,p1)   NULL_EXPR
#       define DBGPRINT_ERROR(format,p1)   NULL_EXPR
#       define DBGPRINT_WARNING(format,p1) NULL_EXPR
#       define DBGPRINT_INFO(format,p1)    NULL_EXPR
#       define DBGPRINT_DEBUG(format,p1)   NULL_EXPR
#       define DBGPRINT_TRACING(format,p1) NULL_EXPR
#       define DBGPRINT_NONE(format,p1)    NULL_EXPR
#       define DBGPRINT_ALL(format,p1)     NULL_EXPR
#else
#       define DBGPRINT_FATAL(format, ...)   NULL_EXPR
#       define DBGPRINT_ERROR(format, ...)   NULL_EXPR
#       define DBGPRINT_WARNING(format, ...) NULL_EXPR
#       define DBGPRINT_INFO(format, ...)    NULL_EXPR
#       define DBGPRINT_DEBUG(format, ...)   NULL_EXPR
#       define DBGPRINT_TRACING(format, ...) NULL_EXPR
#       define DBGPRINT_NONE(format, ...)    NULL_EXPR
#       define DBGPRINT_ALL(format, ...)     NULL_EXPR
#endif                                

//}}}

//{{{ Tracer
#   define DEBUG_TRACER NULL_EXPR

class CTracer
{
public:
   CTracer( const char * )                   {}
   CTracer( TDebugSourceName, const char * ) {}
   CTracer( void * )                         {}
   CTracer( TDebugSourceName, void * )       {}
   ~CTracer()                                {}
};
//}}}

//{{{ Assert
#	undef Assert
#	define Assert(EX) NULL_EXPR;
#	define LevelAssert(EX,LEV) NULL_EXPR;
#	define SourceAssert(EX,SRC) NULL_EXPR;
#	define LevelSourceAssert(EX,LEV,SRC) NULL_EXPR;
#  define NOT_REACHED  NULL_EXPR;
#  define REQUIRE(EX)  NULL_EXPR;
#  define ENSURE(EX)   NULL_EXPR;
#  define INVARIANT    NULL_EXPR;
#  define CHECK_INVARIANT  NULL_EXPR;
//}}}
//}}}
#endif
};

#endif /* ifndef DEBUG_H */

/* {{{ Modeline for ViM
 * vim600:fdm=marker fdl=0 fdc=3:
 * }}} */
