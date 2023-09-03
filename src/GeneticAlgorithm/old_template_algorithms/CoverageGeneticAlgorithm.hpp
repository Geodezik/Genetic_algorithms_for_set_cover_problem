class Genetic::CoverageGeneticAlgorithm: public Genetic::BaseGeneticAlgorithm {
public:
    CoverageGeneticAlgorithm(int population_size, int extended_population_size, double mutation_proba, int max_iter = 100);

    void create_zero_generation(BooleanMatrix::BooleanMatrix& M, int genotype_len);
    void get_parent_indices(int& p1, int& p2);
    Individual crossover(Individual& parent1, Individual& parent2);
    void mutate(BooleanMatrix::BooleanMatrix& M, double mutation_proba, int parameter);
    double fitness(BooleanMatrix::BooleanMatrix& M, Individual& individual);
    void selection(int iteration, int verbose);
};
