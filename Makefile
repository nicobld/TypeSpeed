CC=gcc

CFLAGS=-O3 -std=gnu99 -Wall -Wextra 
LDLIBS=-lpthread -ljson-c

OBJS=$(subst .c,.o,$(shell find ./src -name "*.c"))
BIN=typespeed

all: $(BIN)

$(BIN) : $(OBJS)
	$(CC) $(OBJS) -o $@ $(CPPFLAGS) $(CFLAGS) $(LDLIBS) $(LDFLAGS)

%.o : %.c
	$(CC) -c $< -o $@ $(CPPFLAGS) $(CFLAGS) $(LDLIBS) $(LDFLAGS)

clean:
	rm -f $(BIN)
	rm -f $(OBJS)

clean-player-info:
	rm -f data/player_info.json
