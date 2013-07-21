PROGRAM = main

TEST_PROGRAM = $(PROGRAM)Test

ASM = nasm
AFLAGS = -f elf

#INCLUDES  		= -I/usr/include/stlport -I.
INCLUDES  		=  -I.
#LIBS = -lstlport_gcc3 -lstdc++ 
LIBS =  -lstdc++ -lboost_thread -lloki -lpthread
DEBUG_LIBS = -lcppunit-gcc3.2
DEPENDS_FILES = ./*.cpp

#CC = gcc
CC = gcc -DDEBUG_ENABLED -DEIFFEL_CHECK -std=c++0x

# without Debug-Info
#CFLAGS  =       -pipe -Wall -W -O0 -Wpointer-arith
# with Debug-Info
CFLAGS  =       -pipe -Wall -W -ggdb -O1 -Wpointer-arith
#CFLAGS  =       -pipe -Wall -W -O3 -Wpointer-arith

PROGRAM_OBJECT = $(PROGRAM).o

TEST_PROGRAM_OBJECT = $(TEST_PROGRAM).o

TEST_OBJECTS =

OBJECTS = Debug.o TimerSystem.o ActiveObject.o Event.o

OPTIMIZED_OBJECTS =

# without Debug-Info 	
#$(OPTIMIZED_OBJECTS) : override CFLAGS = -O2 -pipe -Wall -W -Wpointer-arith
# with Debug-Info
$(OPTIMIZED_OBJECTS) : override CFLAGS = -O2 -pipe -Wall -W -ggdb -Wpointer-arith

MAX_OPTIMIZED_OBJECTS = 
	
# without Debug-Info
#$(MAX_OPTIMIZED_OBJECTS) : override CFLAGS = -O3 -pipe -Wall -W -Wpointer-arith
# with Debug-Info
$(MAX_OPTIMIZED_OBJECTS) : override CFLAGS = -O3 -pipe -Wall -W -ggdb -Wpointer-arith

# implementation
.SUFFIXES:	.o .asm
.SUFFIXES:	.o .cpp
.SUFFIXES:	.o .c

.asm.o :
	$(ASM) $(AFLAGS) $<
	
.cpp.o :
	$(CC) $(CFLAGS) -c $(INCLUDES) -o $@ $<

.c.o :
	$(CC) $(CFLAGS) -c $(INCLUDES) -o $@ $<


all: $(PROGRAM)  

test: $(TEST_PROGRAM)

$(PROGRAM):	$(PROGRAM_OBJECT) $(OBJECTS) $(OPTIMIZED_OBJECTS) $(MAX_OPTIMIZED_OBJECTS)
	$(CC) -o $(PROGRAM) $(PROGRAM_OBJECT) $(OBJECTS) $(OPTIMIZED_OBJECTS) $(MAX_OPTIMIZED_OBJECTS) $(LIBS)

$(TEST_PROGRAM):	$(TEST_PROGRAM_OBJECT) $(TEST_OBJECTS) $(OBJECTS) $(OPTIMIZED_OBJECTS) $(MAX_OPTIMIZED_OBJECTS)
	$(CC) -o $(TEST_PROGRAM) $(TEST_PROGRAM_OBJECT) $(TEST_OBJECTS) $(OBJECTS) $(OPTIMIZED_OBJECTS) $(MAX_OPTIMIZED_OBJECTS) $(LIBS) $(DEBUG_LIBS)

clean: 
	rm -f $(PROGRAM) $(TEST_PROGRAM) $(PROGRAM_OBJECT) $(TEST_PROGRAM_OBJECT) $(OBJECTS) $(OPTIMIZED_OBJECTS) $(MAX_OPTIMIZED_OBJECTS) $(TEST_OBJECTS) .depends core
			

.depends: 
	$(CC) -MM $(DEPENDS_FILES) $(INCLUDES) >.depends

vim_run: $(PROGRAM)
	./$(PROGRAM)

test-run: $(TEST_PROGRAM)
	./$(TEST_PROGRAM)

-include .depends

