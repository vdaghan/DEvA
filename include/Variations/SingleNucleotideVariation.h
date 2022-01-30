#pragma once

#include "Genotype.h"

class SingleNucleotideVariation {
	public:
		virtual GenotypePtr operator()(GenotypePtr);
	private:
};

typedef SingleNucleotideVariation SNV;
