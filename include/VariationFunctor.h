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
			VariationInfo<Spec> apply(Spec::IndividualPtrs & matingPool) {
				double probability = RandomNumberGenerator::get()->getDouble();
				if (probability <= m_probability) {
					return {};
				}
				VariationInfo<Spec> variationInfo;
				variationInfo.parents = m_parentSelectionFunction(matingPool);
				if (m_removeParentsFromMatingPool) {
					for (auto it(variationInfo.parents.begin()); it != variationInfo.parents.end(); ++it) {
						matingPool.remove(*it);
					}
				}
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
