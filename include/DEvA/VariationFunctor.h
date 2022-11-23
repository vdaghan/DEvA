#pragma once

#include "DEvA/IndividualIdentifier.h"
#include "DEvA/RandomNumberGenerator.h"

#include <list>
#include <memory>
#include <optional>
#include <random>

namespace DEvA {
	template <typename Spec>
	struct VariationInfo {
		std::string name{};
		typename Spec::IndividualPtrs parentPtrs{};
		IndividualIdentifiers parentIds{};
		typename Spec::GenotypeProxies childProxies{};
		IndividualIdentifiers childIds{};
	};

	struct VariationFunctorParameters {
		double probability;
		bool removeParentsFromMatingPool;
	};

	template <typename Spec>
	struct VariationFunctor {
		Maybe<VariationInfo<Spec>> apply(typename Spec::IndividualPtrs & matingPool) const {
			if (matingPool.size() < numberOfParents) {
				return std::unexpected(ErrorCode::NotEnoughParentsToChoose);
			}
			double p = RandomNumberGenerator::get()->getDouble();
			if (p >= probability) {
				return std::unexpected(ErrorCode::NotApplied);
			}
			VariationInfo<Spec> variationInfo;
			variationInfo.name = name;
			variationInfo.parentPtrs = parentSelectionFunction(matingPool);
			if (variationInfo.parentPtrs.empty()) {
				return std::unexpected(ErrorCode::NoSuitableParentsToChoose);
			}
			if (removeParentsFromMatingPool) {
				for (auto & parentPtr : variationInfo.parentPtrs) {
					matingPool.remove(parentPtr);
				}
			}
			if (variationFunctionFromIndividualPtrs) {
				variationInfo.childProxies = variationFunctionFromIndividualPtrs(matingPool);
			}
			typename Spec::GenotypeProxies parentGenotypes{};
			for (auto & parentPtr : variationInfo.parentPtrs) {
				parentGenotypes.emplace_back(parentPtr->genotypeProxy);
			}
			if (variationFunctionFromGenotypeProxies) {
				variationInfo.childProxies = variationFunctionFromGenotypeProxies(parentGenotypes);
			}
			return variationInfo;
		}

		std::string name{};
		std::size_t numberOfParents{};
		typename Spec::FParentSelection parentSelectionFunction{};
		typename Spec::FVariationFromGenotypeProxies variationFunctionFromGenotypeProxies{};
		typename Spec::FVariationFromIndividualPtrs variationFunctionFromIndividualPtrs{};

		double probability{};
		bool removeParentsFromMatingPool{};
		VariationFunctorParameters parameters{};
	};
}
