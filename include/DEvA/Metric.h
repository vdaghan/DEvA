#pragma once

#include "DEvA/JSON/Common.h"

#include <any>
#include <functional>
#include <string>

namespace DEvA {
    template <typename Types>
    struct Metric {
        using FMetricEquivalence = std::function<bool(std::any const &, std::any const &)>;
        using FMetricOrdering = std::function<bool(std::any const &, std::any const &)>;
        using FMetricToJSONObject = std::function<JSON(std::any const &)>;

        std::string const name;
        std::any const value;
        FMetricEquivalence equivalentToFunction;
        FMetricOrdering betterThanFunction;
        FMetricToJSONObject metricToJSONObjectFunction;

        [[nodiscard]] bool isEquivalence() const {
            return equivalentToFunction.operator bool();
        };
        [[nodiscard]] bool isOrdering() const {
            return betterThanFunction.operator bool();
        };
        bool relationTypesMatch(Metric const & other) const {
            bool bothAreEquivalence(isEquivalence() and other.isEquivalence());
            bool bothAreOrdering(isOrdering() and other.isOrdering());
            bool consistent((isOrdering() and not other.isEquivalence()) or (isEquivalence() and not other.isOrdering()));
            return (bothAreEquivalence or bothAreOrdering) and consistent;
        };
        bool comparableInTheory(Metric const & other) const {
            bool namesMatch(name == other.name);
            bool typesMatch(value.type() == other.value.type());
            return namesMatch and typesMatch and relationTypesMatch(other);
        };
        bool comparableWith(Metric const & other) const {
            bool hasValue(value.has_value());
            bool otherHasValue(other.value.has_value());
            bool comparable(comparableInTheory(other));
            return hasValue and otherHasValue and comparable;
        };
        template <typename T>
        T as() const {
            return std::any_cast<T>(value);
        }
        bool operator<(Metric const & otherMetric) const {
            return betterThanFunction(value, otherMetric.value);
        };
        bool operator==(Metric const & otherMetric) const {
            return equivalentToFunction(value, otherMetric.value);
        };
    };

    template <typename Types>
    using MetricMap = std::map<std::string, Metric<Types>>;

    template <typename Types>
    struct MetricFunctor {
        using FComputeFromIndividualPtr = std::function<std::any(typename Types::IndividualPtr const &)>;
        using FComputeFromGeneration = std::function<std::any(typename Types::Generation const &)>;

        std::string name{};
        FComputeFromIndividualPtr computeFromIndividualPtrFunction{};
        FComputeFromGeneration computeFromGenerationFunction{};
        typename Metric<Types>::FMetricEquivalence equivalentToFunction{};
        typename Metric<Types>::FMetricOrdering betterThanFunction{};
        typename Metric<Types>::FMetricToJSONObject metricToJSONObjectFunction{};

        template <typename T>
        void constructDefaultJSONConverter() {
            metricToJSONObjectFunction = [&](std::any const & value) -> JSON {
                return std::any_cast<T>(value);
            };
        }

        template <typename T>
        Metric<Types> compute(T const & t) const {
            std::any value{};
            if constexpr (std::same_as<T, typename Types::IndividualPtr>) {
                value = computeFromIndividualPtrFunction(t);
            } else if constexpr (std::same_as<T, typename Types::Generation>) {
                value = computeFromGenerationFunction(t);
            } else {
                throw;
            }
            Metric<Types> metric{
                .name = name,
                .value = value
            };
            assignFunctions(metric);
            return metric;
        }

        void assignFunctions(Metric<Types> & metric) const {
            metric.equivalentToFunction = equivalentToFunction;
            metric.betterThanFunction = betterThanFunction;
            metric.metricToJSONObjectFunction = metricToJSONObjectFunction;
        }
        
        [[nodiscard]] bool valid() const {
            bool hasExactlyOneComparisonFunction(equivalentToFunction.operator bool() xor betterThanFunction.operator bool());
            bool hasExactlyOneComputeFunction(computeFromGenerationFunction.operator bool() xor computeFromIndividualPtrFunction.operator bool());
            return hasExactlyOneComparisonFunction and hasExactlyOneComputeFunction;
        }
    };
}
