#pragma once

#include "VParameters.h"

#include <list>
#include <memory>

namespace DEvA {
	template <typename Types, typename VP>
	class Individual : public std::enable_shared_from_this<Individual<Types, VP>> {
		public:
			Individual(Types::GenotypePtr gptr) {
				genotype = gptr;
			};
			void setParents(Types::IndividualPtrs parents_) {
				parents = parents_;
				for (auto it = parents.begin(); it != parents.end(); ++it) {
					(* it)->children.push_back(this->shared_from_this());
				}
			}
			// TODO: Get rid of this
			Individual<Types, VP>& operator=(Individual<Types, VP>& ind) {
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
		private:
			VP vparameters;
	};
}
