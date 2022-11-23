#pragma once

#include "DEvA/Datastore.h"
#include "DEvA/Error.h"
#include "DEvA/IndividualIdentifier.h"
#include "DEvA/JSON/Common.h"
#include "DEvA/JSON/Individual.h"

#include <exception>
#include <fstream>
#include <optional>
#include <string>

namespace DEvA {
	inline std::optional<std::size_t> stringToSizeT(std::string const & s) {
		std::size_t retVal;
		try {
			retVal = std::stoul(s);
		}
		catch (const std::exception&) {
			return std::nullopt;
		}
		return retVal;
	}

	template <typename Types>
	struct Filestore : Datastore<Types> {
		Filestore(std::string const & path = "") {
			if (path.empty()) {
				dataDirectory.assign(std::filesystem::current_path() / "data");
			} else {
				dataDirectory.assign(path);
			}
			std::filesystem::remove_all(dataDirectory);
			std::filesystem::create_directories(dataDirectory);
		}
		typename Datastore<Types>::IndividualFileSet const & scanIndividuals() override {
			for (auto & gen : std::filesystem::directory_iterator{dataDirectory}) {
				if (gen.path() == dataDirectory or not std::filesystem::is_directory(gen.path())) {
					continue;
				}
				auto genOpt = stringToSizeT(gen.path().filename().string());
				if (!genOpt) {
					continue;
				}
				auto & generation(*genOpt);
				std::filesystem::path generationPath(dataDirectory / std::to_string(generation));
				for (auto & id : std::filesystem::directory_iterator{generationPath}) {
					if (std::filesystem::is_directory(id.path())) {
						continue;
					}
					std::string const fileNameStem = id.path().stem().string();
					std::string const fileNameExtension = id.path().extension().string();
					if ("deva" != fileNameExtension) {
						continue;
					}
					auto idOpt = stringToSizeT(fileNameStem);
					if (!idOpt) {
						continue;
					}
					auto & identifier(*idOpt);
					DEvA::IndividualIdentifier individualIdentifier{.generation = generation, .identifier = identifier};
					if (not m_individualFileSet.contains(individualIdentifier)) {
						m_individualFileSet.emplace(individualIdentifier);
						m_individualFileMap.emplace(std::make_pair(individualIdentifier, id));
					}
				}
			}
			return m_individualFileSet;
		}

		typename Types::IndividualPtr importIndividual(DEvA::IndividualIdentifier id) override {
			if (not m_individualFileMap.contains(id)) {
				return {};
			}
			auto individualPath(m_individualFileMap.at(id));
			if (not std::filesystem::exists(individualPath)) {
				return {};
			}
			auto && individual(DEvA::importFromFile<Types>(individualPath));
			if (not individual.has_value()) {
				return {};
			}
			return std::make_shared<typename Types::SIndividual>(std::move(individual.value()));
		}

		void exportIndividual(typename Types::IndividualPtr iptr) override {
			std::size_t generation(iptr->id.generation);
			std::size_t identifier(iptr->id.identifier);
			if (m_individualFileMap.contains(iptr->id)) {
				return;
			}
			std::filesystem::path generationPath(dataDirectory / std::to_string(generation));
			std::filesystem::create_directories(generationPath);
			std::filesystem::path individualPath(generationPath / (std::to_string(identifier) + ".deva"));
			DEvA::exportToFile<Types>(*iptr, individualPath);
		}

		typename Datastore<Types>::IndividualFileMap & individualFileMap() override {
			return m_individualFileMap;
		};
		typename Datastore<Types>::IndividualFileSet & individualFileSet() override {
			return m_individualFileSet;
		};

		typename Datastore<Types>::IndividualFileMap m_individualFileMap{};
		typename Datastore<Types>::IndividualFileSet m_individualFileSet{};
		std::filesystem::path dataDirectory{};
	};
}
