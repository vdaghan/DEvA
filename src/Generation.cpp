#include "Generation.h"

Generation::Generation(size_t epoch_) {
	epoch = epoch_;
}

IndividualPtrList Generation::getIndividuals() {
	return individuals;
}

void Generation::addIndividual(IndividualPtr individual) {
	individuals.push_back(individual);
}

std::optional<std::any> Generation::getParameter(std::string name) const {
	if (parameters.contains(name)) {
		std::any retVal = parameters.at(name);
		return retVal;
	}
	return std::nullopt;
}

Parameters Generation::getParameters() const {
	return parameters;
}

void Generation::setParameter(std::string name, std::any value) {
	parameters[name] = value;
}

size_t Generation::getSize() const {
	return individuals.size();
}
