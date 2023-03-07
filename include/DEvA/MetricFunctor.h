#pragma once

#include "DEvA/Metric.h"

#include <any>
#include <functional>
#include <string>

namespace DEvA {
    template <typename Types>
    struct MetricFunctor {
        using FComputeFromIndividualPtr = std::function<std::any(typename Types::IndividualPtr const&)>;
        using FComputeFromGeneration = std::function<std::any(typename Types::Generation const&)>;

        std::string name{};
        FComputeFromIndividualPtr computeFromIndividualPtrFunction{};
        FComputeFromGeneration computeFromGenerationFunction{};
        typename Metric<Types>::FMetricEquivalence equivalentToFunction{};
        typename Metric<Types>::FMetricOrdering betterThanFunction{};
        typename Metric<Types>::FMetricToJSONObject metricToJSONObjectFunction{};
        typename Metric<Types>::FJSONObjectToAny JSONObjectToAnyFunction{};

        template <typename T>
        void constructDefaultJSONConverter() {
            metricToJSONObjectFunction = [&](std::any const& value) -> JSON {
                return std::any_cast<T>(value);
            };
        }

        template <typename T>
        void constructAsBasicType(std::string betterThan) {
            equivalentToFunction = [](std::any const & lhs, std::any const & rhs) -> T {
                return std::any_cast<T>(lhs) == std::any_cast<T>(rhs);
            };
            if (betterThan == "greater") {
                betterThanFunction = [](std::any const & lhs, std::any const & rhs) -> T {
                    return std::any_cast<T>(lhs) > std::any_cast<T>(rhs);
                };
            } else if (betterThan == "lesser") {
                betterThanFunction = [](std::any const & lhs, std::any const & rhs) -> T {
					return std::any_cast<T>(lhs) < std::any_cast<T>(rhs);
				};
            }
            
            metricToJSONObjectFunction = [](std::any const& value) -> JSON {
                return std::any_cast<T>(value);
            };
            JSONObjectToAnyFunction = [](JSON const & j) -> std::any {
                return j.get<T>();
			};
        }

        template <typename T>
        [[nodiscard]] Metric<Types> compute(T const & t) const {
            std::any value{};
            if constexpr (std::same_as<T, typename Types::IndividualPtr>) {
                value = computeFromIndividualPtrFunction(t);
            } else if constexpr (std::same_as<T, typename Types::Generation>) {
                value = computeFromGenerationFunction(t);
            }
            Metric<Types> metric{
                .name = name,
                .value = value
            };
            assignFunctions(metric);
            return metric;
        }

        void assignFunctions(Metric<Types>& metric) const {
            metric.equivalentToFunction = equivalentToFunction;
            metric.betterThanFunction = betterThanFunction;
            metric.metricToJSONObjectFunction = metricToJSONObjectFunction;
            metric.JSONObjectToAnyFunction = JSONObjectToAnyFunction;
        }

        //[[nodiscard]] bool valid() const {
        //    bool const hasExactlyOneComparisonFunction(equivalentToFunction.operator bool() xor betterThanFunction.operator bool());
        //    bool const hasExactlyOneComputeFunction(computeFromGenerationFunction.operator bool() xor computeFromIndividualPtrFunction.operator bool());
        //    return hasExactlyOneComparisonFunction and hasExactlyOneComputeFunction;
        //}

        [[nodiscard]] bool valid() const {
            bool const hasAtLeastOneComparisonFunction(equivalentToFunction.operator bool() or betterThanFunction.operator bool());
            bool const hasAtLeastOneComputeFunction(computeFromGenerationFunction.operator bool() or computeFromIndividualPtrFunction.operator bool());
            return hasAtLeastOneComparisonFunction and hasAtLeastOneComputeFunction;
        }
    };
}
