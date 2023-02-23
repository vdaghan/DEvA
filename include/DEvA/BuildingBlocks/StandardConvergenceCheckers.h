#pragma once

#include "DEvA/Specialisation.h"

namespace DEvA {
	template <typename Types>
	struct StandardConvergenceCheckers {
		static bool equalTo(ParameterMap parameters, typename Types::SMetricMap metricMap) {
			std::string metricName(parameters.at("metric").get<std::string>());
			auto & metric(metricMap.at(metricName));
			if (parameters.at("N").is_number_float()) {
				auto N(parameters.at("N").get<float>());
				return metric.as<float>() == N;
			}
			else if (parameters.at("N").is_number_integer()) {
				auto N(parameters.at("N").get<int>());
				return metric.as<int>() == N;
			}
			return false;
		}
		static bool lessThan(ParameterMap parameters, typename Types::SMetricMap metricMap) {
			std::string metricName(parameters.at("metric").get<std::string>());
			auto & metric(metricMap.at(metricName));
			if (parameters.at("N").is_number_float()) {
				auto N(parameters.at("N").get<float>());
				return metric.as<float>() < N;
			}
			else if (parameters.at("N").is_number_integer()) {
				auto N(parameters.at("N").get<int>());
				return metric.as<int>() < N;
			}
			return false;
		}
		static bool biggerThan(ParameterMap parameters, typename Types::SMetricMap metricMap) {
			std::string metricName(parameters.at("metric").get<std::string>());
			auto & metric(metricMap.at(metricName));
			if (parameters.at("N").is_number_float()) {
				auto N(parameters.at("N").get<float>());
				return metric.as<float>() > N;
			} else if (parameters.at("N").is_number_integer()) {
				auto N(parameters.at("N").get<int>());
				return metric.as<int>() > N;
			}
			return false;
		}
		static bool never(ParameterMap parameters, typename Types::SMetricMap metricMap) {
			return false;
		}
	};
}
