#pragma once

#include "DEvA/EAState.h"
#include "DEvA/Error.h"
#include "DEvA/Individual.h"
#include "DEvA/JSON/Common.h"
#include "DEvA/JSON/IndividualIdentifier.h"

template <typename Types>
struct nlohmann::adl_serializer<EAGenerationState<Types>> {
    static EAGenerationState<Types> from_json(JSON const & j) {
		EAGenerationState<Types> state{};
        if (j.contains("elderIdentifiers")) {
            j.at("elderIdentifiers").get_to(state.elderIdentifiers);
        }
        if (j.contains("genePoolIdentifiers")) {
            j.at("genePoolIdentifiers").get_to(state.genePoolIdentifiers);
        }
        if (j.contains("newbornIdentifiers")) {
            j.at("newbornIdentifiers").get_to(state.newbornIdentifiers);
        }
        if (j.contains("survivorIdentifiers")) {
            j.at("survivorIdentifiers").get_to(state.survivorIdentifiers);
        }
        return state;
    }

    static void to_json(JSON & j, EAGenerationState<Types> const & state) {
        if (!state.elderIdentifiers.empty()) {
            j["elderIdentifiers"] = state.elderIdentifiers;
        }
        if (!state.genePoolIdentifiers.empty()) {
            j["genePoolIdentifiers"] = state.genePoolIdentifiers;
        }
        if (!state.newbornIdentifiers.empty()) {
            j["newbornIdentifiers"] = state.newbornIdentifiers;
        }
        if (!state.survivorIdentifiers.empty()) {
            j["survivorIdentifiers"] = state.survivorIdentifiers;
        }
    }
};

namespace DEvA {
	template <typename Types>
	DEvA::Maybe<EAGenerationState<Types>> importGenerationStateFromFile(std::filesystem::path const & filename) {
		if (not std::filesystem::exists(filename)) {
			return std::unexpected(DEvA::ErrorCode::FileNotFound);
		}
		std::ifstream f;
		f.open(filename, std::ios_base::in);
		if (not f.is_open()) {
			return std::unexpected(DEvA::ErrorCode::FileNotOpen);
		}
		try {
			JSON const parseResult = JSON::parse(f);
			f.close();
			typename Types::SIndividual retVal = parseResult;
			return std::move(retVal);
		} catch (const std::exception &) {
			f.close();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			return std::unexpected(DEvA::ErrorCode::ParseError);
		}
	}

	template <typename Types>
	DEvA::ErrorCode exportGenerationStateToFile(EAGenerationState<Types> const & state, std::filesystem::path const & filename) {
 		JSON const j = state;
		std::ofstream f(filename);
		if (!f.is_open()) {
			return DEvA::ErrorCode::FileNotOpen;
		}
		f << std::setw(4) << j << std::endl;
		return DEvA::ErrorCode::OK;
	}
}
