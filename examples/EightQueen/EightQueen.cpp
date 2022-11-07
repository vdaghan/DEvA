#include <algorithm>
#include <array>
#include <iostream>
#include <list>
#include <memory>
#include <random>
#include <utility>
#include <vector>

#include "deva_version.h"
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

#include <functional>

int main() {
	std::cout << "DEvA version: " << getDEvAVersion() << std::endl;

	struct Specification {
		using Genotype = std::vector<size_t>;
		using GenotypeProxy = std::shared_ptr<Genotype>;
		using Phenotype = std::vector<size_t>;
		using PhenotypeProxy = std::shared_ptr<Phenotype>;
		using MetricVariant = std::variant<int>;
		using Distance = int;
		using IndividualParameters = DEvA::NullVParameters;
		static GenotypeProxy copy(GenotypeProxy gpx) { return gpx; };
	};

	using Spec = DEvA::Specialisation<Specification>;
	DEvA::EvolutionaryAlgorithm<Spec> ea;

	ea.genotypeFromProxyFunction = [](Spec::GenotypeProxy gpx) -> Spec::Genotype & { return *gpx; };
	ea.phenotypeFromProxyFunction = [](Spec::PhenotypeProxy ppx) -> Spec::Phenotype & { return *ppx; };

	Spec::FEvaluate fevaluate = [](Spec::PhenotypeProxy pptr) -> Spec::MetricVariantMap {
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
		Spec::MetricVariantMap retVal;
		retVal.emplace(std::make_pair("fitness", fitness));
		return retVal;
	};
	Spec::FVariationFromGenotypeProxies variation = [](Spec::GenotypeProxies gptrs) {
		Spec::GenotypeProxies offsprings = DEvA::StandardVariations<Spec>::cutAndCrossfill(gptrs);
		for (auto& gptr : offsprings) {
			double probability = DEvA::RandomNumberGenerator::get()->getDouble();
			if (probability <= 0.8) {
				gptr = DEvA::StandardVariations<Spec>::swap(gptr);
			}
		}
		return offsprings;
	};

	ea.registerEAFunction(DEvA::EAFunction::Initialisation, DEvA::StandardInitialisers<Spec>::permutations<8, 100>);
	ea.registerEAFunction(DEvA::EAFunction::Transformation, DEvA::StandardTransforms<Spec>::copy);
	ea.registerEAFunction(DEvA::EAFunction::Evaluation, fevaluate);
	ea.registerEAFunction(DEvA::EAFunction::FitnessComparison, [&](Spec::MetricVariantMap const& lhs, Spec::MetricVariantMap const& rhs) { return std::get<int>(lhs.at("fitness")) < std::get<int>(rhs.at("fitness")); });
	Spec::SVariationFunctor variationFunctor{
		.name = "cutAndCrossfillThenMaybeSwap",
		.numberOfParents = 2,
		.parentSelectionFunction = DEvA::StandardParentSelectors<Spec>::bestNofM<2, 5>,
		.variationFunctionFromGenotypeProxies = variation,
		.probability = 1.0,
		.removeParentsFromMatingPool = false
	};
	ea.registerVariationFunctor(variationFunctor, true);
	ea.registerEAFunction(DEvA::EAFunction::SurvivorSelection, DEvA::StandardSurvivorSelectors<Spec>::clamp<100>);
	ea.registerEAFunction(DEvA::EAFunction::ConvergenceCheck, [](Spec::MetricVariantMap const & metricMap) { return 0 == std::get<int>(metricMap.at("fitness")); });
	ea.lambda = 50;
	ea.logger.callback = [](DEvA::LogType t, std::string msg) {
		std::cout << msg << std::endl;
	};

	auto result = ea.search(1000);
	if (DEvA::StepResult::Convergence == result) {
		std::cout << "Converged.\n";
	//} else if (DEvA::StepResult::Stopped == result) {
	//	std::cout << "Stopped.\n";
	//	return 0;
	} else {
		std::cout << "Step limit reached.\n";
	}
	std::cout << "Best genotype: [";
	for (auto it = ea.bestGenotype->begin(); it != ea.bestGenotype->end(); ++it) {
		std::cout << *it << " ";
	}
	std::cout << "]\nFitness: " << std::get<int>(ea.bestIndividualMetric.at("fitness")) << "\n";

	return 0;
}