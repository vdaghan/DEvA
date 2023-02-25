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
        }
        [[nodiscard]] bool isOrdering() const {
            return betterThanFunction.operator bool();
        }
        [[nodiscard]] bool relationTypesMatch(Metric const & other) const {
            bool const bothAreEquivalence(isEquivalence() and other.isEquivalence());
            bool const bothAreOrdering(isOrdering() and other.isOrdering());
            bool const consistent((isOrdering() and not other.isEquivalence()) or (isEquivalence() and not other.isOrdering()));
            return (bothAreEquivalence or bothAreOrdering) and consistent;
        }
        [[nodiscard]] bool comparableInTheory(Metric const & other) const {
            bool const namesMatch(name == other.name);
            bool const typesMatch(value.type() == other.value.type());
            return namesMatch and typesMatch and relationTypesMatch(other);
        }
        [[nodiscard]] bool comparableWith(Metric const & other) const {
            bool const hasValue(value.has_value());
            bool const otherHasValue(other.value.has_value());
            bool const comparable(comparableInTheory(other));
            return hasValue and otherHasValue and comparable;
        }
        template <typename T>
        [[nodiscard]] T as() const {
            return std::any_cast<T>(value);
        }
        [[nodiscard]] bool operator<(Metric const & otherMetric) const {
            return betterThanFunction(value, otherMetric.value);
        }
        [[nodiscard]] bool operator<(std::any & otherValue) const {
            return betterThanFunction(value, otherValue);
        }
        [[nodiscard]] bool operator==(Metric const & otherMetric) const {
            return equivalentToFunction(value, otherMetric.value);
        }
        [[nodiscard]] bool operator==(std::any & otherValue) const {
            return equivalentToFunction(value, otherValue);
        }
    };

    template <typename Types>
    using MetricMap = std::map<std::string, Metric<Types>>;
}
