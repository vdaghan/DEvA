#pragma once

namespace DEvA {
	template <typename Types>
	EvolutionaryAlgorithm<Types>::EvolutionaryAlgorithm()
		: lambda(0)
	    , pauseFlag(false)
	    , stopFlag(false) {
		//registerEAFunction(DEvA::EAFunction::GenePoolSelection, StandardGenePoolSelectors<Types>::all);
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
			eaStatistics.eaProgress.eaStage = EAStage::Start;
			tryExecuteCallback<typename Types::CEAStatsUpdate, EAStatistics<Types>>(onEAStatsUpdateCallback, eaStatistics, EAStatisticsUpdateType::Progress);
		}
		newGeneration = {};
		if (0 == genealogy.size()) [[unlikely]] {
			logger.info("Creating new individuals.");
			{
				auto lock(eaStatistics.lock());
				eaStatistics.eaProgress.eaStage = EAStage::Genesis;
				tryExecuteCallback<typename Types::CEAStatsUpdate, EAStatistics<Types>>(onEAStatsUpdateCallback, eaStatistics, EAStatisticsUpdateType::Progress);
			}
			auto genotypeProxies(std::get<typename Types::FGenesis>(eaFunctions.at(EAFunction::Initialisation))());
			//lambda = genotypeProxies.size();
			for (auto& genotypeProxy : genotypeProxies) {
				auto newIndividual = createNewIndividual(genotypeProxy);
				newGeneration.push_back(newIndividual);
			}
			variationInfos.push_back({});
		} else [[likely]] {
			//logger.info("Selecting gene pool.");
			//{
			//	auto lock(eaStatistics.lock());
			//	eaStatistics.eaProgress.eaStage = EAStage::SelectGenePool;
			//	tryExecuteCallback<typename Types::CEAStatsUpdate, EAStatistics<Types>>(onEAStatsUpdateCallback, eaStatistics, EAStatisticsUpdateType::Progress);
			//}
			//auto genePool = genePoolSelectionFunction(genealogy.back());
			auto genePool = genealogy.back();
			logger.info("Creating new individuals.");
			{
				auto lock(eaStatistics.lock());
				eaStatistics.eaProgress.eaStage = EAStage::Variation;
				tryExecuteCallback<typename Types::CEAStatsUpdate, EAStatistics<Types>>(onEAStatsUpdateCallback, eaStatistics, EAStatisticsUpdateType::Progress);
			}
			std::list<VariationInfo<Types>> newVariationInfos;
			while (newGeneration.size() < lambda) {
				bool notEnoughParents(true);
				for (auto const & variationFunctorName : variationFunctorsInUse) {
					auto const & variationFunctor(registeredVariationFunctors.at(variationFunctorName));
					if (checkStopFlagAndMaybeWait()) return StepResult::Stopped;
					if (newGeneration.size() >= lambda) {
						break;
					}
					if (genePool.size() < variationFunctor.numberOfParents) [[unlikely]] {
						continue;
					}
					//auto & fitnessComparisonFunction = std::get<typename Types::FMetricComparison>(metricComparisons.at(EAFunction::MetricComparison));
					auto maybeVariationInfo = variationFunctor.apply(genePool);
					if (maybeVariationInfo == std::unexpected(ErrorCode::NotEnoughParentsToChoose)) {
						continue;
					}
					if (maybeVariationInfo == std::unexpected(ErrorCode::NoSuitableParentsToChoose)) {
						continue;
					}
					notEnoughParents = false;
					if (!maybeVariationInfo.has_value()) {
						continue;
					}
					auto & variationInfo = maybeVariationInfo.value();
					for (auto& parentPtr : variationInfo.parentPtrs) {
						variationInfo.parentIds.push_back(parentPtr->id);
					}
					auto newGenotypeProxies = variationInfo.childProxies;
					for (auto const& newGenotypeProxy : newGenotypeProxies) {
						auto newIndividual = createNewIndividual(newGenotypeProxy);
						newIndividual->variationInfo = variationInfo;
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
			tryExecuteCallback<typename Types::CEAStatsUpdate, EAStatistics<Types>>(onEAStatsUpdateCallback, eaStatistics, EAStatisticsUpdateType::Progress);
			std::size_t newIndividuals(eaStatistics.eaProgress.numberOfNewIndividualsInGeneration);
			std::size_t oldIndividuals(eaStatistics.eaProgress.numberOfOldIndividualsInGeneration);
			std::size_t individuals(eaStatistics.eaProgress.numberOfIndividualsInGeneration);
			logger.info("Epoch {}: {} new, {} old, {} total individuals.", eaState.currentGeneration.load(), newIndividuals, oldIndividuals, individuals);
		}

		logger.info("Transforming individuals.");
		transformIndividuals(newGeneration);
		if (checkStopFlagAndMaybeWait()) return StepResult::Stopped;

		logger.info("Removing invalid individuals.");
		removeInvalidIndividuals(newGeneration);
		if (checkStopFlagAndMaybeWait()) return StepResult::Stopped;

		logger.info("Evaluating individual metrics.");
		evaluateIndividualMetrics(newGeneration);
		if (checkStopFlagAndMaybeWait()) return StepResult::Stopped;

		logger.info("Combining previous generation and new individuals into a new generation.");
		if (0 != genealogy.size()) [[likely]] {
			mergeGenerations(genealogy.back(), newGeneration);
		}
		genealogy.push_back(newGeneration);
		if (checkStopFlagAndMaybeWait()) return StepResult::Stopped;

		logger.info("Sorting generation.");
		sortGeneration(genealogy.back());
		if (checkStopFlagAndMaybeWait()) return StepResult::Stopped;

		if (!variationInfos.back().empty()) {
			logger.info("Evaluating variations.");
			evaluateVariations();
			if (checkStopFlagAndMaybeWait()) return StepResult::Stopped;
		}

		logger.info("Saving data.");
		saveData();

		logger.info("Selecting survivors.");
		{
			auto lock(eaStatistics.lock());
			eaStatistics.eaProgress.eaStage = EAStage::SelectSurvivors;
			tryExecuteCallback<typename Types::CEAStatsUpdate, EAStatistics<Types>>(onEAStatsUpdateCallback, eaStatistics, EAStatisticsUpdateType::Progress);
		}
		std::get<typename Types::FSurvivorSelection>(eaFunctions.at(EAFunction::SurvivorSelection))(genealogy.back());

		//{
		//	std::list<typename Types::MetricVariantMap> individualMetrics;
		//	for (auto& iptr : genealogy.back()) {
		//		if (!iptr->maybePhenotypeProxy) continue;
		//		individualMetrics.push_back(iptr->metrics);
		//	}
		//	auto lock(eaStatistics.lock());
		//	eaStatistics.individualMetrics = individualMetrics;
		//	tryExecuteCallback<typename Types::CEAStatsUpdate, EAStatistics<Types>>(onEAStatsUpdateCallback, eaStatistics, EAStatisticsUpdateType::Fitness);
		//}

		if (checkStopFlagAndMaybeWait()) return StepResult::Stopped;

		bestIndividual = genealogy.back().front();

		{
			auto lock(eaStatistics.lock());
			tryExecuteCallback<typename Types::CEAStatsUpdate, EAStatistics<Types>>(onEAStatsUpdateCallback, eaStatistics, EAStatisticsUpdateType::Final);
			eaStatistics.eaProgress.currentGeneration = eaState.currentGeneration.load();
			eaState.currentGeneration.fetch_add(1);
			eaStatistics.eaProgress.eaStage = EAStage::End;
			tryExecuteCallback<typename Types::CEAStatsUpdate, EAStatistics<Types>>(onEAStatsUpdateCallback, eaStatistics, EAStatisticsUpdateType::Progress);
		}
		if (genealogy.back().empty()) [[unlikely]] {
			return StepResult::Exhaustion;
		}
		if (std::get<typename Types::FConvergenceCheck>(eaFunctions.at(EAFunction::ConvergenceCheck))(bestIndividual->metricMap)) [[unlikely]] {
			return StepResult::Convergence;
		}
		return StepResult::Inconclusive;
	}

	template <typename Types>
	void EvolutionaryAlgorithm<Types>::transformIndividuals(typename Types::Generation & generation) {
		{
			auto lock(eaStatistics.lock());
			eaStatistics.eaProgress.eaStage = EAStage::Transform;
			tryExecuteCallback<typename Types::CEAStatsUpdate, EAStatistics<Types>>(onEAStatsUpdateCallback, eaStatistics, EAStatisticsUpdateType::Progress);
		}
		auto transformLambda = [&](auto & iptr) {
			if (checkStopFlagAndMaybeWait()) {
				return;
			}
			iptr->maybePhenotypeProxy = std::get<typename Types::FTransform>(eaFunctions.at(EAFunction::Transformation))(iptr->genotypeProxy);
			{
				auto lock(eaStatistics.lock());
				++eaStatistics.eaProgress.numberOfTransformedIndividualsInGeneration;
				tryExecuteCallback<typename Types::CEAStatsUpdate, EAStatistics<Types>>(onEAStatsUpdateCallback, eaStatistics, EAStatisticsUpdateType::Progress);
			}
		};
		std::for_each(std::execution::par, generation.begin(), generation.end(), transformLambda);
	}

	template <typename Types>
	void EvolutionaryAlgorithm<Types>::removeInvalidIndividuals(typename Types::Generation & generation) {
		std::size_t beforeRemoval(generation.size());
		auto removeInvalidsLambda = [&](auto& iptr) {
			return iptr->isInvalid();
		};
		generation.remove_if(removeInvalidsLambda);
		std::size_t afterRemoval(generation.size());
		{
			auto lock(eaStatistics.lock());
			eaStatistics.eaProgress.numberOfInvalidIndividualsInGeneration = afterRemoval - beforeRemoval;
			tryExecuteCallback<typename Types::CEAStatsUpdate, EAStatistics<Types>>(onEAStatsUpdateCallback, eaStatistics, EAStatisticsUpdateType::Progress);
		}
	}

	template <typename Types>
	void EvolutionaryAlgorithm<Types>::evaluateIndividualMetrics(typename Types::Generation & generation) {
		{
			auto lock(eaStatistics.lock());
			eaStatistics.eaProgress.eaStage = EAStage::Evaluate;
			tryExecuteCallback<typename Types::CEAStatsUpdate, EAStatistics<Types>>(onEAStatsUpdateCallback, eaStatistics, EAStatisticsUpdateType::Progress);
		}
		auto evaluateLambda = [&](auto & iptr) {
			for (auto const & metricFunctorName : metricFunctorsInUse) {
				if (checkStopFlagAndMaybeWait()) return;
				auto & metricFunctor(registeredMetricFunctors.at(metricFunctorName));
				if (metricFunctor.valid()) [[likely]] {
					if (metricFunctor.computeFromIndividualPtrFunction) {
						iptr->metricMap.emplace(std::make_pair(metricFunctorName, metricFunctor.compute<typename Types::IndividualPtr>(iptr)));
					}
				}
			}
			tryExecuteCallback<IndividualIdentifier>(Callback::Evaluation, iptr->id);
			if (checkStopFlagAndMaybeWait()) return;
			{
				auto lock(eaStatistics.lock());
				++eaStatistics.eaProgress.numberOfEvaluatedIndividualsInGeneration;
				tryExecuteCallback<typename Types::CEAStatsUpdate, EAStatistics<Types>>(onEAStatsUpdateCallback, eaStatistics, EAStatisticsUpdateType::Progress);
			}
		};
		std::for_each(std::execution::par, generation.begin(), generation.end(), evaluateLambda);
	}

	template <typename Types>
	void EvolutionaryAlgorithm<Types>::mergeGenerations(typename Types::Generation & from, typename Types::Generation & to) {
		to.insert(to.end(), from.begin(), from.end());
	}

	template <typename Types>
	void EvolutionaryAlgorithm<Types>::sortGeneration(typename Types::Generation & generation) {
		if (!eaFunctions.contains(EAFunction::SortIndividuals)) {
			return;
		}
		std::stable_sort(genealogy.back().begin(), genealogy.back().end(), [&](auto& lhs, auto& rhs) {
			return std::get<typename Types::FSortIndividuals>(eaFunctions.at(EAFunction::SortIndividuals))(lhs, rhs);
		});
	}

	template <typename Types>
	void EvolutionaryAlgorithm<Types>::evaluateVariations() {
		if (!eaFunctions.contains(EAFunction::SortIndividuals)) {
			return;
		}
		auto & sortFunction(std::get<typename Types::FSortIndividuals>(eaFunctions.at(EAFunction::SortIndividuals)));
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
				if (!child or !child->maybePhenotypeProxy.has_value()) {
					++varStat.fail;
					continue;
				}
				bool betterThanSome(false);
				for (auto const& parent : varInfo.parentPtrs) {
					if (sortFunction(child, parent)) {
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
		for (auto const & varStatPair : varStatMap) {
			auto const & varName = varStatPair.first;
			auto const & varStat = varStatPair.second;
			auto totalChildren = static_cast<double>(varStat.total);
            auto varSuccessRate = static_cast<double>(varStat.success) / totalChildren;
            auto varFailureRate = static_cast<double>(varStat.fail) / totalChildren;
            auto varErrorRate = static_cast<double>(varStat.error) / totalChildren;
			logger.info("Variation {} : (success%, failure%, error%, total#) = ({:5.1f}%, {:5.1f}%, {:5.1f}%, {})", varName, varSuccessRate * 100, varFailureRate * 100, varErrorRate * 100, varStat.total);
		}

		{
			auto lock(eaStatistics.lock());
			eaStatistics.variationStatisticsMap = std::move(varStatMap);
			tryExecuteCallback<typename Types::CEAStatsUpdate, EAStatistics<Types>>(onEAStatsUpdateCallback, eaStatistics, EAStatisticsUpdateType::Variation);
		}
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
	}

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
	typename Types::IndividualPtr EvolutionaryAlgorithm<Types>::createNewIndividual(typename Types::GenotypeProxy gpx) {
		IndividualIdentifier id{};
		if constexpr (std::is_same<typename Types::GenotypeProxy, IndividualIdentifier>()) {
			id = gpx;
		} else {
			id = reserveNewIndividualIdentifier();
		}
		{
			auto lock(eaStatistics.lock());
			++eaStatistics.eaProgress.numberOfNewIndividualsInGeneration;
			tryExecuteCallback<typename Types::CEAStatsUpdate, EAStatistics<Types>>(onEAStatsUpdateCallback, eaStatistics, EAStatisticsUpdateType::Progress);
		}
		return std::make_shared<typename Types::SIndividual>(id, gpx);
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
	typename Types::IndividualPtr EvolutionaryAlgorithm<Types>::find(IndividualIdentifier indId) {
		auto const& gen = indId.generation;
		auto const& id = indId.identifier;
		if (genealogy.size() > gen) [[likely]] {
			for (auto& indPtr : genealogy.at(gen)) {
				if (indPtr->id == indId) {
					return indPtr;
				}
			}
		}
		if (!newGeneration.empty()) [[likely]] {
			for (auto& indPtr : newGeneration) {
				if (indPtr->id == indId) {
					return indPtr;
				}
			}
		}
		return {};
	}
}
