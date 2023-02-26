#pragma once

#include <DEvA/Common.h>
#include <DEvA/Functions.h>
#include <DEvA/MetricFunctor.h>
#include "DEvA/BuildingBlocks/Dependency/Dependency.h"

#include <any>
#include <initializer_list>
#include <list>
#include <map>
#include <set>
#include <string>
#include <type_traits>
#include <utility>

namespace DEvA {
	template <typename Types>
	struct MetricFunctors {
		void defineMetricFunctor(std::string name, ParameterMap parameters) {
			functors.emplace(std::make_pair(name, MetricFunctor<Types>{}));
			auto & functor(functors.at(name));
			functor.name = name;

			if (parameters.contains("type")) {
				auto basicType = parameters.at("type").get<std::string>();
				std::string betterThan{};
				if (parameters.contains("basicBetterThan")) {
					betterThan = parameters.at("basicBetterThan").get<std::string>();
				}
				if (basicType == "int") {
					functor.constructAsBasicType<int>(betterThan);
				} else if (basicType == "float") {
					functor.constructAsBasicType<float>(betterThan);
				} else if (basicType == "double") {
					functor.constructAsBasicType<double>(betterThan);
				}
			}
			if (parameters.contains("equivalence")) {
				auto equivalenceFunctionName = parameters.at("equivalence").get<std::string>();
				dependencies.emplace_back(Dependency{
					.condition = [&, equivalenceFunctionName]() { return equivalences.contains(equivalenceFunctionName); },
					.action = [&, equivalenceFunctionName]() { functor.equivalentToFunction = equivalences.at(equivalenceFunctionName); }
				});
			}
			if (parameters.contains("betterThan")) {
				auto orderingFunctionName = parameters.at("betterThan").get<std::string>();
				dependencies.emplace_back(Dependency{
					.condition = [&, orderingFunctionName]() { return orderings.contains(orderingFunctionName); },
					.action = [&, orderingFunctionName]() { functor.betterThanFunction = orderings.at(orderingFunctionName); }
				});
			}
			if (parameters.contains("metricToJSON")) {
				auto metricToJSONFunctionName = parameters.at("metricToJSON").get<std::string>();
				dependencies.emplace_back(Dependency{
					.condition = [&, metricToJSONFunctionName]() { return metricToJSONObjectFunctions.contains(metricToJSONFunctionName); },
					.action = [&, metricToJSONFunctionName]() { functor.metricToJSONObjectFunction = metricToJSONObjectFunctions.at(metricToJSONFunctionName); }
				});
			}
			if (parameters.contains("computeFromIndividualPtr")) {
				auto computeFromIndividualPtrFunctionName = parameters.at("computeFromIndividualPtr").get<std::string>();
				dependencies.emplace_back(Dependency{
					.condition = [&, computeFromIndividualPtrFunctionName]() {
						return computeFromIndividualPtrFunctions.contains(computeFromIndividualPtrFunctionName);
					},
					.action = [&, computeFromIndividualPtrFunctionName]() {
						functor.computeFromIndividualPtrFunction = computeFromIndividualPtrFunctions.at(computeFromIndividualPtrFunctionName);
					}
				});
			}
		};

		void useForIndividuals(std::initializer_list<std::string> names) {
			for (auto & name : names) {
				usedIndividualMetricFunctors.emplace(name);
			}
		};

		std::map<std::string, typename Metric<Types>::FMetricEquivalence> equivalences;
		std::map<std::string, typename Metric<Types>::FMetricOrdering> orderings;
		std::map<std::string, typename Metric<Types>::FMetricToJSONObject> metricToJSONObjectFunctions;
		std::map<std::string, typename MetricFunctor<Types>::FComputeFromIndividualPtr> computeFromIndividualPtrFunctions;
		std::map<std::string, typename MetricFunctor<Types>::FComputeFromGeneration> computeFromGenerationFunctions;
		
		std::map<std::string, MetricFunctor<Types>> functors;
		std::set<std::string> usedIndividualMetricFunctors;

		Dependencies dependencies{};
	};
}
