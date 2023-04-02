CC = g++
CFLAGS = -g
SOURCES = GameMaster.cpp Player.cpp Card.cpp Tile.cpp Deck.cpp

OBJECTS = $(SOURCES:%.cpp=build/%.o)

TESTSRC = moronic_text_ui.cpp test.cpp
TESTOBJ = $(TESTSRC:%.cpp=build/%.o)

all: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o build/game

test: $(OBJECTS) $(TESTOBJ)
	$(CC) $(CFLAGS) $(OBJECTS) $(TESTOBJ) -o build/test

build/%.o: %.cpp
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build

.PHONY: clean