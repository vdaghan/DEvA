#pragma once

#include "EAStatistics.h"
#include "Error.h"
#include "Genealogy.h"
#include "Individual.h"
#include "Logger.h"
#include "VariationFunctor.h"

#include "BuildingBlocks/StandardGenePoolSelectors.h"

#include <algorithm>
#include <deque>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <ranges>

namespace DEvA {
	enum class StepResult { Inconclusive, StepCount, Exhaustion, Convergence };

	template <typename Types>
	class EvolutionaryAlgorithm {
		public:
			EvolutionaryAlgorithm();

			std::size_t lambda;

			// Specialisation functions
			Types::FGenotypeFromProxy genotypeFromProxyFunction;
			Types::FPhenotypeFromProxy phenotypeFromProxyFunction;

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
			Types::CEAStatsUpdate onEAStatsUpdateCallback;
			Types::COnEpoch onEpochStartCallback;
			Types::COnEpoch onEpochEndCallback;
			Types::COnVariation onVariationCallback;

			StepResult epoch();
			StepResult search(size_t count);
			Types::IndividualPtr find(IndividualIdentifier);

			void addGeneration(Types::Generation gen) { genealogy.push_back(gen); };

			Logger logger;
			Types::GenotypeProxy bestGenotype;
			Types::PhenotypeProxy bestPhenotype;
			Types::Fitness bestFitness;
			Types::Genealogy genealogy;
		private:
			template <typename F, typename ... VTypes> void tryExecuteCallback(F f, VTypes ... vargs) { if(f) f(vargs...); };
			VariationStatisticsMap evaluateVariations();
			std::deque<std::list<VariationInfo<Types>>> variationInfos;
			EAStatistics eaStatistics;
	};
}

#include "EvolutionaryAlgorithm.hpp"
