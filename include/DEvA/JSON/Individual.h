#pragma once

#include "DEvA/Error.h"
#include "DEvA/Individual.h"
#include "DEvA/JSON/Common.h"
#include "DEvA/JSON/IndividualIdentifier.h"
#include "DEvA/JSON/Metric.h"

namespace nlohmann {
	template <typename Types>
	struct adl_serializer<DEvA::Individual<Types>> {
		static DEvA::Individual<Types> from_json(JSON const & j) {
			DEvA::IndividualIdentifier id = j.at("identifier").get<DEvA::IndividualIdentifier>();
			DEvA::Individual<Types> individual(id, {});
			if (j.contains("parentIdentifiers")) {
				j.at("parentIdentifiers").get_to(individual.parentIdentifiers);
			}
			if (j.contains("genotype")) {
				j.at("genotype").get_to(individual.genotype);
			}
			return individual;
		}

		static void to_json(JSON & j, DEvA::Individual<Types> const & individual) {
			j["identifier"] = individual.id;
			if (!individual.parentIdentifiers.empty()) {
				j["parentIdentifiers"] = individual.parentIdentifiers;
			}
			if (!individual.metricMap.empty()) {
				j["metricMap"] = individual.metricMap;
			}
			j["variation"] = individual.variationInfo.name;
			j["genotype"] = individual.genotype;
		}
	};
}

namespace DEvA {
	template <typename Types>
	DEvA::Maybe<typename Types::SIndividual> importFromFile(std::filesystem::path filename) {
		if (not std::filesystem::exists(filename)) {
			return std::unexpected(DEvA::ErrorCode::FileNotFound);
		}
		std::ifstream f;
		f.open(filename, std::ios_base::in);
		if (not f.is_open()) {
			return std::unexpected(DEvA::ErrorCode::FileNotOpen);
		}
		try {
			typename Types::SIndividual parseResult(std::move(nlohmann::json::parse(f)));
			f.close();
			return std::move(parseResult);
		} catch (const std::exception &) {
			f.close();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			return std::unexpected(DEvA::ErrorCode::ParseError);
		}
	}

	template <typename Types>
	DEvA::ErrorCode exportToFile(typename DEvA::Individual<Types> const & t, std::filesystem::path filename) {
 		JSON j = t;
		std::ofstream f(filename);
		if (!f.is_open()) {
			return DEvA::ErrorCode::FileNotOpen;
		}
		f << std::setw(4) << j << std::endl;
		return DEvA::ErrorCode::OK;
	}
}
