Genetic::CoverageGeneticAlgorithm::CoverageGeneticAlgorithm(int population_size, int extended_population_size, double mutation_proba,
                                                            int max_iter): Genetic::BaseGeneticAlgorithm(population_size, extended_population_size,
                                                                                                         mutation_proba, max_iter) {}

void Genetic::CoverageGeneticAlgorithm::create_zero_generation(BooleanMatrix::BooleanMatrix& M, int genotype_len)
{
    //creating zero generation with some genes
    for(int i = 0; i < population_size; i++) {
        std::vector<bool> genotype;
        for(int j = 0; j < genotype_len; j++) {
            genotype.push_back(true);
        }
        population.push_back(Individual(genotype, true));
    };
}

void Genetic::CoverageGeneticAlgorithm::get_parent_indices(int& p1, int& p2)
{
    std::uniform_int_distribution<> parents_d(0, population_size - 1);
    do {
        p1 = parents_d(rng);
        p2 = parents_d(rng);
    } while (p1 == p2);
}

Genetic::Individual Genetic::CoverageGeneticAlgorithm::crossover(Individual& parent1, Individual& parent2)
{
    // pick a random point (but not at the end of a chromosomes), then
    // join two parts from different parents
    std::vector<bool> new_genotype;
    int length = parent1.genotype.size();
    std::uniform_int_distribution<> d(1, length - 1);
    int point = d(rng);

    for(int i = 0; i < point; i++) {
        new_genotype.push_back(parent1.genotype[i]);
    }

    for(int i = point; i < length; i++) {
        new_genotype.push_back(parent2.genotype[i]);
    }

    return Individual(new_genotype);
}

void Genetic::CoverageGeneticAlgorithm::mutate(BooleanMatrix::BooleanMatrix& M, double mutation_proba, int parameter)
{
    parameter = (n / 10) * (max_iter - parameter) / max_iter;
    std::bernoulli_distribution bernoulli_d(mutation_proba);
    int genotype_len = population[0].size();
    for(int mut_iter = 0; mut_iter < parameter; mut_iter++) {
        std::uniform_int_distribution<> genes_d(0, genotype_len - 1);
        for(int j = population_size + 1; j < extended_population_size; j++) {
            bool mutate = bernoulli_d(rng);
            if(!mutate)
                continue;

            int random_gen = genes_d(rng);
            population[j].genotype[random_gen] = !population[j].genotype[random_gen];
        }
    }
}

double Genetic::CoverageGeneticAlgorithm::fitness(BooleanMatrix::BooleanMatrix& M, Genetic::Individual& individual)
{
    if(individual.is_from_zero_gen())
        return n;

    if(!M.is_covered_by(individual.genotype))
        return n + 1;

    // cheap???
    int ones_counter = 0;
    for(int i = 0; i < individual.size(); i++) {
        if(individual.genotype[i])
            ones_counter++;
    }

    return ones_counter;
}

void Genetic::CoverageGeneticAlgorithm::selection(int iteration, int verbose)
{
    //Take best
    std::vector<int> scores_copy = scores;
    std::vector<int> argbest = argsort(scores);
    print_stats(argbest, iteration, verbose);
    std::vector<Individual> best;

    // include coverings only
    for(int j = 0; j < extended_population_size; j++) {
        if(best.size() == population_size)
            break;
        if(scores[j] > n)
            continue;
        best.push_back(population[argbest[j]]);
    }

    population = best;
}
