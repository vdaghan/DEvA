#pragma once

#include "BasicTypes.h"
#include "Parameters.h"
#include "Variations/StandardVariations.h"

#include <any>
#include <functional>
#include <string>

namespace DEvA {
	enum class VariationFlags { Exhaustive = 1
								 };

	template <typename Types>
	struct Variation {
		using Genotype = Types::Genotype;
		using GenotypePtr = Types::GenotypePtr;
		using GenotypePtrs = Types::GenotypePtrs;
		using InputOperator = std::function<GenotypePtrs(GenotypePtrs)>;
		using VariationOperator = std::function<GenotypePtrs(GenotypePtrs)>;

		Variation(std::string name_, VariationOperator variationOperator_) {
			name = name_;
			variationOperator = variationOperator_;
		};
		GenotypePtrs operator()(GenotypePtrs gptrs) { return variationOperator(gptrs); };

		std::string name;

		InputOperator inputOperator;
		VariationOperator variationOperator;
		bool hasOperator();
		Parameters parameters;
	};
}
