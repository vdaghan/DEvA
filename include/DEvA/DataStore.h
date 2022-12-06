#pragma once

#include "DEvA/EAState.h"
#include "DEvA/Error.h"
#include "DEvA/IndividualIdentifier.h"

#include <filesystem>
#include <map>
#include <set>

namespace DEvA {
	template <typename Types>
	struct Datastore {
			using IndividualFileMap = std::map<DEvA::IndividualIdentifier, std::filesystem::path>;
			using IndividualFileSet = std::set<DEvA::IndividualIdentifier>;

			virtual IndividualFileSet const & scanIndividuals() = 0;
			virtual typename Types::IndividualPtr importIndividual(DEvA::IndividualIdentifier id) = 0;
			virtual void exportIndividual(typename Types::IndividualPtr iptr) = 0;
			virtual void exportGenerationState(EAGenerationState<Types> const &, std::size_t) = 0;
			virtual IndividualFileMap & individualFileMap() = 0;
			virtual IndividualFileSet & individualFileSet() = 0;
	};
}
