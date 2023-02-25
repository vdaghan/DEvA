#pragma once

#include "DEvA/Metric.h"
#include "DEvA/Specialisation.h"

#include <algorithm>
#include <functional>

namespace DEvA {
	template <typename Types>
	struct StandardSurvivorSelectors {
		static void all(ParameterMap parameters, typename Types::IndividualPtrs& iptrs) {
			return;
		}
		static void clamp(ParameterMap parameters, typename Types::IndividualPtrs & iptrs) {
			std::size_t N(parameters.at("N").get<std::size_t>());
			if (iptrs.size() > N) {
				iptrs.resize(N);
			}
		}
		static void metricComparison(ParameterMap parameters, typename Types::IndividualPtrs& iptrs) {
			std::string metric(parameters.at("metric").get<std::string>());
			std::string comparisonType(parameters.at("comparison").get<std::string>());
			std::any value;
			if (parameters.at("value").is_number_float()) {
				value = parameters.at("value").get<double>();
			} else if (parameters.at("value").is_number_integer()) {
				value = parameters.at("value").get<int64_t>();
			}

			std::function<bool(typename Types::IndividualPtr)> comparisonFunction;

			comparisonFunction = [&](typename Types::IndividualPtr iptr) {
				bool lt(iptr->metricMap.at(metric) < value);
				bool et(iptr->metricMap.at(metric) == value);
				if ("lt" == comparisonType) {
					return lt;
				} else if ("lte" == comparisonType) {
					return lt or et;
				} else if ("gt" == comparisonType) {
					return not (lt or et);
				} else if ("gte" == comparisonType) {
					return not lt;
				} else if ("eq" == comparisonType) {
					return et;
				} else if ("ne" == comparisonType) {
					return not et;
				} else {
					throw std::invalid_argument("Invalid comparison type in EASetup: " + comparisonType);
				}
			};

			bool hasAny = std::any_of(iptrs.begin(), iptrs.end(), [&](auto const & iptr) {
				return comparisonFunction(iptr);
			});
			// GitHub Copilot answers: If there are any survivors satisfying comparison, copy them to a new variable.
			if (hasAny) {
				typename Types::IndividualPtrs survivors{};
				std::copy_if(iptrs.begin(), iptrs.end(), std::back_inserter(survivors), [&](auto const& iptr) {
					return comparisonFunction(iptr);
				});
				iptrs = survivors;
			}
		}
	};
}
