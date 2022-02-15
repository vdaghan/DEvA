#include <vector>

#include "EvolutionaryAlgorithm.h"
#include "Specialisation.h"

/**
* Chapter 2.4.1, Eiben, 2009, Introduction to Evolutionar Computing
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

#include <array>
#include <list>
#include <memory>
#include <random>
#include <utility>
#include <vector>

int main() {
	using Genotype = std::vector<size_t>;
	using Phenotype = std::vector<size_t>;
	using Fitness = size_t;
	using Spec = DEvA::Specialisation<Genotype, Phenotype, Fitness>;
	Spec::SEvolutionaryAlgorithm ea;
	//DEvA::EvolutionaryAlgorithm<Spec::BT> ea;

	// Initialise
	std::default_random_engine randGen;
	std::uniform_int_distribution<int> distribution(1, 8);
	auto createRandomGenotype = [&]() -> Spec::GenotypePtr {
		Spec::GenotypePtr gptr = std::make_shared<Spec::Genotype>();
		for (size_t i = 0; i < 8; ++i) {
			gptr->emplace_back(i) = distribution(randGen);
		}
		return gptr;
	};
	Spec::FGenesis initialise = [&]() -> Spec::Generation {
		Spec::Generation generation;
		for (size_t i = 0; i < 100; ++i) {
			Spec::GenotypePtr gptr = createRandomGenotype();
			generation.emplace(std::make_shared<Spec::SIndividual>(gptr));
		}
		return generation;
	};
	ea.genesis(initialise);

	/*auto extractNRandomElements = [](Spec::GenotypePtrSet gptrs, size_t N) -> Spec::FSlicerReturn {
		std::default_random_engine randGen;
		Spec::GenotypePtrSet retSet;
		for (size_t i = 0; i < N; ++i) {
			std::uniform_int_distribution<int> distribution(0, gptrs.size() - 1);
			size_t randomIndex = distribution(randGen);
			auto it = gptrs.begin();
			for (size_t j = 0; j < randomIndex; ++j) {
				++it;
			}
			retSet.push_back(*it);
			gptrs.erase(it);
		}
		return Spec::FSlicerReturn(retList, gptrs);
	};
	auto slicer = [&](Spec::GenotypePtrList orig, size_t N) -> Spec::GenotypePtrListList {
		Spec::GenotypePtrListList gptrlistlist;
		while (orig.size() >= N) {
			Spec::FSlicerReturn slicing = extractNRandomElements(orig, 5);
			gptrlistlist.push_back(slicing.first);
			orig = slicing.second;
		}
		return gptrlistlist;
	};
	auto sslicer = [&](Spec::GenotypePtrList orig) {
		return slicer(orig, 5);
	};
	ea.slicer = sslicer;*/

	return 0;
}