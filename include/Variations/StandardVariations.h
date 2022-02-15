#pragma once

#include "Specialisation.h"
#include "Common.h"

#include <algorithm>
#include <memory>
#include <random>

namespace DEvA {
	template <typename Types>
	struct StandardVariations {
		using Genotype = Types::Genotype;
		using GenotypePtr = Types::GenotypePtr;
		using GenotypePtrList = Types::GenotypePtrList;

		static GenotypePtr copy(GenotypePtr gptr) {
			return std::make_shared<Genotype>(*gptr);
		}

		static GenotypePtrList cutAndCrossfill(GenotypePtrList gptrlist) {
			//static_assert(2 == gptrlist.size());
			GenotypePtr first = std::make_shared<Genotype>(*gptrlist.front());
			GenotypePtr second = std::make_shared<Genotype>(*gptrlist.back());
			//static_assert(first->size() == second->size());
			size_t genotypeSize = first->size();

			std::default_random_engine randGen;
			std::uniform_int_distribution<int> distribution(0, genotypeSize - 1);

			size_t crossoverIndex = distribution(randGen);

			auto it1 = Common::iteratorForNthListElement(*first, crossoverIndex);
			auto it2 = Common::iteratorForNthListElement(*second, crossoverIndex);

			for (++it1, ++it2; it1 != first->end() or it2 != second->end(); ++it1, ++it2) {
				std::iter_swap(it1, it2);
			}

			GenotypePtrList retList = { first, second };
			return retList;
		}
	};
}
