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
	ea.setGenesisFunction(initialise);

	Spec::FTransform ftransform = [](Spec::GenotypePtr gptr) -> Spec::PhenotypePtr {
		return gptr;
	};
	ea.setTransformFunction(ftransform);

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
				if (y1 == y2) {
					continue;
				}
				int diffx(x1 - x2);
				int diffy(y1 - y2);
				if (0 != std::abs(std::abs(diffx) - std::abs(diffy))) {
					++fitness;
				}
			}
		}
		return fitness;
	};
	ea.setEvaluationFunction(fevaluate);

	auto extractNRandomElements = [](Spec::GenotypePtrSet domain, size_t N) -> Spec::RFGenotypePtrSet {
		std::default_random_engine randGen;
		Spec::GenotypePtrSet rest(domain);
		Spec::GenotypePtrSet preimage;
		for (size_t i = 0; i < N; ++i) {
			std::uniform_int_distribution<int> distribution(0, rest.size() - 1);
			size_t randomIndex = distribution(randGen);
			auto it = rest.begin();
			for (size_t j = 0; j < randomIndex; ++j) {
				++it;
			}
			preimage.emplace(*it);
			rest.erase(it);
		}
		return Spec::RFGenotypePtrSet({ .domain = domain,.preimage = preimage, .rest = rest });
	};
	/*
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