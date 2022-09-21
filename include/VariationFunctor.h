#pragma once

#include "RandomNumberGenerator.h"

#include <list>
#include <memory>
#include <random>

namespace DEvA {
	template <typename Spec>
	struct VariationInfo {
		Spec::IndividualPtrs parents;
		Spec::GenotypeProxies children;
	};

	template <typename Spec>
	class VariationFunctor {
		public:
			VariationInfo<Spec> apply(Spec::FFitnessComparison comp, Spec::IndividualPtrs & matingPool) const {
				double probability = RandomNumberGenerator::get()->getDouble();
				if (probability >= m_probability) {
					return {};
				}
				VariationInfo<Spec> variationInfo;
				variationInfo.parents = m_parentSelectionFunction(comp, matingPool);
				if (m_removeParentsFromMatingPool) {
					for (auto it(variationInfo.parents.begin()); it != variationInfo.parents.end(); ++it) {
						matingPool.remove(*it);
					}
				}
				typename Spec::GenotypeProxies parentGenotypes{};
				for (auto it = variationInfo.parents.begin(); it != variationInfo.parents.end(); ++it) {
					parentGenotypes.push_back((*it)->genotypeProxy);
				}
				variationInfo.children = m_variationFunction(parentGenotypes);
				return variationInfo;
			}
			void setProbability(double p) { m_probability = p; };
			void setParentSelectionFunction(Spec::FParentSelection f) { m_parentSelectionFunction = f; };
			void setVariationFunction(Spec::FVariation f) { m_variationFunction = f; };
			void setRemoveParentFromMatingPool(bool r) { m_removeParentsFromMatingPool = r; };
		private:
			double m_probability;
			Spec::FParentSelection m_parentSelectionFunction;
			Spec::FVariation m_variationFunction;
			bool m_removeParentsFromMatingPool;
	};
}
