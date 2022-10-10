#pragma once

#include "IndividualIdentifier.h"

#include <deque>
#include <map>
#include <string>

namespace DEvA {
	struct VariationStatistics {
		std::size_t success;
		std::size_t fail;
		std::size_t error;
		std::size_t total;
		double successRate() { return double(success) / double(total); };
		double failureRate() { return double(fail) / double(total); };
		double errorRate() { return double(error) / double(total); };
	};
	using VariationStatisticsMap = std::map<std::string, VariationStatistics>;

	struct EAProgress {
		std::size_t currentGeneration;
		std::size_t nextIdentifier;
		std::size_t numberOfIndividualsInGeneration;
		std::size_t numberOfTransformedIndividualsInGeneration;
		std::size_t numberOfEvaluatedIndividualsInGeneration;
	};

	struct EAStatistics {
		EAProgress eaProgress;
		VariationStatisticsMap variationStatisticsMap;
	};
	using EAStatisticsHistory = std::deque<EAStatistics>;
}
