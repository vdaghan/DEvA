#pragma once

#include "BasicTypes.h"
#include "Genealogy.h"
#include "Individual.h"

#include <memory>

namespace DEvA {
	enum class TerminationReason { StepCount, Convergence };

	template <typename Types>
	struct EvolutionaryAlgorithm {
		using SGenealogy = Genealogy<Types>;
		using GenesisFunction = SGenealogy::GenesisFunction;

		void genesis(GenesisFunction gfunc) { genealogy += gfunc(); };
		TerminationReason step() { return TerminationReason::Convergence; };

		SGenealogy genealogy;
	};
}
