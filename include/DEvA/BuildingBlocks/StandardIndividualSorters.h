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
			return lhs->metricMap.at(metricName) < rhs->metricMap.at(metricName);
		}
	};
}