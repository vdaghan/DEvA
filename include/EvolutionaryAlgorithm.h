#pragma once

#include "Error.h"
#include "Genealogy.h"
#include "Individual.h"
#include "VariationFunctor.h"

#include "BuildingBlocks/StandardGenePoolSelectors.h"

#include <algorithm>
#include <iostream>
#include <list>
#include <memory>
#include <ranges>

namespace DEvA {
	enum class StepResult { Inconclusive, StepCount, Exhaustion, Convergence };

	template <typename Types>
	class EvolutionaryAlgorithm {
		public:
			EvolutionaryAlgorithm();
			void setGenotypeFromProxyFunction(Types::FGenotypeFromProxy gfunc) { genotypeFromProxyFunxtion = gfunc; };
			void setPhenotypeFromProxyFunction(Types::FPhenotypeFromProxy gfunc) { phenotypeFromProxyFunxtion = gfunc; };

			void setGenesisFunction(Types::FGenesis gfunc) { genesisFunction = gfunc; };
			void setGenePoolSelectionFunction(Types::FGenePoolSelection gfunc) { genePoolSelectionFunction = gfunc; };
			void setTransformFunction(Types::FTransform tfunc) { transformFunction = tfunc; };
			void setEvaluationFunction(Types::FEvaluate efunc) { evaluationFunction = efunc; };
			void setFitnessComparisonFunction(Types::FFitnessComparison cfunc) { fitnessComparisonFunction = cfunc; };
			void addVariationFunctor(Types::SVariationFunctor func) { variationFunctors.push_back(func); };
			void setSurvivorSelectionFunction(Types::FSurvivorSelection ssfunc) { survivorSelectionFunction = ssfunc; };
			void setConvergenceCheckFunction(Types::FConvergenceCheck ccfunc) { convergenceCheckFunction = ccfunc; };

			void setOnEpochStartCallback(Types::COnEpoch c) { onEpochStartCallback = c; };
			void setOnEpochEndCallback(Types::COnEpoch c) { onEpochEndCallback = c; };

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
			Types::FGenePoolSelection genePoolSelectionFunction;
			Types::FTransform transformFunction;
			Types::FEvaluate evaluationFunction;
			Types::FFitnessComparison fitnessComparisonFunction;
			std::list<typename Types::SVariationFunctor> variationFunctors;
			Types::FSurvivorSelection survivorSelectionFunction;
			Types::FConvergenceCheck convergenceCheckFunction;
			// Callbacks
			template <typename F, typename ... VTypes> void tryExecuteCallback(F f, VTypes ... vargs) { if(f) f(vargs...); };
			Types::COnEpoch onEpochStartCallback;
			Types::COnEpoch onEpochEndCallback;

	};
	template <typename Types>
	EvolutionaryAlgorithm<Types>::EvolutionaryAlgorithm() {
		setGenePoolSelectionFunction(StandardGenePoolSelectors<Types>::all);
	}
	template <typename Types>
	StepResult EvolutionaryAlgorithm<Types>::epoch() {
		typename Types::Generation newGeneration{};
		if (0 == genealogy.size()) [[unlikely]] {
			genealogy.push_back(genesisFunction());
			std::cout << "Genesis: " << genealogy.back().size() << " individuals.\n";
		} else [[likely]] {
			auto genePool = genePoolSelectionFunction(genealogy.back());
			typename Types::IndividualPtrs newOffsprings{};
			for (auto const & variationFunctor: variationFunctors) {
				auto variationInfo = variationFunctor.apply(genePool);
				auto newGenotypes = variationInfo.children;
				for (auto const & newGenotype : newGenotypes) {
					auto newIndividual = std::make_shared<Individual<Types, Types::IndividualParameters>>(newGenotype);
					newIndividual->setParents(variationInfo.parents);
					newOffsprings.emplace_back(newIndividual);
				}
			}
			typename Types::IndividualPtrs newGeneration{};
			newGeneration.insert(newGeneration.end(), newOffsprings.begin(), newOffsprings.end());
			newGeneration.insert(newGeneration.end(), genealogy.back().begin(), genealogy.back().end());
			genealogy.push_back(newGeneration);
		}

		auto processIndividual = [this](Types::IndividualPtr iptr) {
			iptr->maybePhenotypeProxy = transformFunction(iptr->genotypeProxy);
			if (std::unexpected(ErrorCode::InvalidTransform) == iptr->maybePhenotypeProxy) {
				return;
			}
			iptr->fitness = evaluationFunction(iptr->maybePhenotypeProxy.value());
		};
		std::for_each(genealogy.back().begin(), genealogy.back().end(), [&](auto & iptr) { processIndividual(iptr); });

		auto fitter = [this](Types::IndividualPtr iptr1, Types::IndividualPtr iptr2) -> bool {
			return fitnessComparisonFunction(iptr1->fitness, iptr2->fitness);
		};
		std::stable_sort(genealogy.back().begin(), genealogy.back().end(), fitter);

		//if (1 != genealogy.size()) [[likely]] {
			survivorSelectionFunction(genealogy.back());
		//}
		auto bestIndividual = genealogy.back().front();
		bestGenotype = bestIndividual->genotypeProxy;
		bestPhenotype = bestIndividual->maybePhenotypeProxy.value();
		bestFitness = bestIndividual->fitness;
		//std::cout << "Best fitness: " << bestFitness << "\n";

		if (genealogy.back().empty()) [[unlikely]] {
			return StepResult::Exhaustion;
		}
		if (convergenceCheckFunction(bestFitness)) [[unlikely]] {
			return StepResult::Convergence;
		}
		return StepResult::Inconclusive;
	}

	template <typename Types>
	StepResult EvolutionaryAlgorithm<Types>::search(size_t count) {
		for (size_t i(0); i < count; ++i) {
			tryExecuteCallback<Types::COnEpoch, std::size_t>(onEpochStartCallback, genealogy.size());
			StepResult epochResult = epoch();
			tryExecuteCallback<Types::COnEpoch, std::size_t>(onEpochEndCallback, genealogy.size() - 1);
			if (StepResult::Inconclusive != epochResult) [[unlikely]] {
				return epochResult;
			}
		}
		return StepResult::StepCount;
	};
}
