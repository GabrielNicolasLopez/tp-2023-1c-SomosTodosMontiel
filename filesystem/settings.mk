#Libraries
LIBS= cspecs shared m commons pthread readline cunit rt

#Custom libraries' paths
SHARED_LIBPATHS=
STATIC_LIBPATHS=../shared

#Compiler flags
CDEBUG=-g -Wall -Werror -DDEBUG
CRELEASE=-g -O3 -Wall -DNDEBUG

#Arguments when executing with start, memcheck or helgrind
ARGS=./cfg/filesystem.cfg

#Valgrind flags
MEMCHECK_FLAGS=--tool=memcheck --leak-check=full --track-origins=yes --show-possibly-lost=no --show-reachable=no --num-callers=20
HELGRIND_FLAGS=--log-file="helgrind.log"

#Source files (*.c) to be excluded from tests compilation
TEST_EXCLUDE=src/filesystem.c