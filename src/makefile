all: CorrectEncoding.o

BCGA.o: BCGA.cpp BCGA.hpp
	g++ -O3 -c $< $(CXXFLAGS)

Greedy.o: Greedy.cpp Greedy.hpp
	g++ -O3 -c $< $(CXXFLAGS)

BooleanMatrix.o: BooleanMatrix.cpp BooleanMatrix.hpp
	g++ -O3 -c $< $(CXXFLAGS)

main.o: main.cpp BooleanMatrix.cpp BooleanMatrix.hpp BCGA.cpp BCGA.hpp Greedy.cpp Greedy.hpp
	g++ -O3 -c $< $(CXXFLAGS)

CorrectEncoding.o: main.o BooleanMatrix.o BCGA.o Greedy.o
	g++ -O3 -o $@ main.o BooleanMatrix.o BCGA.o Greedy.o
 
clean:
	rm *.o
