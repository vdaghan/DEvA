#pragma once

#include "IndividualIdentifier.h"

#include <map>
#include <string>

namespace DEvA {
	struct VariationStatistics {
		std::size_t success;
		std::size_t fail;
		std::size_t error;
		std::size_t total;
	};
	using VariationStatisticsMap = std::map<std::string, VariationStatistics>;

	struct EAProgress {
		std::size_t currentGeneration;
		std::size_t nextIdentifier;
		std::size_t numberOfIndividualsInGeneration;
		IndividualIdentifier bestIndividualId;
	};

	struct EAStatistics {
		EAProgress eaProgress;
		VariationStatisticsMap variationStatisticsMap;
	};
}
