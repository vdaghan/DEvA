#pragma once

#include "deva_version.h"
#include "DEvA/Logger.h"

namespace DEvA {
	template <typename Types>
	EvolutionaryAlgorithm<Types>::EvolutionaryAlgorithm()
		: lambda(0)
	    , pauseFlag(false)
	    , stopFlag(false)
	{
		initialiseLogger();
		spdlog::info("DEvA version: {}", getDEvAVersion());
		setupStandardFunctions();
		//registerEAFunction(DEvA::EAFunction::GenePoolSelection, StandardGenePoolSelectors<Types>::all);
		eaStatistics.eaProgress.currentGeneration = 0;
		currentGeneration.store(0);
		nextIdentifier.store(0);
		tryExecuteCallback<typename Types::CEAStatsUpdate, EAStatistics<Types>>(onEAStatsUpdateCallback, eaStatistics, EAStatisticsUpdateType::Progress);
	}

	template <typename Types>
	bool EvolutionaryAlgorithm<Types>::compile() {
		std::string uncompiled{};
		auto compileLambda = [&](Dependencies & dependencies) {
			std::size_t unsatisfiedBefore(std::numeric_limits<std::size_t>::max());
			std::size_t unsatisfiedNow(0);
			while (unsatisfiedBefore != unsatisfiedNow) {
				uncompiled.clear();
				unsatisfiedBefore = unsatisfiedNow;
				unsatisfiedNow = 0;
				for (auto & dependency : dependencies) {
					if (not dependency.isSatisfied()) {
						uncompiled += dependency.explanation + "\n";
						++unsatisfiedNow;
					}
				}
			}
			return unsatisfiedNow == 0;
		};

		Dependencies allDependencies{};
		auto functionsDependencies(functions.getDependencies());
		std::copy(functionsDependencies.begin(), functionsDependencies.end(), std::back_inserter(allDependencies));
		std::copy(variationFunctors.dependencies.begin(), variationFunctors.dependencies.end(), std::back_inserter(allDependencies));
		std::copy(metricFunctors.dependencies.begin(), metricFunctors.dependencies.end(), std::back_inserter(allDependencies));
		bool compileSuccessful(compileLambda(allDependencies));
		if (not compileSuccessful) {
			spdlog::error("Metrics compilation error. {}", uncompiled);
		}
		return compileSuccessful;
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
			spdlog::info("Creating new individuals.");
			{
				auto lock(eaStatistics.lock());
				eaStatistics.eaProgress.eaStage = EAStage::Genesis;
				tryExecuteCallback<typename Types::CEAStatsUpdate, EAStatistics<Types>>(onEAStatsUpdateCallback, eaStatistics, EAStatisticsUpdateType::Progress);
			}
			
			auto genotypeProxies(functions.genesis.get()());
			//auto genotypeProxies(std::get<typename Types::FGenesis>(eaFunctions.at(EAFunction::Initialisation))());
			//lambda = genotypeProxies.size();
			for (auto& genotypeProxy : genotypeProxies) {
				auto newIndividual = createNewIndividual(genotypeProxy);
				newGeneration.push_back(newIndividual);
			}
			variationInfos.push_back({});
		} else [[likely]] {
			eaGenerationState.elderIdentifiers = extractIdentifiers(genealogy.back());
			//spdlog::info("Selecting gene pool.");
			//{
			//	auto lock(eaStatistics.lock());
			//	eaStatistics.eaProgress.eaStage = EAStage::SelectGenePool;
			//	tryExecuteCallback<typename Types::CEAStatsUpdate, EAStatistics<Types>>(onEAStatsUpdateCallback, eaStatistics, EAStatisticsUpdateType::Progress);
			//}
			//auto genePool = genePoolSelectionFunction(genealogy.back());
			auto genePool = genealogy.back();
			eaGenerationState.genePoolIdentifiers = extractIdentifiers(genealogy.back());
			spdlog::info("Creating new individuals.");
			{
				auto lock(eaStatistics.lock());
				eaStatistics.eaProgress.eaStage = EAStage::Variation;
				tryExecuteCallback<typename Types::CEAStatsUpdate, EAStatistics<Types>>(onEAStatsUpdateCallback, eaStatistics, EAStatisticsUpdateType::Progress);
			}
			std::list<VariationInfo<Types>> newVariationInfos;
			while (newGeneration.size() < lambda) {
				bool notEnoughParents(true);
				for (auto const & variationFunctorName : variationFunctors.used) {
					auto const & variationFunctor(variationFunctors.functors.at(variationFunctorName));
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
					auto newGenotypes = variationInfo.children;
					for (auto const& newGenotype : newGenotypes) {
						auto newIndividual = createNewIndividual(newGenotype);
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

		eaGenerationState.newbornIdentifiers = extractIdentifiers(newGeneration);

		{
			auto lock(eaStatistics.lock());
			eaStatistics.eaProgress.numberOfNewIndividualsInGeneration = newGeneration.size();
			eaStatistics.eaProgress.numberOfOldIndividualsInGeneration = 0 == genealogy.size() ? 0 : genealogy.back().size();
			eaStatistics.eaProgress.numberOfIndividualsInGeneration = eaStatistics.eaProgress.numberOfNewIndividualsInGeneration + eaStatistics.eaProgress.numberOfOldIndividualsInGeneration;
			tryExecuteCallback<typename Types::CEAStatsUpdate, EAStatistics<Types>>(onEAStatsUpdateCallback, eaStatistics, EAStatisticsUpdateType::Progress);
			std::size_t newIndividuals(eaStatistics.eaProgress.numberOfNewIndividualsInGeneration);
			std::size_t oldIndividuals(eaStatistics.eaProgress.numberOfOldIndividualsInGeneration);
			std::size_t individuals(eaStatistics.eaProgress.numberOfIndividualsInGeneration);
			spdlog::info("Epoch {}: {} new, {} old, {} total individuals.", currentGeneration.load(), newIndividuals, oldIndividuals, individuals);
		}

		spdlog::info("Transforming individuals.");
		transformIndividuals(newGeneration);
		if (checkStopFlagAndMaybeWait()) return StepResult::Stopped;

		spdlog::info("Removing invalid individuals.");
		removeInvalidIndividuals(newGeneration);
		if (checkStopFlagAndMaybeWait()) return StepResult::Stopped;

		eaGenerationState.healthyIdentifiers = extractIdentifiers(newGeneration);

		spdlog::info("Evaluating individual metrics.");
		evaluateIndividualMetrics(newGeneration);
		if (checkStopFlagAndMaybeWait()) return StepResult::Stopped;

		spdlog::info("Combining previous generation and new individuals into a new generation.");
		if (0 != genealogy.size()) [[likely]] {
			mergeGenerations(genealogy.back(), newGeneration);
		}
		genealogy.push_back(newGeneration);
		if (checkStopFlagAndMaybeWait()) return StepResult::Stopped;

		spdlog::info("Sorting generation.");
		sortGeneration(genealogy.back());
		if (checkStopFlagAndMaybeWait()) return StepResult::Stopped;

		if (!variationInfos.back().empty()) {
			spdlog::info("Evaluating variations.");
			evaluateVariations();
			if (checkStopFlagAndMaybeWait()) return StepResult::Stopped;
		}

		spdlog::info("Saving new individuals to disk.");
		saveIndividuals(newGeneration);
		spdlog::info("Saving generation state to disk.");
		saveState(eaGenerationState, currentGeneration.load());

		spdlog::info("Selecting survivors.");
		{
			auto lock(eaStatistics.lock());
			eaStatistics.eaProgress.eaStage = EAStage::SelectSurvivors;
			tryExecuteCallback<typename Types::CEAStatsUpdate, EAStatistics<Types>>(onEAStatsUpdateCallback, eaStatistics, EAStatisticsUpdateType::Progress);
		}
		auto survivorSelection(functions.survivorSelection.get());
		genealogy.back() = survivorSelection(genealogy.back());
		//std::get<typename Types::FSurvivorSelection>(eaFunctions.at(EAFunction::SurvivorSelection))(genealogy.back());

		spdlog::info("Sorting generation.");
		sortGeneration(genealogy.back());
		if (checkStopFlagAndMaybeWait()) return StepResult::Stopped;

		eaGenerationState.survivorIdentifiers = extractIdentifiers(genealogy.back());

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
			eaStatistics.eaProgress.currentGeneration = currentGeneration.load();
			eaStatistics.eaProgress.eaStage = EAStage::End;
			tryExecuteCallback<typename Types::CEAStatsUpdate, EAStatistics<Types>>(onEAStatsUpdateCallback, eaStatistics, EAStatisticsUpdateType::Progress);
		}
		if (genealogy.back().empty()) [[unlikely]] {
			return StepResult::Exhaustion;
		}
		auto convergenceCheck(functions.convergenceCheck.get());
		bool converged(convergenceCheck(bestIndividual->metricMap));
		//bool converged(std::get<typename Types::FConvergenceCheck>(eaFunctions.at(EAFunction::ConvergenceCheck))(bestIndividual->metricMap));
		if (converged) [[unlikely]] {
			return StepResult::Convergence;
		}
		return StepResult::Inconclusive;
	}

	template <typename Types>
	void EvolutionaryAlgorithm<Types>::epochStart() {
		spdlog::info("Epoch {} started.", currentGeneration.load());
	}

	template <typename Types>
	void EvolutionaryAlgorithm<Types>::epochEnd() {
		spdlog::info("Epoch {} ended.", currentGeneration.load());

		auto const & bestIndividualPtr = genealogy.back().front();
		spdlog::info("Best individual: {}", bestIndividualPtr->id);
		std::string bestIndividualParents{};
		for (auto & parent : bestIndividualPtr->variationInfo.parentIds) {
			auto parentGeneration(std::to_string(parent.generation));
			auto parentIdentifier(std::to_string(parent.identifier));
			bestIndividualParents += "(" + parentGeneration + ", " + parentIdentifier + ") ";
		}
		spdlog::info("Best individual variation: {} from parent(s) {}", bestIndividualPtr->variationInfo.name, bestIndividualParents);

		auto & bestIndividualMetric(bestIndividualPtr->metricMap);
		for (auto & [metricName, metric] : bestIndividualPtr->metricMap) {
			if (metric.value.type() == std::type_index(typeid(double))) {
				spdlog::info("Best individual {}: {}", metricName, metric.as<double>());
			} else if (metric.metricToJSONObjectFunction) {
				auto metricAsJSON(metric.metricToJSONObjectFunction(metric.value));
				spdlog::info("Best individual {}: {}", metricName, metricAsJSON.dump());
			}
		}

		currentGeneration.fetch_add(1);
	}

	template <typename Types>
	void EvolutionaryAlgorithm<Types>::transformIndividuals(typename Types::Generation & generation) {
		{
			auto lock(eaStatistics.lock());
			eaStatistics.eaProgress.eaStage = EAStage::Transform;
			tryExecuteCallback<typename Types::CEAStatsUpdate, EAStatistics<Types>>(onEAStatsUpdateCallback, eaStatistics, EAStatisticsUpdateType::Progress);
		}
		auto transformLambda = [&](auto iptr) {
			if (checkStopFlagAndMaybeWait()) {
				return;
			}
			auto transform(functions.transform.get());
			iptr->maybePhenotype = transform(iptr->genotype);
			//iptr->maybePhenotype = std::get<typename Types::FTransform>(eaFunctions.at(EAFunction::Transformation))(iptr->genotype);
			{
				auto lock(eaStatistics.lock());
				++eaStatistics.eaProgress.numberOfTransformedIndividualsInGeneration;
				tryExecuteCallback<typename Types::CEAStatsUpdate, EAStatistics<Types>>(onEAStatsUpdateCallback, eaStatistics, EAStatisticsUpdateType::Progress);
			}
		};
		//std::for_each(std::execution::par, generation.begin(), generation.end(), transformLambda);
		std::list<std::jthread> threads{};
		for (auto iptr : generation) {
			std::jthread j(transformLambda, iptr);
			threads.emplace_back(std::move(j));
		}
	}

	template <typename Types>
	void EvolutionaryAlgorithm<Types>::removeInvalidIndividuals(typename Types::Generation & generation) {
		std::size_t const beforeRemoval(generation.size());
		auto removeInvalidsLambda = [&](auto& iptr) {
			return iptr->isInvalid();
		};
		generation.remove_if(removeInvalidsLambda);
		std::size_t const afterRemoval(generation.size());
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
			for (auto const & metricFunctorName : metricFunctors.usedIndividualMetricFunctors) {
				if (checkStopFlagAndMaybeWait()) return;
				auto & metricFunctor(metricFunctors.functors.at(metricFunctorName));
				if (not metricFunctor.valid()) [[unlikely]] {
					continue;
				}
				if (not metricFunctor.computeFromIndividualPtrFunction) [[unlikely]] {
					continue;
				}
				iptr->metricMap.emplace(std::make_pair(metricFunctorName, metricFunctor.compute<typename Types::IndividualPtr>(iptr)));
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
		//if (!eaFunctions.contains(EAFunction::SortIndividuals)) {
		//	return;
		//}
		std::stable_sort(genealogy.back().begin(), genealogy.back().end(), [&](auto& lhs, auto& rhs) {
			auto sortIndividuals(functions.sortIndividuals.get());
			return sortIndividuals(lhs, rhs);
			//return std::get<typename Types::FSortIndividuals>(eaFunctions.at(EAFunction::SortIndividuals))(lhs, rhs);
		});
	}

	template <typename Types>
	void EvolutionaryAlgorithm<Types>::evaluateVariations() {
		//if (!eaFunctions.contains(EAFunction::SortIndividuals)) {
		//	return;
		//}
		auto sortFunction(functions.sortIndividuals.get());
		//auto & sortFunction(std::get<typename Types::FSortIndividuals>(eaFunctions.at(EAFunction::SortIndividuals)));
		auto const & varInfos = variationInfos.back();
		VariationStatisticsMap varStatMap{};
		for (auto const & varInfo : varInfos) {
			if (!varStatMap.contains(varInfo.name)) {
				varStatMap.emplace(std::make_pair(varInfo.name, VariationStatistics{ .success = 0, .fail = 0, .error = 0, .total = 0 }));
			}
			auto & varStat = varStatMap.at(varInfo.name);
			for (auto const & childId : varInfo.childIds) {
				auto const & child = find(childId);
				++varStat.total;
				if (!child or !child->maybePhenotype.has_value()) {
					++varStat.fail;
					continue;
				}
				bool betterThanSome(false);
				for (auto const & parent : varInfo.parentPtrs) {
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
		for (auto const & [varName, varStat] : varStatMap) {
			auto const totalChildren = static_cast<double>(varStat.total);
            auto const varSuccessRate = static_cast<double>(varStat.success) / totalChildren;
            auto const varFailureRate = static_cast<double>(varStat.fail) / totalChildren;
            auto const varErrorRate = static_cast<double>(varStat.error) / totalChildren;
			spdlog::info("Variation {} : (success%, failure%, error%, total#) = ({:5.1f}%, {:5.1f}%, {:5.1f}%, {})", varName, varSuccessRate * 100.0, varFailureRate * 100.0, varErrorRate * 100.0, varStat.total);
		}

		{
			auto lock(eaStatistics.lock());
			eaStatistics.variationStatisticsMap = std::move(varStatMap);
			tryExecuteCallback<typename Types::CEAStatsUpdate, EAStatistics<Types>>(onEAStatsUpdateCallback, eaStatistics, EAStatisticsUpdateType::Variation);
		}
	}

	template <typename Types>
	StepResult EvolutionaryAlgorithm<Types>::search(size_t count) {
		StepResult stepResult(StepResult::Unknown);
		for (size_t i(0); i < count; ++i) {
			if (checkStopFlagAndMaybeWait()) {
				stepResult = StepResult::Stopped;
				break;
			}
			epochStart();
			tryExecuteCallback<typename Types::COnEpoch, std::size_t>(onEpochStartCallback, genealogy.size());
			StepResult const epochResult = epoch();
			epochEnd();
			tryExecuteCallback<typename Types::COnEpoch, std::size_t>(onEpochEndCallback, genealogy.size() - 1);
			if (StepResult::Inconclusive != epochResult) [[unlikely]] {
				stepResult = epochResult;
				break;
			}
		}
		if (StepResult::Unknown == stepResult) {
			stepResult = StepResult::StepCount;
		}
		if (StepResult::Convergence == stepResult) {
			spdlog::info("Search converged.");
		} else if (StepResult::StepCount == stepResult) {
			spdlog::info("Step limit reached.");
		} else if (StepResult::Stopped == stepResult) {
			spdlog::info("Search stopped externally.");
		}
		return stepResult;
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
	typename Types::IndividualPtr EvolutionaryAlgorithm<Types>::createNewIndividual(typename Types::Genotype genotype) {
		IndividualIdentifier id(reserveNewIndividualIdentifier());
		{
			auto lock(eaStatistics.lock());
			++eaStatistics.eaProgress.numberOfNewIndividualsInGeneration;
			tryExecuteCallback<typename Types::CEAStatsUpdate, EAStatistics<Types>>(onEAStatsUpdateCallback, eaStatistics, EAStatisticsUpdateType::Progress);
		}
		auto iptr(std::make_shared<typename Types::SIndividual>(id));
		iptr->genotype = genotype;
		return iptr;
	}

	template <typename Types>
	IndividualIdentifier EvolutionaryAlgorithm<Types>::reserveNewIndividualIdentifier() {
		IndividualIdentifier const id{
			.generation = currentGeneration.load(),
			.identifier = nextIdentifier.fetch_add(1)
		};
		return id;
	}

	template <typename Types>
	typename Types::IndividualPtr EvolutionaryAlgorithm<Types>::find(IndividualIdentifier indId) {
		auto const & gen = indId.generation;
		auto const & id = indId.identifier;
		if (genealogy.size() > gen) [[likely]] {
			for (auto & indPtr : genealogy.at(gen)) {
				if (indPtr->id == indId) {
					return indPtr;
				}
			}
		}
		if (!newGeneration.empty()) [[likely]] {
			for (auto & indPtr : newGeneration) {
				if (indPtr->id == indId) {
					return indPtr;
				}
			}
		}
		return {};
	}
}
