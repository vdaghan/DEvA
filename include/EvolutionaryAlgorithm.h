#pragma once

#include "Genealogy.h"
#include "Individual.h"
#include "VariationFunctor.h"

#include <algorithm>
#include <iostream>
#include <list>
#include <memory>
#include <ranges>

namespace DEvA {
	enum class StepResult { Inconclusive, StepCount, Convergence };

	template <typename Types>
	class EvolutionaryAlgorithm {
		public:
			void setGenotypeFromProxyFunction(Types::FGenotypeFromProxy gfunc) { genotypeFromProxyFunxtion = gfunc; };
			void setPhenotypeFromProxyFunction(Types::FPhenotypeFromProxy gfunc) { phenotypeFromProxyFunxtion = gfunc; };

			void setGenesisFunction(Types::FGenesis gfunc) { genesisFunction = gfunc; };
			void setTransformFunction(Types::FTransform tfunc) { transformFunction = tfunc; };
			void setEvaluationFunction(Types::FEvaluate efunc) { evaluationFunction = efunc; };
			void addVariationFunctor(Types::SVariationFunctor func) { variationFunctors.push_back(func); };
			void setSurvivorSelectionFunction(Types::FSurvivorSelection ssfunc) { survivorSelectionFunction = ssfunc; };
			void setConvergenceCheckFunction(Types::FConvergenceCheck ccfunc) { convergenceCheckFunction = ccfunc; };

			void setOnEpochCallback(Types::COnEpoch oec) { onEpochCallback = oec; };

			StepResult epoch();
			StepResult search(size_t count);

			void addGeneration(Types::Generation gen) { genealogy.push_back(gen); };

			Types::GenotypeProxy bestGenotype;
			Types::PhenotypeProxy bestPhenotype;
			Types::Fitness bestFitness;
			Types::Genealogy genealogy;
		private:
			// Specialisation functions
			Types::FGenotypeFromProxy genotypeFromProxyFunxtion;
			Types::FPhenotypeFromProxy phenotypeFromProxyFunxtion;
			// EA functions
			Types::FGenesis genesisFunction;
			Types::FTransform transformFunction;
			Types::FEvaluate evaluationFunction;
			std::list<typename Types::SVariationFunctor> variationFunctors;
			Types::FSurvivorSelection survivorSelectionFunction;
			Types::FConvergenceCheck convergenceCheckFunction;

			Types::COnEpoch onEpochCallback;

	};
	template <typename Types>
	StepResult EvolutionaryAlgorithm<Types>::epoch() {
		typename Types::Generation newGeneration{};
		if (0 == genealogy.size()) [[unlikely]] {
			genealogy.push_back(genesisFunction());
			std::cout << "Genesis: " << genealogy.back().size() << " individuals.\n";
		} else [[likely]] {
			auto matingPool = genealogy.back();
			typename Types::IndividualPtrs newOffsprings{};
			for (auto it(variationFunctors.begin()); it != variationFunctors.end(); ++it) {
				auto & variationFunctor = *it;
				typename Types::SVariationInfo variationInfo = variationFunctor.apply(matingPool);
				typename Types::GenotypeProxies newGenotypes = variationInfo.children;
				for (auto it = newGenotypes.begin(); it != newGenotypes.end(); ++it) {
					typename Types::IndividualPtr newIndividual = std::make_shared<Individual<Types, Types::IndividualParameters>>(*it);
					newIndividual->setParents(variationInfo.parents);
					newOffsprings.emplace_back(newIndividual);
				}
			}
			typename Types::IndividualPtrs newGeneration{};
			newGeneration.insert(newGeneration.end(), newOffsprings.begin(), newOffsprings.end());
			newGeneration.insert(newGeneration.end(), genealogy.back().begin(), genealogy.back().end());
			genealogy.push_back(newGeneration);
		}
		auto & lastGen(genealogy.back());
		//std::cout << "Transforming...\n";
		std::for_each(lastGen.begin(), lastGen.end(), [&](auto & iptr) { iptr->phenotypeProxy = transformFunction(iptr->genotypeProxy); });
		//std::cout << "Evaluating...\n";
		std::for_each(lastGen.begin(), lastGen.end(), [&](auto & iptr) { iptr->fitness = evaluationFunction(iptr->phenotypeProxy); });
		//std::cout << "Sorting...\n";
		std::stable_sort(lastGen.begin(), lastGen.end(), [](auto iptr1, auto iptr2) -> bool { return ((iptr1->fitness) > (iptr2->fitness)); });

		// Survivor selection
		if (1 != genealogy.size()) [[likely]] {
			survivorSelectionFunction(genealogy.back());
		}
		auto bestIndividual = genealogy.back().back();
		bestGenotype = bestIndividual->genotypeProxy;
		bestPhenotype = bestIndividual->phenotypeProxy;
		bestFitness = bestIndividual->fitness;
		//std::cout << "Best fitness: " << bestFitness << "\n";

		if (convergenceCheckFunction(bestFitness)) [[unlikely]] {
			return StepResult::Convergence;
		}
		return StepResult::Inconclusive;
	}

	template <typename Types>
	StepResult EvolutionaryAlgorithm<Types>::search(size_t count) {
		for (size_t i(0); i < count; ++i) {
			StepResult epochResult = epoch();
			if (onEpochCallback) {
				onEpochCallback(genealogy.back());
			}
			if (StepResult::Inconclusive != epochResult) [[unlikely]] {
				return epochResult;
			}
		}
		return StepResult::StepCount;
	};
}
