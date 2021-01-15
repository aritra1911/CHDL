CXXFLAGS=-Wall -Wextra -pedantic -std=c++17

main: main.o chips.o
	$(CXX) main.o chips.o -o main

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

chips.o: chips.cpp
	$(CXX) $(CXXFLAGS) -c chips.cpp

clean:
	$(RM) *.o main
