CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror -pedantic
SFML_LIBS = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

all: Sokoban Sokoban.a test

Sokoban: main.o Sokoban.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(SFML_LIBS)

Sokoban.a: Sokoban.o
	ar rcs $@ $^

test: test.o Sokoban.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(SFML_LIBS)

main.o: main.cpp Sokoban.hpp
	$(CXX) $(CXXFLAGS) -c $<

Sokoban.o: Sokoban.cpp Sokoban.hpp
	$(CXX) $(CXXFLAGS) -c $<

test.o: test.cpp Sokoban.hpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o Sokoban Sokoban.a test

lint:
	cpplint --filter=-legal/copyright *.cpp *.hpp

run_tests: test
	./test

.PHONY: all clean lint run_tests