#include <algorithm>
#include <array>
#include <list>
#include <memory>
#include <random>
#include <utility>
#include <vector>

#include "EvolutionaryAlgorithm.h"
#include "Specialisation.h"

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

int main() {
	using Genotype = std::vector<size_t>;
	using Phenotype = std::vector<size_t>;
	using Fitness = int;

	using Spec = DEvA::Specialisation<Genotype, Phenotype, Fitness, DEvA::NullVParameters>;
	DEvA::EvolutionaryAlgorithm<Spec> ea;

	Spec::FEvaluate fevaluate = [](Spec::PhenotypePtr pptr) -> Spec::Fitness {
		auto last = std::unique(pptr->begin(), pptr->end());
		pptr->erase(last, pptr->end());

		int fitness(0);
		int x1(0);
		for (auto it1 = pptr->begin(); it1 != pptr->end(); ++it1, ++x1) {
			int y1(*it1);
			int x2(0);
			for (auto it2 = pptr->begin(); it2 != pptr->end(); ++it2, ++x2) {
				int y2(*it2);
				if (x1 >= x2) {
					continue;
				}
				int diffx(x1 - x2);
				int diffy(y1 - y2);
				if (0 == std::abs(std::abs(diffx) - std::abs(diffy))) {
					++fitness;
				}
			}
		}
		return fitness;
	};
	Spec::FVariation variation = [](Spec::GenotypePtrs gptrs) {
		Spec::GenotypePtrs offsprings = DEvA::StandardVariations<Spec>::cutAndCrossfill(gptrs);
		for (auto& gptr : offsprings) {
			std::default_random_engine randGen;
			std::uniform_int_distribution<int> distribution(0, 100);
			size_t swapProbability = distribution(randGen);
			if (swapProbability <= 80) {
				gptr = DEvA::StandardVariations<Spec>::swap(gptr);
			}
		}
		return offsprings;
	};

	ea.setGenesisFunction(DEvA::StandardInitialisers<Spec>::permutations<8, 100>);
	ea.setTransformFunction(DEvA::StandardTransforms<Spec>::copy);
	ea.setEvaluationFunction(fevaluate);
	ea.setParentSelectionFunction(DEvA::StandardParentSelectors<Spec>::bestNofM<2, 5>);
	ea.setVariationFunction(variation);
	ea.setSurvivorSelectionFunction(DEvA::StandardSurvivorSelectors<Spec>::clamp<100>);
	ea.setConvergenceCheckFunction(DEvA::StandardConvergenceCheckers<Spec>::equalTo<0>);

	auto result = ea.search(10000);
	std::cout << "Best genotype: [";
	for (auto it = ea.bestGenotype->begin(); it != ea.bestGenotype->end(); ++it) {
		std::cout << *it << " ";
	}
	std::cout << "]\nFitness: " << ea.bestFitness << "\n";
	if (DEvA::StepResult::Convergence == result) {
		std::cout << "Converged.\n";
	} else {
		std::cout << "Step limit reached.\n";
	}

	return 0;
}