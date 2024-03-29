// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include <algorithm>
#include <array>
#include <iostream>
#include <list>
#include <memory>
#include <random>
#include <utility>
#include <vector>

#include "DEvA/EvolutionaryAlgorithm.h"
#include "DEvA/Project/Filestore.h"
#include "DEvA/Specialisation.h"

/**
* Chapter 2.4.1, Eiben, 2009, Introduction to Evolutionary Computing
* 
* +---------------------------+--------------------------------------+
* | Representation            | Permutations                         |
* +---------------------------+--------------------------------------+
* | Recombination             | "Cut-and-crossfill" crossover        |
* +---------------------------+--------------------------------------+
* | Recombination Probability | 100%                                 |
* +---------------------------+--------------------------------------+
* | Mutation                  | Swap                                 |
* +---------------------------+--------------------------------------+
* | Mutation Probability      | 80%                                  |
* +---------------------------+--------------------------------------+
* | Parent Selection          | Best 2 out of random 5               |
* +---------------------------+--------------------------------------+
* | Survival Selection        | Replace worst                        |
* +---------------------------+--------------------------------------+
* | Population Size           | 100                                  |
* +---------------------------+--------------------------------------+
* | Number of Offspring       | 2                                    |
* +---------------------------+--------------------------------------+
* | Initialisation            | Random                               |
* +---------------------------+--------------------------------------+
* | Termination Condition     | Solution or 10000 fitness evaluation |
* +---------------------------+--------------------------------------+
**/

#include <functional>

struct Specification {
	using Genotype = std::vector<std::size_t>;
	using Phenotype = std::vector<std::size_t>;
};
using Spec = DEvA::Specialisation<Specification>;

int main() {
	DEvA::EvolutionaryAlgorithm<Spec> ea;
	ea.datastore = std::make_shared<DEvA::Filestore<Spec>>();

	Spec::BPVariationFromGenotypes::Function variation = [](Spec::Genotypes genotypes) {
		Spec::Genotypes offsprings = DEvA::StandardVariations<Spec>::cutAndCrossfill(std::move(genotypes));
		for (auto& genotype : offsprings) {
			double const probability = DEvA::RandomNumberGenerator::get()->getDouble();
			if (probability <= 0.8) {
				genotype = DEvA::StandardVariations<Spec>::swap(genotype);
			}
		}
		return offsprings;
	};
	ea.functions.variationFromGenotypes.definePlain("EightQueenVariation", variation);

	auto fevaluate = [](DEvA::ParameterMap pMap, Spec::IndividualPtr const & iptr) -> std::any {
		auto phenotype(iptr->maybePhenotype.value());
		auto const last = std::ranges::unique(phenotype).begin();
		phenotype.erase(last, phenotype.end());

		int fitness(0);
		int x1(0);
		for (auto it1 = phenotype.begin(); it1 != phenotype.end(); ++it1, ++x1) {
			int y1(*it1);
			int x2(0);
			for (auto it2 = phenotype.begin(); it2 != phenotype.end(); ++it2, ++x2) {
				int y2(*it2);
				if (x1 >= x2) {
					continue;
				}
				int const diffx(std::max(x1, x2) - std::min(x1, x2));
				int const diffy(std::max(y1, y2) - std::min(y1, y2));
				if (diffx == diffy) {
					++fitness;
				}
			}
		}
		return fitness;
	};
	ea.metricFunctors.computeFromIndividualPtrFunctions.emplace(std::pair("fitnessEvaluation", fevaluate));
	ea.importSetup("./EASetup.json");
	ea.compile();

	ea.lambda = 50;

	auto const result = ea.search(1000);

	std::cout << "Best genotype: [";
	auto const & bestGenotype(ea.bestIndividual->genotype);
	for (std::size_t const it : bestGenotype) {
		std::cout << it << " ";
	}
	std::cout << "]" << std::endl;
	for (std::size_t row(0); row != bestGenotype.size(); ++row) {
		for (std::size_t col(0); col != bestGenotype.size(); ++col) {
			if (col == bestGenotype.at(row)) {
				std::cout << "O ";
			} else {
				std::cout << ". ";
			}
		}
		std::cout << std::endl;
	}
	auto const & bestIndividualMetricMap(ea.bestIndividual->metricMap);
	auto const & bestIndividualFitness(bestIndividualMetricMap.at("fitness"));
	std::cout << "Fitness: " << bestIndividualFitness.as<int>() << "\n";

	return 0;
}