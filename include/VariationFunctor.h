#pragma once

#include "IndividualIdentifier.h"
#include "RandomNumberGenerator.h"

#include <list>
#include <memory>
#include <random>

namespace DEvA {
	template <typename Spec>
	struct VariationInfo {
		std::string name;
		Spec::IndividualPtrs parentPtrs;
		IndividualIdentifiers parentIds;
		Spec::GenotypeProxies childProxies;
		IndividualIdentifiers childIds;
	};

	struct VariationFunctorParameters {
		double probability;
		bool removeParentsFromMatingPool;
	};

	template <typename Spec>
	struct VariationFunctor {
		Maybe<VariationInfo<Spec>> apply(Spec::FFitnessComparison comp, Spec::IndividualPtrs & matingPool) const {
			double p = RandomNumberGenerator::get()->getDouble();
			if (p >= probability) {
				return VariationInfo<Spec>{.name="", .parentPtrs={}, .parentIds={}, .childProxies={}, .childIds={}};
			}
			VariationInfo<Spec> variationInfo;
			variationInfo.name = name;
			variationInfo.parentPtrs = parentSelectionFunction(comp, matingPool);
			if (removeParentsFromMatingPool) {
				for (auto it(variationInfo.parentPtrs.begin()); it != variationInfo.parentPtrs.end(); ++it) {
					matingPool.remove(*it);
				}
			}
			typename Spec::GenotypeProxies parentGenotypes{};
			for (auto it = variationInfo.parentPtrs.begin(); it != variationInfo.parentPtrs.end(); ++it) {
				parentGenotypes.push_back((*it)->genotypeProxy);
			}
			variationInfo.childProxies = variationFunction(parentGenotypes);
			return variationInfo;
		}

		std::string name;
		Spec::FParentSelection parentSelectionFunction;
		Spec::FVariation variationFunction;

		double probability;
		bool removeParentsFromMatingPool;
		VariationFunctorParameters parameters;
	};
}
