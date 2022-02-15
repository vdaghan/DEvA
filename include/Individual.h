#pragma once

#include <memory>
#include <unordered_set>

#include "BasicTypes.h"

namespace DEvA {
	template <typename Types>
	class Individual {
		public:
			using IndividualPtr = std::shared_ptr<Individual<Types>>;
			using IndividualPtrSet = std::unordered_set<IndividualPtr>;

			Individual(IndividualPtrSet parents_, Types::GenotypePtr gptr) {
				parents = parents_;
				genotype = gptr;
				for (auto it = parents.begin(); it != parents.end(); ++it) {
					it->children.emplace_back(std::make_shared<Individual<Types>>(this));
				}
			};
			Individual(Types::GenotypePtr gptr) {
				genotype = gptr;
			};

			Types::GenotypePtr genotype;
			Types::PhenotypePtr phenotype;
			Types::Fitness fitness;
			IndividualPtrSet parents;
			IndividualPtrSet children;
	};
}
