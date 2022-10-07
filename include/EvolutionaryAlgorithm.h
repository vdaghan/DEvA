#pragma once

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
			Types::COnEpoch onEpochStartCallback;
			Types::COnEpoch onEpochEndCallback;
			Types::COnVariation onVariationCallback;

			StepResult epoch();
			StepResult search(size_t count);
			Types::IndividualPtr find(IndividualIdentifier);

			void addGeneration(Types::Generation gen) { genealogy.push_back(gen); };

			Logger logger;
			std::deque<std::list<VariationInfo<Types>>> variationInfos;
			Types::GenotypeProxy bestGenotype;
			Types::PhenotypeProxy bestPhenotype;
			Types::Fitness bestFitness;
			Types::Genealogy genealogy;
		private:
			template <typename F, typename ... VTypes> void tryExecuteCallback(F f, VTypes ... vargs) { if(f) f(vargs...); };
			void evaluateVariations();
			std::size_t currentGeneration;
			std::size_t nextIdentifier;

	};
	template <typename Types>
	EvolutionaryAlgorithm<Types>::EvolutionaryAlgorithm()
			: lambda(0)
			, currentGeneration(0)
			, nextIdentifier(0)
			, genePoolSelectionFunction(StandardGenePoolSelectors<Types>::all) {
	}
	template <typename Types>
	StepResult EvolutionaryAlgorithm<Types>::epoch() {
		typename Types::Generation newGeneration{};
		if (0 == genealogy.size()) [[unlikely]] {
			auto genotypeProxies(genesisFunction());
			for (auto & genotypeProxy : genotypeProxies) {
				auto newIndividual = std::make_shared<Individual<Types, Types::IndividualParameters>>(currentGeneration, nextIdentifier, genotypeProxy);
				++nextIdentifier;
				newGeneration.push_back(newIndividual);
			}
			variationInfos.push_back({});
		} else [[likely]] {
			++currentGeneration;
			auto genePool = genePoolSelectionFunction(genealogy.back());
			std::list<VariationInfo<Types>> newVariationInfos;
			typename Types::IndividualPtrs newOffsprings{};
			while (newOffsprings.size() < lambda) {
				for (auto const & variationFunctor: variationFunctors) {
					if (newOffsprings.size() >= lambda) {
						break;
					}
					auto variationInfo = variationFunctor.apply(fitnessComparisonFunction, genePool);
					for (auto & parentPtr : variationInfo.parentPtrs) {
						variationInfo.parentIds.push_back(parentPtr->id);
					}
					auto newGenotypes = variationInfo.childProxies;
					for (auto const & newGenotype : newGenotypes) {
						auto newIndividual = std::make_shared<Individual<Types, Types::IndividualParameters>>(currentGeneration, nextIdentifier, newGenotype);
						++nextIdentifier;
						newIndividual->setParents(variationInfo.parentPtrs);
						variationInfo.childIds.push_back(newIndividual->id);
						newOffsprings.emplace_back(newIndividual);
					}
					newVariationInfos.push_back(variationInfo);
					tryExecuteCallback<Types::COnVariation, VariationInfo<Types> const &>(onVariationCallback, variationInfo);
				}
			}
			newGeneration.insert(newGeneration.end(), newOffsprings.begin(), newOffsprings.end());
			newGeneration.insert(newGeneration.end(), genealogy.back().begin(), genealogy.back().end());
			variationInfos.push_back(newVariationInfos);
		}
		genealogy.push_back(newGeneration);
		logger.info("Epoch {}: {} individuals.", currentGeneration, genealogy.back().size());

		std::for_each(genealogy.back().begin(), genealogy.back().end(), [&](auto & iptr) { iptr->maybePhenotypeProxy = transformFunction(iptr->genotypeProxy); });
		genealogy.back().remove_if([&](auto & iptr) { return iptr->isInvalid(); });
		std::for_each(genealogy.back().begin(), genealogy.back().end(), [&](auto & iptr) { iptr->fitness = evaluationFunction(iptr->maybePhenotypeProxy.value()); });
		std::stable_sort(genealogy.back().begin(), genealogy.back().end(), [&](auto & lhs, auto & rhs) { return fitnessComparisonFunction(lhs->fitness, rhs->fitness); });

		if (!variationInfos.back().empty()) {
			evaluateVariations();
		}

		survivorSelectionFunction(genealogy.back());

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

	template <typename Types>
	Types::IndividualPtr EvolutionaryAlgorithm<Types>::find(IndividualIdentifier indId) {
		auto const & gen = indId.generation;
		auto const & id = indId.identifier;
		if (genealogy.size() <= gen) [[unlikely]] {
			return {};
		}
		for (auto & indPtr : genealogy.at(gen)) {
			if (indPtr->id == indId) {
				return indPtr;
			}
		}
		return {};
	}

	template <typename Types>
	void EvolutionaryAlgorithm<Types>::evaluateVariations() {
		auto const & varInfos = variationInfos.back();
		struct VariationStatistics {
			std::size_t success;
			std::size_t fail;
			std::size_t error;
			std::size_t total;
		};
		std::map<std::string, VariationStatistics> varStatMap;
		for (auto const & varInfo : varInfos) {
			if (!varStatMap.contains(varInfo.name)) {
				varStatMap.emplace(std::make_pair(varInfo.name, VariationStatistics{.success = 0, .fail = 0, .error = 0, .total = 0}));
			}
			auto & varStat = varStatMap.at(varInfo.name);
			for (auto const & childId : varInfo.childIds) {
				auto const & child = find(childId);
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
		for (auto const & varStatPair : varStatMap) {
			auto const & varName = varStatPair.first;
			auto const & varStat = varStatPair.second;
			double totalChildren = static_cast<double>(varStat.total);
			double varSuccessRate = static_cast<double>(varStat.success) / totalChildren;
			double varFailureRate = static_cast<double>(varStat.fail) / totalChildren;
			double varErrorRate = static_cast<double>(varStat.error) / totalChildren;
			logger.info("Variation {} : (success, failure, error, total) = ({}, {}, {}, {})", varName, varSuccessRate, varFailureRate, varErrorRate, varStat.total);
		}
	}
}
