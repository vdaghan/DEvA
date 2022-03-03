#pragma once

#include "Individual.h"

#include <functional>
#include <list>

namespace DEvA {
	/*template <typename Types>
	struct Genealogy {
		void operator++() { generations.push_back({}); };
		void operator+=(Types::Generation generation) { generations.push_back(generation); };
		Genealogy& operator<<(Types::IndividualPtr individual) { generations.back()->push_back(individual); return this; };

		size_t epoch() const { return generations.size(); };
		size_t sizeOfLastGeneration() const { return (epoch() > 0) ? generations.back().size() : 0; };

		Types::Generations generations;
	};*/
}
