#pragma once

#include "IndividualIdentifier.h"

#include <deque>
#include <list>
#include <map>
#include <mutex>
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
		std::size_t numberOfNewIndividualsInGeneration;
		std::size_t numberOfOldIndividualsInGeneration;
		std::size_t numberOfIndividualsInGeneration;
		std::size_t numberOfTransformedIndividualsInGeneration;
		std::size_t numberOfInvalidIndividualsInGeneration;
		std::size_t numberOfEvaluatedIndividualsInGeneration;
	};

	enum class EAStatisticsUpdateType {Progress, Variation, Fitness, Distance, Final};
	template <typename Types>
	class EAStatistics {
		public:
			EAStatistics()
				: eaProgress({})
				, fitnesses({})
				, distanceMatrix({})
				, variationStatisticsMap({}) {};
			EAStatistics(EAStatistics const& other)
				: eaProgress(other.eaProgress)
				, fitnesses(other.fitnesses)
				, distanceMatrix(other.distanceMatrix)
				, variationStatisticsMap(other.variationStatisticsMap) {};

			EAProgress eaProgress;
			std::list<typename Types::Fitness> fitnesses;
			typename Types::DistanceMatrix distanceMatrix;
			VariationStatisticsMap variationStatisticsMap;

			std::unique_lock<std::mutex> lock() const { return std::unique_lock(accessMutex); };
		private:
			mutable std::mutex accessMutex;
	};
}
