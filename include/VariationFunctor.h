#pragma once

#include "RandomNumberGenerator.h"

#include <list>
#include <memory>
#include <random>

namespace DEvA {
	template <typename Spec>
	struct VariationInfo {
		std::string name;
		Spec::IndividualPtrs parents;
		Spec::GenotypeProxies children;
	};

	template <typename Spec>
	struct VariationFunctor {
		VariationInfo<Spec> apply(Spec::FFitnessComparison comp, Spec::IndividualPtrs & matingPool) const {
			double p = RandomNumberGenerator::get()->getDouble();
			if (p >= probability) {
				return {};
			}
			VariationInfo<Spec> variationInfo;
			variationInfo.name = name;
			variationInfo.parents = parentSelectionFunction(comp, matingPool);
			if (removeParentsFromMatingPool) {
				for (auto it(variationInfo.parents.begin()); it != variationInfo.parents.end(); ++it) {
					matingPool.remove(*it);
				}
			}
			typename Spec::GenotypeProxies parentGenotypes{};
			for (auto it = variationInfo.parents.begin(); it != variationInfo.parents.end(); ++it) {
				parentGenotypes.push_back((*it)->genotypeProxy);
			}
			variationInfo.children = variationFunction(parentGenotypes);
			return variationInfo;
		}

		std::string name;
		double probability;
		Spec::FParentSelection parentSelectionFunction;
		Spec::FVariation variationFunction;
		bool removeParentsFromMatingPool;
	};
}
