#pragma once

#include "DEvA/Datastore.h"
#include "DEvA/EAState.h"
#include "DEvA/EAStatistics.h"
#include "DEvA/Error.h"
#include "DEvA/Individual.h"
#include "DEvA/IndividualIdentifier.h"
#include "DEvA/Logger.h"
#include "DEvA/Metric.h"
#include "DEvA/VariationFunctor.h"

#include <algorithm>
#include <atomic>
#include <deque>
#include <execution>
#include <future>
#include <list>
#include <map>
#include <memory>
#include <variant>
#include <set>

namespace DEvA {
	enum class StepResult { Inconclusive, StepCount, Exhaustion, Convergence, Stopped };

	enum class EAFunction {
		Initialisation,
		GenePoolSelection,
		Transformation,
		SurvivorSelection,
		SortIndividuals,
		ConvergenceCheck
	};
	enum class Callback { StatsUpdate, EpochStart, EpochEnd, Variation, Evaluation, Pause, Stop };

	template <typename Types>
	class EvolutionaryAlgorithm {
		public:
			EvolutionaryAlgorithm();

			// Targeted number of individuals
			std::size_t lambda;

			// Callbacks
			typename Types::CEAStatsUpdate onEAStatsUpdateCallback;
			typename Types::COnEpoch onEpochStartCallback;
			typename Types::COnEpoch onEpochEndCallback;
			typename Types::COnVariation onVariationCallback;
			typename Types::CVoid onPauseCallback;
			typename Types::CVoid onStopCallback;

			void registerEAFunction(EAFunction functionType, typename Types::FVariant function) {
				eaFunctions.emplace(std::make_pair(functionType, function));
			}
			void registerVariationFunctor(VariationFunctor<Types> variationFunctor, bool use = false) {
				registeredVariationFunctors[variationFunctor.name] = variationFunctor;
				if (use) {
					useVariationFunctor(variationFunctor.name);
				}
			}
			void useVariationFunctor(std::string vfName) {
				variationFunctorsInUse.emplace(vfName);
			}
			void registerMetricFunctor(MetricFunctor<Types> metricFunctor, bool use = false) {
				registeredMetricFunctors[metricFunctor.name] = metricFunctor;
				if (use) {
					useMetricFunctor(metricFunctor.name);
				}
			}
			void useMetricFunctor(std::string mfName) {
				metricFunctorsInUse.emplace(mfName);
			}
			void registerCallback(Callback callbackType, typename Types::CVariant function) {
				callbacks.emplace(std::make_pair(callbackType, function));
			}

			StepResult search(size_t count);
			void pause();
			void stop();
			typename Types::IndividualPtr createNewIndividual(typename Types::Genotype);
			IndividualIdentifier reserveNewIndividualIdentifier();
			typename Types::IndividualPtr find(IndividualIdentifier);

			void addGeneration(typename Types::Generation gen) { genealogy.push_back(gen); }
			Logger logger;
			typename Types::IndividualPtr bestIndividual;
			typename Types::Generation newGeneration;
			typename Types::Genealogy genealogy;
			//std::deque<typename Types::SMetricMap> generationMetricMap;
			//Types::SMetricMap genealogyMetricMap;

			std::shared_ptr<Datastore<Types>> datastore;
			void recallData();
			IndividualIdentifiers extractIdentifiers(typename Types::Generation const &);
			void saveState(EAGenerationState<Types> const &, std::size_t);
			void saveIndividuals(typename Types::Generation const &);

	        std::atomic<std::size_t> currentGeneration;
	        std::atomic<std::size_t> nextIdentifier;
			EAGenerationState<Types> eaGenerationState;
		private:
			StepResult epoch();
			typename Types::FVariantMap eaFunctions;
			typename Types::CVariantMap callbacks;
			std::map<std::string, MetricFunctor<Types>> registeredMetricFunctors;
			std::set<std::string> metricFunctorsInUse;
			std::map<std::string, VariationFunctor<Types>> registeredVariationFunctors;
			std::set<std::string> variationFunctorsInUse;

			void transformIndividuals(typename Types::Generation &);
			void removeInvalidIndividuals(typename Types::Generation &);
			void evaluateIndividualMetrics(typename Types::Generation &);
			void mergeGenerations(typename Types::Generation &, typename Types::Generation &);
			void sortGeneration(typename Types::Generation &);
			void evaluateVariations();
			std::mutex callbackMutex;
			template <typename F, typename ... VTypes>
				void tryExecuteCallback(F f, VTypes ... vargs) {
					if (f) {
						std::lock_guard<std::mutex> lock(callbackMutex);
						f(vargs...);
					}
				}
			template <typename ... VTypes>
				void tryExecuteCallback(Callback callbackType, VTypes ... vargs) {
					if (callbacks.contains(callbackType)) {
						std::lock_guard<std::mutex> lock(callbackMutex);
						auto & callback(callbacks.at(callbackType));
						if (Callback::Evaluation == callbackType) {
							std::get<typename Types::COnEvaluate>(callback)(vargs...);
						}
						//callbacks.at(callbackType)(vargs...);
					}
				}
			std::deque<std::list<VariationInfo<Types>>> variationInfos;
			EAStatistics<Types> eaStatistics;
			std::mutex eaStatisticsMutex;

			std::atomic<bool> pauseFlag;
			bool checkStopFlagAndMaybeWait();
			std::atomic<bool> stopFlag;
	};
}

#include "DEvA/EvolutionaryAlgorithm.hpp"
#include "DEvA/EvolutionaryAlgorithm_ImportExport.h"