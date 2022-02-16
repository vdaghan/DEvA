#pragma once

#include "BasicTypes.h"
#include "Genealogy.h"
#include "Individual.h"

#include <memory>

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

			void genesis() {
				//genesisFunction();
				genealogy += genesisFunction();
				transformFunction();
				evaluationFunction();
			};
			StepResult step(size_t count) { 
				for (size_t i(0); i < count; ++i) {
					survivorSelectionFunction();
					parentSelectionFunction();
					variationFunction();
					transformFunction();
					evaluationFunction();
					[[unlikely]] if (true) return StepResult::Convergence;
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
