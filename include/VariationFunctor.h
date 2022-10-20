#pragma once

#include "IndividualIdentifier.h"
#include "RandomNumberGenerator.h"

#include <list>
#include <memory>
#include <optional>
#include <random>

namespace DEvA {
	template <typename Spec>
	struct VariationInfo {
		std::string name{};
		Spec::IndividualPtrs parentPtrs{};
		IndividualIdentifiers parentIds{};
		Spec::GenotypeProxies childProxies{};
		IndividualIdentifiers childIds{};
	};

	struct VariationFunctorParameters {
		double probability;
		bool removeParentsFromMatingPool;
	};

	template <typename Spec>
	struct VariationFunctor {
		Maybe<VariationInfo<Spec>> apply(Spec::FFitnessComparison comp, Spec::IndividualPtrs & matingPool) const {
			if (matingPool.size() < numberOfParents) {
				return std::unexpected(ErrorCode::NotEnoughParentsToChoose);
			}
			double p = RandomNumberGenerator::get()->getDouble();
			if (p >= probability) {
				return std::unexpected(ErrorCode::NotApplied);
			}
			VariationInfo<Spec> variationInfo;
			variationInfo.name = name;
			variationInfo.parentPtrs = parentSelectionFunction(comp, matingPool);
			if (removeParentsFromMatingPool) {
				for (auto & parentPtr : variationInfo.parentPtrs) {
					matingPool.remove(parentPtr);
				}
			}
			typename Spec::GenotypeProxies parentGenotypes{};
			for (auto & parentPtr : variationInfo.parentPtrs) {
				parentGenotypes.emplace_back(parentPtr->genotypeProxy);
			}
			variationInfo.childProxies = variationFunction(parentGenotypes);
			return variationInfo;
		}

		std::string name{};
		std::size_t numberOfParents{};
		Spec::FParentSelection parentSelectionFunction{};
		Spec::FVariation variationFunction{};

		double probability{};
		bool removeParentsFromMatingPool{};
		VariationFunctorParameters parameters{};
	};
}
