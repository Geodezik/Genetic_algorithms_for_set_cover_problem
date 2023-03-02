all: GreedyAlgorithm

Genetic.o: Greedy.cpp Greedy.hpp
	$(CXX) -c $< $(CXXFLAGS)

BooleanMatrix.o: BooleanMatrix.cpp BooleanMatrix.hpp
	$(CXX) -c $< $(CXXFLAGS)

main.o: main.cpp BooleanMatrix.cpp BooleanMatrix.hpp Greedy.cpp Greedy.hpp
	$(CXX) -c $< $(CXXFLAGS)

GreedyAlgorithm: main.o BooleanMatrix.o Greedy.o
	$(CXX) -o $@ main.o BooleanMatrix.o Greedy.o
 
clean:
	rm *.o GreedyAlgorithm