#include "Variation.h"

template<class Genotype>
Variation<Genotype>::Variation(std::string name_) {
	name = name_;
}

template<class Genotype, int InputArity, int OutputArity>
Variation<Genotype, InputArity, OutputArity>::Variation(GenericVariation) {
}

template<class Genotype>
void Variation<Genotype>::setOperator(GenericVariation variation) {
	if (GenericVariation::COPY == variation) {

	}
}

template<class Genotype>
MaybeParameter Variation<Genotype>::getParameter(std::string) const {
	return MaybeParameter();
}
