#pragma once

#include "DEvA/JSON/Common.h"
#include "DEvA/Metric.h"

#include <any>
#include <string>

template <typename Types>
struct nlohmann::adl_serializer<DEvA::MetricMap<Types>> {
    static DEvA::MetricMap<Types> from_json(JSON const & j) {
        DEvA::MetricMap<Types> metricMap{};
        return metricMap;
    }

    static void to_json(JSON & j, DEvA::MetricMap<Types> const & metricMap) {
        for (auto const & [metricName, metricObject] : metricMap) {
            j[metricName] = metricObject.metricToJSONObjectFunction(metricObject.value);
        }
    }
};
