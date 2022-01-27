#pragma once

#include "Genotype.h"
#include "Phenotype.h"

#include <vector>

class Individual {
	public:
		Individual();
	private:
		Genotype genotype;
		Phenotype phenotype;
		std::vector<Individual> parents;
		size_t age;
};
