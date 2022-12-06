#pragma once

#include "DEvA/IndividualIdentifier.h"

#include <deque>

template <typename Types>
struct EAGenerationState {
	DEvA::IndividualIdentifiers elderIdentifiers;
	DEvA::IndividualIdentifiers genePoolIdentifiers;
	DEvA::IndividualIdentifiers newbornIdentifiers;
	DEvA::IndividualIdentifiers survivorIdentifiers;
	typename Types::SMetricMap generationMetricMap;
};

template <typename Types>
using EAStates = std::deque<EAGenerationState<Types>>;
