#pragma once

#include "EAState.h"
#include "EAStatistics.h"
#include "Error.h"
#include "Genealogy.h"
#include "Individual.h"
#include "IndividualIdentifier.h"
#include "Logger.h"
#include "VariationFunctor.h"

#include "BuildingBlocks/StandardGenePoolSelectors.h"

#include <algorithm>
#include <atomic>
#include <deque>
#include <execution>
#include <future>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <ranges>

namespace DEvA {
	enum class StepResult { Inconclusive, StepCount, Exhaustion, Convergence, Stopped };

	template <typename Types>
	class EvolutionaryAlgorithm {
		public:
			EvolutionaryAlgorithm();

			// Targeted number of individuals
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
			Types::FDistanceCalculation distanceCalculationFunction;
			Types::FSurvivorSelection survivorSelectionFunction;
			Types::FConvergenceCheck convergenceCheckFunction;

			// Callbacks
			Types::CEAStatsUpdate onEAStatsUpdateCallback;
			Types::COnEpoch onEpochStartCallback;
			Types::COnEpoch onEpochEndCallback;
			Types::COnVariation onVariationCallback;
			Types::CVoid onPauseCallback;
			Types::CVoid onStopCallback;

			StepResult search(size_t count);
			void pause();
			void stop();
			Types::IndividualPtr createNewIndividual(Types::GenotypeProxy);
			IndividualIdentifier reserveNewIndividualIdentifier();
			Types::IndividualPtr find(IndividualIdentifier);

			void addGeneration(Types::Generation gen) { genealogy.push_back(gen); };
			Logger logger;
			Types::GenotypeProxy bestGenotype;
			Types::PhenotypeProxy bestPhenotype;
			Types::Fitness bestFitness;
			Types::Genealogy genealogy;
		private:
			EAState eaState;
			StepResult epoch();
			void transformIndividuals(Types::Generation &);
			void removeInvalidIndividuals(Types::Generation &);
			void evaluateIndividuals(Types::Generation &);
			void mergeGenerations(Types::Generation &, Types::Generation &);
			void sortGeneration(Types::Generation &);
			void computeDistances(Types::Generation &);
			void evaluateVariations();
			std::mutex callbackMutex;
			template <typename F, typename ... VTypes>
				void tryExecuteCallback(F f, VTypes ... vargs) {
					if (f) {
						std::lock_guard<std::mutex> lock(callbackMutex);
						f(vargs...);
					}
				};
			std::deque<std::list<VariationInfo<Types>>> variationInfos;
			EAStatistics<Types> eaStatistics;
			std::mutex eaStatisticsMutex;

			std::atomic<bool> pauseFlag;
			bool checkStopFlagAndMaybeWait();
			std::atomic<bool> stopFlag;
	};
}

#include "EvolutionaryAlgorithm.hpp"
