# SchedulX backend. Works with GNU make / mingw32-make and GCC.
CC      ?= gcc
CFLAGS  = -std=gnu11 -Wall -Wextra -O2 -Iinclude -D__USE_MINGW_ANSI_STDIO=1
CORE    = src/strbuf.c src/linkedlist.c src/hashtable.c src/queue.c src/stack.c src/heap.c src/graph.c \
          src/process.c src/stats.c src/scheduler.c
SERVER  = src/api.c src/http_server.c src/main.c

ifeq ($(OS),Windows_NT)
  EXE = .exe
  LIBS = -lws2_32
  RM = if exist bin\*.exe del /q bin\*.exe
else
  EXE =
  LIBS = -pthread
  RM = rm -f bin/schedulx bin/test_scheduler
endif

all: bin/schedulx$(EXE)

bin:
	mkdir bin

bin/schedulx$(EXE): $(CORE) $(SERVER) | bin
	$(CC) $(CFLAGS) $(CORE) $(SERVER) -o $@ $(LIBS)

bin/test_scheduler$(EXE): $(CORE) tests/test_scheduler.c | bin
	$(CC) $(CFLAGS) $(CORE) tests/test_scheduler.c -o $@ -lm

test: bin/test_scheduler$(EXE)
	./bin/test_scheduler$(EXE)

clean:
	$(RM)

.PHONY: all test clean
