#pragma once

namespace DEvA {
	template <typename Types>
	EvolutionaryAlgorithm<Types>::EvolutionaryAlgorithm()
		: lambda(0)
		, genePoolSelectionFunction(StandardGenePoolSelectors<Types>::all)
	    , pauseFlag(false)
	    , stopFlag(false) {
		eaStatistics.eaProgress.currentGeneration = 0;
		eaState.currentGeneration.store(0);
		eaState.nextIdentifier.store(0);
		tryExecuteCallback<typename Types::CEAStatsUpdate, EAStatistics<Types>>(onEAStatsUpdateCallback, eaStatistics, EAStatisticsUpdateType::Progress);
	}
	template <typename Types>
	StepResult EvolutionaryAlgorithm<Types>::epoch() {
		{
			auto lock(eaStatistics.lock());
			eaStatistics.eaProgress.numberOfNewIndividualsInGeneration = 0;
			eaStatistics.eaProgress.numberOfOldIndividualsInGeneration = 0;
			eaStatistics.eaProgress.numberOfIndividualsInGeneration = 0;
			eaStatistics.eaProgress.numberOfTransformedIndividualsInGeneration = 0;
			eaStatistics.eaProgress.numberOfInvalidIndividualsInGeneration = 0;
			eaStatistics.eaProgress.numberOfEvaluatedIndividualsInGeneration = 0;
		}
		tryExecuteCallback<typename Types::CEAStatsUpdate, EAStatistics<Types>>(onEAStatsUpdateCallback, eaStatistics, EAStatisticsUpdateType::Progress);
		typename Types::Generation newGeneration{};
		if (0 == genealogy.size()) [[unlikely]] {
			auto genotypeProxies(genesisFunction());
			//lambda = genotypeProxies.size();
			for (auto& genotypeProxy : genotypeProxies) {
				auto newIndividual = createNewIndividual(genotypeProxy);
				newGeneration.push_back(newIndividual);
			}
			variationInfos.push_back({});
		} else [[likely]] {
			auto genePool = genePoolSelectionFunction(genealogy.back());
			std::list<VariationInfo<Types>> newVariationInfos;
			while (newGeneration.size() < lambda) {
				bool notEnoughParents(true);
				for (auto const& variationFunctor : variationFunctors) {
					if (checkStopFlagAndMaybeWait()) return StepResult::Stopped;
					if (newGeneration.size() >= lambda) {
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
					auto newGenotypeProxies = variationInfo.childProxies;
					for (auto const& newGenotypeProxy : newGenotypeProxies) {
						auto newIndividual = createNewIndividual(newGenotypeProxy);
						newIndividual->setParents(variationInfo.parentPtrs);
						variationInfo.childIds.push_back(newIndividual->id);
						newGeneration.emplace_back(newIndividual);
					}
					newVariationInfos.push_back(variationInfo);
					tryExecuteCallback<typename Types::COnVariation, VariationInfo<Types> const&>(onVariationCallback, variationInfo);
				}
				if (notEnoughParents) {
					break;
				}
			}
			variationInfos.push_back(newVariationInfos);
		}

		{
			auto lock(eaStatistics.lock());
			eaStatistics.eaProgress.numberOfNewIndividualsInGeneration = newGeneration.size();
			eaStatistics.eaProgress.numberOfOldIndividualsInGeneration = 0 == genealogy.size() ? 0 : genealogy.back().size();
			eaStatistics.eaProgress.numberOfIndividualsInGeneration = eaStatistics.eaProgress.numberOfNewIndividualsInGeneration + eaStatistics.eaProgress.numberOfOldIndividualsInGeneration;
		}
		tryExecuteCallback<typename Types::CEAStatsUpdate, EAStatistics<Types>>(onEAStatsUpdateCallback, eaStatistics, EAStatisticsUpdateType::Progress);
		{
			auto lock(eaStatistics.lock());
			std::size_t newIndividuals(eaStatistics.eaProgress.numberOfNewIndividualsInGeneration);
			std::size_t oldIndividuals(eaStatistics.eaProgress.numberOfOldIndividualsInGeneration);
			std::size_t individuals(eaStatistics.eaProgress.numberOfIndividualsInGeneration);
			logger.info("Epoch {}: {} new, {} old, {} total individuals.", eaState.currentGeneration.load(), newIndividuals, oldIndividuals, individuals);
		}
		
		transformIndividuals(newGeneration);
		if (checkStopFlagAndMaybeWait()) return StepResult::Stopped;

		removeInvalidIndividuals(newGeneration);
		if (checkStopFlagAndMaybeWait()) return StepResult::Stopped;

		evaluateIndividuals(newGeneration);
		if (checkStopFlagAndMaybeWait()) return StepResult::Stopped;

		if (0 != genealogy.size()) [[likely]] {
			mergeGenerations(genealogy.back(), newGeneration);
		}
		genealogy.push_back(newGeneration);
		if (checkStopFlagAndMaybeWait()) return StepResult::Stopped;

		sortGeneration(genealogy.back());
		if (checkStopFlagAndMaybeWait()) return StepResult::Stopped;

		{
			auto lock(eaStatistics.lock());
			eaStatistics.fitnesses.clear();
		}
		for (auto& iptr : genealogy.back()) {
			if (!iptr->maybePhenotypeProxy) continue;
			auto lock(eaStatistics.lock());
			eaStatistics.fitnesses.push_back(iptr->fitness);
		};
		tryExecuteCallback<typename Types::CEAStatsUpdate, EAStatistics<Types>>(onEAStatsUpdateCallback, eaStatistics, EAStatisticsUpdateType::Fitness);

		if (checkStopFlagAndMaybeWait()) return StepResult::Stopped;

		if (distanceCalculationFunction) {
			computeDistances();
		}

		if (checkStopFlagAndMaybeWait()) return StepResult::Stopped;

		if (!variationInfos.back().empty()) {
			evaluateVariations();
		}

		survivorSelectionFunction(genealogy.back());

		auto bestIndividual = genealogy.back().front();
		bestGenotype = bestIndividual->genotypeProxy;
		bestPhenotype = bestIndividual->maybePhenotypeProxy.value();
		bestFitness = bestIndividual->fitness;
		//std::cout << "Best fitness: " << bestFitness << "\n";

		tryExecuteCallback<typename Types::CEAStatsUpdate, EAStatistics<Types>>(onEAStatsUpdateCallback, eaStatistics, EAStatisticsUpdateType::Final);
		{
			auto lock(eaStatistics.lock());
			eaStatistics.eaProgress.currentGeneration = eaState.currentGeneration.load();
		}
		eaState.currentGeneration.fetch_add(1);
		tryExecuteCallback<typename Types::CEAStatsUpdate, EAStatistics<Types>>(onEAStatsUpdateCallback, eaStatistics, EAStatisticsUpdateType::Progress);
		if (genealogy.back().empty()) [[unlikely]] {
			return StepResult::Exhaustion;
		}
		if (convergenceCheckFunction(bestFitness)) [[unlikely]] {
			return StepResult::Convergence;
		}
		return StepResult::Inconclusive;
	}

	template <typename Types>
	void EvolutionaryAlgorithm<Types>::transformIndividuals(Types::Generation& generation) {
		auto transformLambda = [&](auto& iptr) {
			if (checkStopFlagAndMaybeWait()) {
				return;
			}
			iptr->maybePhenotypeProxy = transformFunction(iptr->genotypeProxy);
			{
				auto lock(eaStatistics.lock());
				++eaStatistics.eaProgress.numberOfTransformedIndividualsInGeneration;
			}
			tryExecuteCallback<typename Types::CEAStatsUpdate, EAStatistics<Types>>(onEAStatsUpdateCallback, eaStatistics, EAStatisticsUpdateType::Progress);

		};
		std::for_each(std::execution::seq, generation.begin(), generation.end(), transformLambda);
	}

	template <typename Types>
	void EvolutionaryAlgorithm<Types>::removeInvalidIndividuals(Types::Generation & generation) {
		std::size_t beforeRemoval(generation.size());
		auto removeInvalidsLambda = [&](auto& iptr) {
			return iptr->isInvalid();
		};
		generation.remove_if(removeInvalidsLambda);
		std::size_t afterRemoval(generation.size());
		{
			auto lock(eaStatistics.lock());
			eaStatistics.eaProgress.numberOfInvalidIndividualsInGeneration = afterRemoval - beforeRemoval;
		}
		tryExecuteCallback<typename Types::CEAStatsUpdate, EAStatistics<Types>>(onEAStatsUpdateCallback, eaStatistics, EAStatisticsUpdateType::Progress);
	}

	template <typename Types>
	void EvolutionaryAlgorithm<Types>::evaluateIndividuals(Types::Generation & generation) {
		auto evaluateLambda = [&](auto& iptr) {
			if (checkStopFlagAndMaybeWait()) return;
			iptr->fitness = evaluationFunction(iptr->maybePhenotypeProxy.value());
			{
				auto lock(eaStatistics.lock());
				++eaStatistics.eaProgress.numberOfEvaluatedIndividualsInGeneration;
			}
			tryExecuteCallback<typename Types::CEAStatsUpdate, EAStatistics<Types>>(onEAStatsUpdateCallback, eaStatistics, EAStatisticsUpdateType::Progress);
		};
		std::for_each(std::execution::seq, generation.begin(), generation.end(), evaluateLambda);
	}

	template <typename Types>
	void EvolutionaryAlgorithm<Types>::mergeGenerations(Types::Generation & from, Types::Generation & to) {
		to.insert(to.end(), from.begin(), from.end());
	}

	template <typename Types>
	void EvolutionaryAlgorithm<Types>::sortGeneration(Types::Generation & generation) {
		std::stable_sort(genealogy.back().begin(), genealogy.back().end(), [&](auto& lhs, auto& rhs) {
			return fitnessComparisonFunction(lhs->fitness, rhs->fitness);
		});
	}

	template <typename Types>
	void EvolutionaryAlgorithm<Types>::computeDistances() {
		{
			auto lock(eaStatistics.lock());
			eaStatistics.distanceMatrix.clear();
		}
		auto computeDistanceLambda = [&](auto& iptr) {
			auto const& id1(iptr->id);
			//if (!iptr->maybePhenotypeProxy) return;
			for (auto& iptr2 : genealogy.back()) {
				//if (checkStopFlagAndMaybeWait()) return;
				auto const& id2(iptr2->id);
				//if (!iptr2->maybePhenotypeProxy) return;
				{
					auto lock(eaStatistics.lock());
					eaStatistics.distanceMatrix[id1][id2] = distanceCalculationFunction(id1, id2);
				}
			}
		};
		std::for_each(std::execution::seq, genealogy.back().begin(), genealogy.back().end(), computeDistanceLambda);
		tryExecuteCallback<typename Types::CEAStatsUpdate, EAStatistics<Types>>(onEAStatsUpdateCallback, eaStatistics, EAStatisticsUpdateType::Distance);
	}

	template <typename Types>
	void EvolutionaryAlgorithm<Types>::evaluateVariations() {
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
				}
				else {
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
			logger.info("Variation {} : success={:5.1f}%, failure={:5.1f}%, error={:5.1f}%, total#={}", varName, varSuccessRate * 100, varFailureRate * 100, varErrorRate * 100, varStat.total);
		}

		{
			auto lock(eaStatistics.lock());
			eaStatistics.variationStatisticsMap = std::move(varStatMap);
		}
		tryExecuteCallback<typename Types::CEAStatsUpdate, EAStatistics<Types>>(onEAStatsUpdateCallback, eaStatistics, EAStatisticsUpdateType::Variation);
	}

	template <typename Types>
	StepResult EvolutionaryAlgorithm<Types>::search(size_t count) {
		for (size_t i(0); i < count; ++i) {
			if (checkStopFlagAndMaybeWait()) {
				return StepResult::Stopped;
			}
			tryExecuteCallback<typename Types::COnEpoch, std::size_t>(onEpochStartCallback, genealogy.size());
			StepResult epochResult = epoch();
			tryExecuteCallback<typename Types::COnEpoch, std::size_t>(onEpochEndCallback, genealogy.size() - 1);
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
			tryExecuteCallback<typename Types::CVoid>(onPauseCallback);
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
		tryExecuteCallback<typename Types::CVoid>(onStopCallback);
	}

	template <typename Types>
	Types::IndividualPtr EvolutionaryAlgorithm<Types>::createNewIndividual(Types::GenotypeProxy gpx) {
		IndividualIdentifier id;
		if constexpr (std::is_same<typename Types::GenotypeProxy, IndividualIdentifier>()) {
			id = gpx;
		} else {
			id = reserveNewIndividualIdentifier();
		}
		{
			auto lock(eaStatistics.lock());
			++eaStatistics.eaProgress.numberOfIndividualsInGeneration;
		}
		tryExecuteCallback<typename Types::CEAStatsUpdate, EAStatistics<Types>>(onEAStatsUpdateCallback, eaStatistics, EAStatisticsUpdateType::Progress);
		return std::make_shared<typename Types::SIndividual>(id.generation, id.identifier, gpx);
	}

	template <typename Types>
	IndividualIdentifier EvolutionaryAlgorithm<Types>::reserveNewIndividualIdentifier() {
		IndividualIdentifier id{
			.generation = eaState.currentGeneration.load(),
			.identifier = eaState.nextIdentifier.fetch_add(1)
		};
		return id;
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
}
