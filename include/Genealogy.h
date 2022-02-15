#pragma once

#include "BasicTypes.h"
#include "Individual.h"

#include <functional>
#include <list>

namespace DEvA {
	template <typename Types>
	struct Genealogy {
		using Generation = Individual<Types>::IndividualPtrSet;
		using IndividualPtr = Individual<Types>::IndividualPtr;
		using Generations = std::list<Generation>;
		using GenesisFunction = std::function<Generation(void)>;

		void operator++() { generations.push_back({}); };
		void operator+=(Generation generation) { generations.push_back(generation); };
		Genealogy& operator<<(IndividualPtr individual) { generations.back()->push_back(individual); return this; };

		size_t epoch() const { return generations.size(); };
		size_t sizeOfLastGeneration() const { return (epoch() > 0) ? generations.back().size() : 0; };

		Generations generations;
	};
}
