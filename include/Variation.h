#pragma once

#include <any>
#include <functional>
#include <map>
#include <string>

#include "Parameter.h"

enum class VariationArity { UNARY_UNARY, UNARY_NARY, NARY_UNARY, NARY_NARY };
enum class GenericVariation { COPY };

template <class Genotype, class VariationArity>
class Variation {
	public:
		typedef std::function<Genotype(Genotype)> OperatorType;
		Variation(std::string);
		Variation(GenericVariation);

		std::string_view name() { return name; };

		void setOperator(GenericVariation);
		void setOperator(OperatorType);

		Parameters getParameters() const { return parameters; };
		MaybeParameter getParameter(std::string) const;
		void addParameter(Parameter);

		void operator()();
	private:
		std::function<int(int)> variationOperator;
		std::string name;
		bool exhaustive;
		Parameters parameters;
};
