#pragma once

#include <deque>
#include <memory>

#include "Genotype.h"
#include "Phenotype.h"

class Individual;
typedef std::shared_ptr<Individual> IndividualPtr;
typedef std::deque<IndividualPtr> IndividualPtrList;

class Individual {
	public:
		Individual();
	private:
		GenotypePtr genotype;
		PhenotypePtr phenotype;
		IndividualPtrList parents;
		IndividualPtrList children;
		size_t age;
};
