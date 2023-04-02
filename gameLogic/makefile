CC = g++
CFLAGS = -g
SOURCES = GameMaster.cpp Player.cpp Card.cpp Tile.cpp Deck.cpp

OBJECTS = $(SOURCES:%.cpp=build/%.o)

TESTSRC = Terminal_UI.cpp Test_main.cpp
TESTOBJ = $(TESTSRC:%.cpp=build/%.o)

all: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o build/game

test: $(OBJECTS) $(TESTOBJ)
	$(CC) $(CFLAGS) $(OBJECTS) $(TESTOBJ) -o build/test
	build/test

build/%.o: %.cpp
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

gdb: $(OBJECTS) $(TESTOBJ)
	$(CC) -g $(CFLAGS) $(OBJECTS) $(TESTOBJ) -o build/test
	gdb -tui build/test

clean:
	rm -rf build

.PHONY: clean 