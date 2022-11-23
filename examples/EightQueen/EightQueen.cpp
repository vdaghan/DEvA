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

#include "deva_version.h"
#include "DEvA/EvolutionaryAlgorithm.h"
#include "DEvA/Filestore.h"
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
	using Genotype = std::vector<size_t>;
	using GenotypeProxy = std::shared_ptr<Genotype>;
	using Phenotype = std::vector<size_t>;
	using PhenotypeProxy = std::shared_ptr<Phenotype>;
	static GenotypeProxy copy(GenotypeProxy gpx) { return gpx; };
};
using Spec = DEvA::Specialisation<Specification>;

int main() {
	std::cout << "DEvA version: " << getDEvAVersion() << std::endl;
	
	DEvA::EvolutionaryAlgorithm<Spec> ea;
	ea.datastore = std::make_shared<DEvA::Filestore<Spec>>();

	ea.genotypeFromProxyFunction = [](Spec::GenotypeProxy gpx) -> Spec::Genotype & { return *gpx; };
	ea.phenotypeFromProxyFunction = [](Spec::PhenotypeProxy ppx) -> Spec::Phenotype & { return *ppx; };

	auto fevaluate = [](Spec::IndividualPtr iptr) -> std::any {
		auto const & pptr(iptr->maybePhenotypeProxy.value());
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
	auto fitnessComparison = [](std::any const & lhs, std::any const & rhs) {
		return std::any_cast<int>(lhs) < std::any_cast<int>(rhs);
	};
	auto fitnessToStringFunction = [](std::any const& v) {
		return std::format("{}", std::any_cast<int>(v));
	};
	DEvA::MetricFunctor<Spec> metricFunctor{
		.name = "fitness",
		.computeFromIndividualPtrFunction = fevaluate,
		.betterThanFunction = fitnessComparison
	};
	metricFunctor.constructDefaultJSONConverter<int>();
	ea.registerMetricFunctor(metricFunctor, true);

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
	Spec::SVariationFunctor variationFunctor{
		.name = "cutAndCrossfillThenMaybeSwap",
		.numberOfParents = 2,
		.parentSelectionFunction = std::bind_front(DEvA::StandardParentSelectors<Spec>::bestNofM<2, 5>, "fitness"),
		.variationFunctionFromGenotypeProxies = variation,
		.probability = 1.0,
		.removeParentsFromMatingPool = false
	};
	ea.registerVariationFunctor(variationFunctor, true);

	ea.registerEAFunction(DEvA::EAFunction::Initialisation, DEvA::StandardInitialisers<Spec>::permutations<8, 100>);
	ea.registerEAFunction(DEvA::EAFunction::Transformation, DEvA::StandardTransforms<Spec>::copy);
	ea.registerEAFunction(DEvA::EAFunction::SortIndividuals, [](Spec::IndividualPtr const & lhs, Spec::IndividualPtr const & rhs) {
		return lhs->metricMap.at("fitness") < rhs->metricMap.at("fitness");
	});
	ea.registerEAFunction(DEvA::EAFunction::SurvivorSelection, DEvA::StandardSurvivorSelectors<Spec>::clamp<100>);
	ea.registerEAFunction(DEvA::EAFunction::ConvergenceCheck, [](Spec::SMetricMap const & metricMap) { return 0 == metricMap.at("fitness").as<int>(); });
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
	auto & bestGenotype(ea.bestIndividual->genotypeProxy);
	for (auto it = bestGenotype->begin(); it != bestGenotype->end(); ++it) {
		std::cout << *it << " ";
	}
	std::cout << "]" << std::endl;
	for (std::size_t row(0); row != bestGenotype->size(); ++row) {
		for (std::size_t col(0); col != bestGenotype->size(); ++col) {
			if (col == bestGenotype->at(row)) {
				std::cout << "O ";
			} else {
				std::cout << ". ";
			}
		}
		std::cout << std::endl;
	}
	auto & bestIndividualMetricMap(ea.bestIndividual->metricMap);
	auto & bestIndividualFitness(bestIndividualMetricMap.at("fitness"));
	std::cout << "Fitness: " << bestIndividualFitness.as<int>() << "\n";

	return 0;
}