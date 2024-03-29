#pragma once

#include "DEvA/Project/Datastore.h"
#include "DEvA/EAState.h"
#include "DEvA/Error.h"
#include "DEvA/IndividualIdentifier.h"
#include "DEvA/JSON/Common.h"
#include "DEvA/JSON/Individual.h"
#include "DEvA/JSON/GenerationState.h"

#include <exception>
#include <fstream>
#include <optional>
#include <string>
#include <sstream>

namespace DEvA {
	inline std::optional<std::size_t> stringToSizeT(std::string const & s) {
		std::size_t retVal;
		std::stringstream ss(s);
		if (!(ss >> retVal) || !ss.eof()) {
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
			std::size_t const generation(iptr->id.generation);
			std::size_t const identifier(iptr->id.identifier);
			if (m_individualFileMap.contains(iptr->id)) {
				return;
			}
			std::filesystem::path const generationPath(dataDirectory / std::to_string(generation));
			std::filesystem::create_directories(generationPath);
			std::filesystem::path const individualPath(generationPath / (std::to_string(identifier) + ".deva"));
			DEvA::exportToFile<Types>(*iptr, individualPath);
		}

		void exportGenerationState(EAGenerationState<Types> const & genState, std::size_t generation) override {
			std::filesystem::path const generationPath(dataDirectory / std::to_string(generation));
			std::filesystem::create_directories(generationPath);
			std::filesystem::path const stateFilePath(generationPath / "state.gen");
			DEvA::exportGenerationStateToFile<Types>(genState, stateFilePath);
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
