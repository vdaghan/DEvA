#pragma once

#include <DEvA/Metric.h>
#include <DEvA/MetricFunctor.h>
#include <DEvA/MetricFunctors.h>
#include <DEvA/JSON/Common.h>

namespace DEvA {
	template <typename Types>
	void importMetricFunctors(JSON & j, MetricFunctors<Types> & metricFunctors) {
		if (not j.contains("metricFunctors")) {
			return;
		}
		auto & metricFunctorsJSON(j.at("metricFunctors"));
		if (metricFunctorsJSON.contains("definitions")) {
			auto & definitions(metricFunctorsJSON.at("definitions"));
			for (auto definitionIt = definitions.begin(); definitionIt != definitions.end(); ++definitionIt) {
				std::string functorName(definitionIt.key());
				auto & parameters(definitionIt.value());
				metricFunctors.defineMetricFunctor(functorName, parameters);
			}
		}
		if (metricFunctorsJSON.contains("usedForIndividuals")) {
			auto & usedMetricFunctors(metricFunctorsJSON.at("usedForIndividuals"));
			for (auto & metricFunctor : usedMetricFunctors) {
				metricFunctors.useForIndividuals({ metricFunctor });
			}
		}
	}
}
