#pragma once

#include "DEvA/Metric.h"
#include "DEvA/Specialisation.h"

#include <algorithm>
#include <functional>

namespace DEvA {
	template <typename Types>
	struct StandardSurvivorSelectors {
		using IPtrs = typename Types::IndividualPtrs;
		static IPtrs all(ParameterMap parameters, IPtrs iptrs) {
			return iptrs;
		}
		static IPtrs clamp(ParameterMap parameters, IPtrs iptrs) {
			std::size_t N(parameters.at("N").get<std::size_t>());
			if (iptrs.size() > N) {
				iptrs.resize(N);
			}
			return iptrs;
		}
		static IPtrs metricComparison(ParameterMap parameters, IPtrs iptrs) {
			std::string metric(parameters.at("metric").get<std::string>());

			auto comparisonFunction = [&](typename Types::IndividualPtr iptr) {
				auto & metricObject(iptr->metricMap.at(metric));
				std::any value(metricObject.JSONObjectToAnyFunction(parameters.at("value")));

				Metric<Types> anchor{
					.name = metric,
					.value = value,
					.equivalentToFunction = metricObject.equivalentToFunction,
					.betterThanFunction = metricObject.betterThanFunction,
					.metricToJSONObjectFunction = metricObject.metricToJSONObjectFunction,
					.JSONObjectToAnyFunction = metricObject.JSONObjectToAnyFunction
				};
				return metricObject.isBetterThan(anchor);
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
			return iptrs;
		}
	};
}
