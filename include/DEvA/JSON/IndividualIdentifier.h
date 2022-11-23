#pragma once

#include "DEvA/JSON/Common.h"
#include "DEvA/IndividualIdentifier.h"

template <>
struct nlohmann::adl_serializer<DEvA::IndividualIdentifier> {
    static DEvA::IndividualIdentifier from_json(JSON const & j) {
        std::size_t generation{};
        std::size_t identifier{};
        j.at("generation").get_to(generation);
        j.at("identifier").get_to(identifier);
        DEvA::IndividualIdentifier const individualIdentifier{
                    .generation = generation,
                    .identifier = identifier
                };
        return individualIdentifier;
    }

    static void to_json(JSON & j, DEvA::IndividualIdentifier const & individualIdentifier) {
        j["generation"] = individualIdentifier.generation;
        j["identifier"] = individualIdentifier.identifier;
    }
};
