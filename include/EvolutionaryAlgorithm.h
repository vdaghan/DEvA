#pragma once

#include "BasicTypes.h"
#include "Genealogy.h"
#include "Individual.h"

#include <algorithm>
#include <memory>
#include <ranges>

namespace DEvA {
	enum class StepResult { StepCount, Convergence };

	template <typename Types>
	class EvolutionaryAlgorithm {
		public:
			using SGenealogy = Genealogy<Types>;
			using GenesisFunction = SGenealogy::GenesisFunction;
			using TransformFunction = Types::FTransform;
			using EvaluationFunction = Types::FEvaluate;
			using SurvivorSelectionFunction = SGenealogy::GenesisFunction;
			using ParentSelectionFunction = SGenealogy::GenesisFunction;
			using VariationFunction = SGenealogy::GenesisFunction;

			void setGenesisFunction(GenesisFunction gfunc) { genesisFunction = gfunc; };
			void setTransformFunction(TransformFunction tfunc) { transformFunction = tfunc; };
			void setEvaluationFunction(EvaluationFunction efunc) { evaluationFunction = efunc; };
			void setSurvivorSelectionFunction(SurvivorSelectionFunction ssfunc) { survivorSelectionFunction = ssfunc; };
			void setParentSelectionFunction(ParentSelectionFunction psfunc) { parentSelectionFunction = psfunc; };
			void setVariationFunction(VariationFunction vfunc) { variationFunction = vfunc; };

			StepResult search(size_t count) {
				for (size_t i(0); i < count; ++i) {
					if (0 == genealogy.generations.size()) [[unlikely]] {
						genealogy += genesisFunction();
					} else [[likely]] {
						// Parent selection
						typename SGenealogy::Generation newGeneration{};
						// Variation
					}
					auto& lastGen(genealogy.generations.back());
					// Transform
					std::for_each(lastGen.begin(), lastGen.end(), [&](auto& iptr) { iptr->phenotype = transformFunction(iptr->genotype);});
					// Evaluate
					std::for_each(lastGen.begin(), lastGen.end(), [&](auto& iptr) { iptr->fitness = evaluationFunction(iptr->phenotype);});
					std::stable_sort(lastGen.begin(), lastGen.end(), [](auto iptr1, auto iptr2) -> bool { return ((iptr1->fitness) < (iptr2->fitness)); });

					// Survivor selection
					if (1 != genealogy.generations.size()) [[likely]] {
						//lastGen | std::ranges::drop_view(2);
					}
					if (false) [[unlikely]] {
						return StepResult::Convergence;
					}
				}
				return StepResult::StepCount;
			};

			SGenealogy genealogy;
	private:
		GenesisFunction genesisFunction;
		TransformFunction transformFunction;
		EvaluationFunction evaluationFunction;
		SurvivorSelectionFunction survivorSelectionFunction;
		ParentSelectionFunction parentSelectionFunction;
		VariationFunction variationFunction;
	};
}
