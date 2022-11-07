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
#include <set>

namespace DEvA {
	enum class StepResult { Inconclusive, StepCount, Exhaustion, Convergence, Stopped };

	enum class EAFunction { Initialisation, GenePoolSelection, Transformation, Evaluation, FitnessComparison, DistanceCalculation, SurvivorSelection, ConvergenceCheck };
	enum class Callback { StatsUpdate, EpochStart, EpochEnd, Variation, Pause, Stop };

	template <typename Types>
	class EvolutionaryAlgorithm {
		public:
			EvolutionaryAlgorithm();

			// Targeted number of individuals
			std::size_t lambda;

			// Specialisation functions
			Types::FGenotypeFromProxy genotypeFromProxyFunction;
			Types::FPhenotypeFromProxy phenotypeFromProxyFunction;

			// Callbacks
			Types::CEAStatsUpdate onEAStatsUpdateCallback;
			Types::COnEpoch onEpochStartCallback;
			Types::COnEpoch onEpochEndCallback;
			Types::COnVariation onVariationCallback;
			Types::CVoid onPauseCallback;
			Types::CVoid onStopCallback;

			void registerEAFunction(EAFunction functionType, Types::FVariant function) {
				eaFunctions.emplace(std::make_pair(functionType, function));
			}
			void registerVariationFunctor(typename Types::SVariationFunctor variationFunctor, bool use = false) {
				registeredVariationFunctors[variationFunctor.name] = variationFunctor;
				if (use) {
					useVariationFunctor(variationFunctor.name);
				}
			}
			void useVariationFunctor(std::string vfName) {
				variationFunctorsInUse.emplace(vfName);
			}
			void registerCallback(Callback callbackType, Types::CVariant function) {
				callbacks.emplace(std::make_pair(callbackType, function));
			}

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
			Types::MetricVariantMap bestIndividualMetric;
			Types::Genealogy genealogy;
			std::deque<typename Types::MetricVariantMap> generationMetrics;
			Types::MetricVariantMap genealogyMetrics;
		private:
			EAState eaState;
			StepResult epoch();
			Types::FVariantMap eaFunctions;
			Types::CVariantMap callbacks;
			std::map<std::string, typename Types::SVariationFunctor> registeredVariationFunctors;
			std::set<std::string> variationFunctorsInUse;

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
			template <typename ... VTypes>
				void tryExecuteCallback(Callback callbackType, VTypes ... vargs) {
					if (callbacks.contains(callbackType)) {
						std::lock_guard<std::mutex> lock(callbackMutex);
						callbacks.at(callbackType)(vargs...);
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
