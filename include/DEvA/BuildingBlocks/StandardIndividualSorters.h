#pragma once


#include "DEvA/Common.h"
#include "DEvA/Metric.h"
#include "DEvA/RandomNumberGenerator.h"
#include "DEvA/Specialisation.h"

#include <algorithm>
#include <ranges>
#include <string_view>
#include <vector>

namespace DEvA {
	template <typename Types>
	struct StandardIndividualSorters {
		using IndividualPtr = typename Types::IndividualPtr;
		static bool metricSort(ParameterMap parameters, IndividualPtr lhs, IndividualPtr rhs) {
			std::string metricName(parameters.at("metric").get<std::string>());
			bool isBetterThan(lhs->metricMap.at(metricName).isBetterThan(rhs->metricMap.at(metricName)));
			//bool isEquivalentTo(lhs->metricMap.at(metricName).isEquivalentTo(rhs->metricMap.at(metricName)));
			//bool isOlderThan(lhs->id.generation < rhs->id.generation);
			//return isBetterThan or (isEquivalentTo and isOlderThan);
			return isBetterThan;
		}
	};
}