#pragma once

#include <filesystem>

#include "DEvA/Logger.h"
#include <DEvA/JSON/Functions.h>

namespace DEvA {
	template <typename Types>
	void EvolutionaryAlgorithm<Types>::importSetup(std::filesystem::path const & filename) {
		if (not std::filesystem::exists(filename)) {
			return;
		}
		std::ifstream f;
		f.open(filename, std::ios_base::in);
		if (not f.is_open()) {
			return;
		}
		JSON parseResult{};
		try {
			parseResult = JSON::parse(f);
			f.close();
		} catch (const std::exception&) {
			f.close();
			return;
		}
		importFunctions<Types>(parseResult, functions);
	}

	template <typename Types>
	void EvolutionaryAlgorithm<Types>::recallData() {
		auto const & individuals = datastore->scanIndividuals();
		for (auto const & individualIdentifier : individuals) {
			auto const generation(individualIdentifier.generation);
			genealogy.resize(std::max(generation, genealogy.size()));
			typename Types::IndividualPtr iptr = datastore->importIndividual(individualIdentifier);
			if (iptr) {
				genealogy.at(generation).emplace_back(iptr);
				for (auto & metricPair : iptr->metricMap) {
					auto & metricName(metricPair.first);
					auto & metricObject(metricPair.second);
					if (registeredMetricFunctors.contains(metricName)) [[likely]] {
						if (not metricFunctorsInUse.contains(metricName)) [[unlikely]] {
							metricFunctorsInUse.emplace(metricName);
						}
						auto & metricFunctor(registeredMetricFunctors.at(metricName));
						metricFunctor.assignFunctions(metricObject);
					}
				}
			}
		}
	}

	template <typename Types>
	IndividualIdentifiers EvolutionaryAlgorithm<Types>::extractIdentifiers(typename Types::Generation const & individuals) {
		IndividualIdentifiers identifiers{};
		std::transform(individuals.begin(), individuals.end(), std::back_inserter(identifiers), [](auto const & iptr) {
			return iptr->id;
		});
		return identifiers;
	}

	template <typename Types>
	void EvolutionaryAlgorithm<Types>::saveIndividuals(typename Types::Generation const & individuals) {
		for (auto & iptr : individuals) {
			if (currentGeneration.load() != iptr->id.generation) [[unlikely]] {
				continue;
			}
			for (auto & parent : iptr->parents) {
				iptr->parentIdentifiers.emplace_back(parent->id);
			}
			datastore->exportIndividual(iptr);
		}
	}

	template <typename Types>
	void EvolutionaryAlgorithm<Types>::saveState(EAGenerationState<Types> const & eaState, std::size_t generation) {
		datastore->exportGenerationState(eaState, generation);
	}
}
