all: GreedyAlgorithm

Genetic.o: Greedy.cpp Greedy.hpp
	g++ -O3 -c $< $(CXXFLAGS)

BooleanMatrix.o: BooleanMatrix.cpp BooleanMatrix.hpp
	g++ -O3 -c $< $(CXXFLAGS)

main.o: main.cpp BooleanMatrix.cpp BooleanMatrix.hpp Greedy.cpp Greedy.hpp
	g++ -O3 -c $< $(CXXFLAGS)

GreedyAlgorithm: main.o BooleanMatrix.o Greedy.o
	g++ -O3 -o $@ main.o BooleanMatrix.o Greedy.o
 
clean:
	rm *.o GreedyAlgorithm