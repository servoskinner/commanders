CC = g++
CFLAGS = -g -Iinclude
SOURCES = $(addprefix source/, GameMaster.cpp Player.cpp Card.cpp Tile.cpp Deck.cpp Roster.cpp CardAbilities.cpp)

OBJECTS = $(patsubst source/%.cpp,build/%.o,$(SOURCES))

TESTSRC = $(addprefix source/, TerminalControl.cpp Test_main.cpp)
TESTOBJ = $(patsubst source/%.cpp,build/%.o,$(TESTSRC))

all: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o build/game

test: $(OBJECTS) $(TESTOBJ)
	$(CC) $(CFLAGS) $(OBJECTS) $(TESTOBJ) -o build/test
	build/test

build/%.o: source/%.cpp
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

gdb: $(OBJECTS) $(TESTOBJ)
	$(CC) -g $(CFLAGS) $(OBJECTS) $(TESTOBJ) -o build/test
	gdb -tui build/test

clean:
	rm -rf build

.PHONY: clean