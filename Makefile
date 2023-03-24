CC=gcc

CPPFLAGS=-Idependencies/include
LDFLAGS=-Ldependencies/lib
CFLAGS=-O3 -std=gnu99 -Wall -Wextra 
LDLIBS=-lpthread -ljson-c

OBJS=$(subst .c,.o,$(shell find ./src -name "*.c"))
DEPJSONC=dependencies/lib/libjson-c.a
BIN=typespeed

all: $(BIN)

$(BIN) : $(OBJS) $(DEPJSONC)
	$(CC) $(OBJS) -o $@ $(CPPFLAGS) $(CFLAGS) $(LDLIBS) $(LDFLAGS)

%.o : %.c $(DEPJSONC)
	$(CC) -c $< -o $@ $(CPPFLAGS) $(CFLAGS) $(LDLIBS) $(LDFLAGS)

$(DEPJSONC) :
	$(shell cd dependencies; ./install-deps.sh)

clean-all:
	$(MAKE) clean
	$(MAKE) clean-dependencies

clean:
	rm -f $(BIN)
	rm -f $(OBJS)

clean-dependencies:
	$(MAKE) clean-dependencies-build

clean-dependencies-build:
	rm -rf dependencies/json-c-build
	rm -rf dependencies/include
	rm -rf dependencies/lib

clean-player-info:
	rm -f data/player_info.json