#pragma once

#include "Genotype.h"

class CrossoverVariation {
	public:
		GenotypePtr operator()(GenotypePtr, GenotypePtr);
	private:
};
