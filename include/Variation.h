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
		using GenotypePtrSet = Types::GenotypePtrSet;
		using InputOperator = std::function<GenotypePtrSet(GenotypePtrSet)>;
		using VariationOperator = std::function<GenotypePtrSet(GenotypePtrSet)>;

		Variation(std::string name_, VariationOperator variationOperator_) {
			name = name_;
			variationOperator = variationOperator_;
		}
		GenotypePtrSet operator()(GenotypePtrSet gptrset) { return variationOperator(gptrset); };

		std::string name;

		InputOperator inputOperator;
		VariationOperator variationOperator;
		bool hasOperator();
		Parameters parameters;
	};
}
