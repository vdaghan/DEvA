#pragma once

namespace DEvA {
	template <typename Types>
	EvolutionaryAlgorithm<Types>::EvolutionaryAlgorithm()
		: lambda(0)
		, genePoolSelectionFunction(StandardGenePoolSelectors<Types>::all)
	    , pauseFlag(false)
	    , stopFlag(false) {
		eaStatistics.eaProgress.currentGeneration = 0;
		eaStatistics.eaProgress.nextIdentifier = 0;
		eaStatistics.eaProgress.numberOfIndividualsInGeneration = 0;
		tryExecuteCallback<Types::CEAStatsUpdate, EAStatistics>(onEAStatsUpdateCallback, eaStatistics);
	}
	template <typename Types>
	StepResult EvolutionaryAlgorithm<Types>::epoch() {
		eaStatistics.eaProgress.numberOfTransformedIndividualsInGeneration = 0;
		eaStatistics.eaProgress.numberOfEvaluatedIndividualsInGeneration = 0;
		typename Types::Generation newGeneration{};
		if (0 == genealogy.size()) [[unlikely]] {
			auto genotypeProxies(genesisFunction());
			//lambda = genotypeProxies.size();
			for (auto& genotypeProxy : genotypeProxies) {
				auto newIndividual = std::make_shared<Individual<Types, Types::IndividualParameters>>(eaStatistics.eaProgress.currentGeneration, eaStatistics.eaProgress.nextIdentifier, genotypeProxy);
				++eaStatistics.eaProgress.numberOfIndividualsInGeneration;
				++eaStatistics.eaProgress.nextIdentifier;
				tryExecuteCallback<Types::CEAStatsUpdate, EAStatistics>(onEAStatsUpdateCallback, eaStatistics);
				newGeneration.push_back(newIndividual);
			}
			variationInfos.push_back({});
		} else [[likely]] {
			++eaStatistics.eaProgress.currentGeneration;
			tryExecuteCallback<Types::CEAStatsUpdate, EAStatistics>(onEAStatsUpdateCallback, eaStatistics);
			auto genePool = genePoolSelectionFunction(genealogy.back());
			std::list<VariationInfo<Types>> newVariationInfos;
			typename Types::IndividualPtrs newOffsprings{};
			while (newOffsprings.size() < lambda) {
				bool notEnoughParents(true);
				for (auto const& variationFunctor : variationFunctors) {
					if (checkStopFlagAndMaybeWait()) {
						return StepResult::Stopped;
					}
					if (newOffsprings.size() >= lambda) {
						break;
					}
					auto maybeVariationInfo = variationFunctor.apply(fitnessComparisonFunction, genePool);
					if (maybeVariationInfo == std::unexpected(ErrorCode::NotApplied)) {
						continue;
					}
					if (maybeVariationInfo == std::unexpected(ErrorCode::NotEnoughParentsToChoose)) {
						continue;
					}
					notEnoughParents = false;
					auto& variationInfo = maybeVariationInfo.value();
					for (auto& parentPtr : variationInfo.parentPtrs) {
						variationInfo.parentIds.push_back(parentPtr->id);
					}
					auto newGenotypes = variationInfo.childProxies;
					for (auto const& newGenotype : newGenotypes) {
						auto newIndividual = std::make_shared<Individual<Types, Types::IndividualParameters>>(eaStatistics.eaProgress.currentGeneration, eaStatistics.eaProgress.nextIdentifier, newGenotype);
						++eaStatistics.eaProgress.numberOfIndividualsInGeneration;
						++eaStatistics.eaProgress.nextIdentifier;
						tryExecuteCallback<Types::CEAStatsUpdate, EAStatistics>(onEAStatsUpdateCallback, eaStatistics);
						newIndividual->setParents(variationInfo.parentPtrs);
						variationInfo.childIds.push_back(newIndividual->id);
						newOffsprings.emplace_back(newIndividual);
					}
					newVariationInfos.push_back(variationInfo);
					tryExecuteCallback<Types::COnVariation, VariationInfo<Types> const&>(onVariationCallback, variationInfo);
				}
				if (notEnoughParents) {
					break;
				}
			}
			newGeneration.insert(newGeneration.end(), newOffsprings.begin(), newOffsprings.end());
			newGeneration.insert(newGeneration.end(), genealogy.back().begin(), genealogy.back().end());
			variationInfos.push_back(newVariationInfos);
		}
		genealogy.push_back(newGeneration);
		logger.info("Epoch {}: {} individuals.", eaStatistics.eaProgress.currentGeneration, eaStatistics.eaProgress.numberOfIndividualsInGeneration);

		std::mutex eaStatusWriteMutex;
		std::for_each(genealogy.back().begin(), genealogy.back().end(), [&](auto& iptr) {
			if (checkStopFlagAndMaybeWait()) {
				return;
			}
			iptr->maybePhenotypeProxy = transformFunction(iptr->genotypeProxy);
			std::lock_guard<std::mutex> lock(eaStatusWriteMutex);
			++eaStatistics.eaProgress.numberOfTransformedIndividualsInGeneration;
			tryExecuteCallback<typename Types::CEAStatsUpdate, EAStatistics>(onEAStatsUpdateCallback, eaStatistics);
		});
		genealogy.back().remove_if([&](auto& iptr) {
			return iptr->isInvalid();
		});
		std::for_each(genealogy.back().begin(), genealogy.back().end(), [&](auto& iptr) {
			if (checkStopFlagAndMaybeWait()) {
				return;
			}
			iptr->fitness = evaluationFunction(iptr->maybePhenotypeProxy.value());
			std::lock_guard<std::mutex> lock(eaStatusWriteMutex);
			++eaStatistics.eaProgress.numberOfEvaluatedIndividualsInGeneration;
			tryExecuteCallback<typename Types::CEAStatsUpdate, EAStatistics>(onEAStatsUpdateCallback, eaStatistics);
		});
		std::stable_sort(genealogy.back().begin(), genealogy.back().end(), [&](auto& lhs, auto& rhs) {
			return fitnessComparisonFunction(lhs->fitness, rhs->fitness);
		});
		if (checkStopFlagAndMaybeWait()) {
			return StepResult::Stopped;
		}

		if (!variationInfos.back().empty()) {
			VariationStatisticsMap vSM = evaluateVariations();
			eaStatistics.variationStatisticsMap = vSM;
			tryExecuteCallback<Types::CEAStatsUpdate, EAStatistics>(onEAStatsUpdateCallback, eaStatistics);
		}

		survivorSelectionFunction(genealogy.back());

		auto bestIndividual = genealogy.back().front();
		bestGenotype = bestIndividual->genotypeProxy;
		bestPhenotype = bestIndividual->maybePhenotypeProxy.value();
		bestFitness = bestIndividual->fitness;
		//std::cout << "Best fitness: " << bestFitness << "\n";

		eaStatisticsHistory.push_back(eaStatistics);
		tryExecuteCallback<Types::CEAStatsHistoryUpdate, EAStatisticsHistory>(onEAStatsHistoryUpdateCallback, eaStatisticsHistory);
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
			if (checkStopFlagAndMaybeWait()) {
				return StepResult::Stopped;
			}
			tryExecuteCallback<Types::COnEpoch, std::size_t>(onEpochStartCallback, genealogy.size());
			StepResult epochResult = epoch();
			tryExecuteCallback<Types::COnEpoch, std::size_t>(onEpochEndCallback, genealogy.size() - 1);
			if (StepResult::Inconclusive != epochResult) [[unlikely]] {
				return epochResult;
			}
		}
		return StepResult::StepCount;
	};

	template <typename Types>
	void EvolutionaryAlgorithm<Types>::pause() {
		pauseFlag.store(!pauseFlag.load());
		if (pauseFlag.load()) {
			tryExecuteCallback<Types::CVoid>(onPauseCallback);
		}
	}

	template <typename Types>
	bool EvolutionaryAlgorithm<Types>::checkStopFlagAndMaybeWait() {
		while (pauseFlag.load() and !stopFlag.load()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		return stopFlag.load();
	}

	template <typename Types>
	void EvolutionaryAlgorithm<Types>::stop() {
		stopFlag.store(true);
		tryExecuteCallback<Types::CVoid>(onStopCallback);
	}

	template <typename Types>
	Types::IndividualPtr EvolutionaryAlgorithm<Types>::find(IndividualIdentifier indId) {
		auto const& gen = indId.generation;
		auto const& id = indId.identifier;
		if (genealogy.size() <= gen) [[unlikely]] {
			return {};
		}
		for (auto& indPtr : genealogy.at(gen)) {
			if (indPtr->id == indId) {
				return indPtr;
			}
		}
		return {};
	}

	template <typename Types>
	VariationStatisticsMap EvolutionaryAlgorithm<Types>::evaluateVariations() {
		auto const& varInfos = variationInfos.back();
		VariationStatisticsMap varStatMap{};
		for (auto const& varInfo : varInfos) {
			if (!varStatMap.contains(varInfo.name)) {
				varStatMap.emplace(std::make_pair(varInfo.name, VariationStatistics{ .success = 0, .fail = 0, .error = 0, .total = 0 }));
			}
			auto& varStat = varStatMap.at(varInfo.name);
			for (auto const& childId : varInfo.childIds) {
				auto const& child = find(childId);
				++varStat.total;
				if (!child->maybePhenotypeProxy.has_value()) {
					++varStat.fail;
					continue;
				}
				bool betterThanSome(false);
				for (auto const& parent : varInfo.parentPtrs) {
					if (child->fitness > parent->fitness) {
						betterThanSome = true;
					}
				}
				if (betterThanSome) {
					++varStat.success;
				} else {
					++varStat.fail;
				}
			}
		}
		for (auto const& varStatPair : varStatMap) {
			auto const& varName = varStatPair.first;
			auto const& varStat = varStatPair.second;
			double totalChildren = static_cast<double>(varStat.total);
			double varSuccessRate = static_cast<double>(varStat.success) / totalChildren;
			double varFailureRate = static_cast<double>(varStat.fail) / totalChildren;
			double varErrorRate = static_cast<double>(varStat.error) / totalChildren;
			logger.info("Variation {} : (success, failure, error, total) = ({}, {}, {}, {})", varName, varSuccessRate, varFailureRate, varErrorRate, varStat.total);
		}
		return varStatMap;
	}
}
