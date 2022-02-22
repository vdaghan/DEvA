#pragma once

#include <list>
#include <memory>

#include "BasicTypes.h"

namespace DEvA {
	template <typename Types>
	class Individual : public std::enable_shared_from_this<Individual<Types>> {
		public:
			using IndividualPtr = Types::IndividualPtr;
			using IndividualPtrs = Types::IndividualPtrs;

			Individual(Types::GenotypePtr gptr) {
				genotype = gptr;
			};
			void setParents(IndividualPtrs parents_) {
				parents = parents_;
				for (auto it = parents.begin(); it != parents.end(); ++it) {
					(* it)->children.push_back(this->shared_from_this());
				}
			}
			// TODO: Get rid of this
			Individual<Types>& operator=(Individual<Types>& ind) {
				genotype = ind.genotype;
				phenotype = ind.phenotype;
				fitness = ind.fitness;
				parents = ind.parents;
				children = ind.children;
				return *this;
			};


			Types::GenotypePtr genotype;
			Types::PhenotypePtr phenotype;
			Types::Fitness fitness;
			Types::IndividualPtrs parents;
			Types::IndividualWPtrs children;
	};
}
