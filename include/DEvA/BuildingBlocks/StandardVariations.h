#pragma once

#include "DEvA/Common.h"
#include "DEvA/Individual.h"
#include "DEvA/Specialisation.h"

#include <algorithm>
#include <concepts>
#include <memory>
#include <random>

namespace DEvA {
	template <typename Types>
	struct StandardVariations {
		// For convenience
		using Genotype = typename Types::Genotype;
		using Genotypes = typename Types::Genotypes;

		//static GenotypeProxy copy(GenotypeProxy gpx) {
		//	return std::make_shared<Genotype>(*gpx);
		//}

		/*template <typename T>
		requires std::same_as<T, GenotypePtrs> and !std::is_bounded_array<decltype(*T::first())>
		static T cutAndCrossfill(T gptrs) {*/
		static Genotypes cutAndCrossfill(Genotypes genotypes) {
			//GenotypeProxy firstParent = std::make_shared<Genotype>(*(gpxs.front()));
			Genotype firstParent = genotypes.front();
			Genotype secondParent = genotypes.back();
			Genotype firstOffspring{};
			Genotype secondOffspring{};

			size_t const genotypeSize = firstParent.size();
			std::random_device r;
			std::default_random_engine randGen(r());
			std::uniform_int_distribution<int> distribution(0, genotypeSize - 1);
			size_t crossoverIndex = distribution(randGen);

			auto itCrossover1 = Common::iteratorForNthListElement<Types>(firstParent, crossoverIndex);
			auto itCrossover2 = Common::iteratorForNthListElement<Types>(secondParent, crossoverIndex);
			auto it1 = firstParent.begin();
			auto it2 = secondParent.begin();
			size_t ind(0);

			while (ind <= crossoverIndex) {
				firstOffspring.push_back(*it1);
				secondOffspring.push_back(*it2);
				++it1;
				++it2;
				++ind;
			}

			for (auto it = secondParent.begin(); it != secondParent.end(); ++it) {
				if (firstOffspring.end() == std::find(firstOffspring.begin(), firstOffspring.end(), *it)) {
					firstOffspring.push_back(*it);
				}
			}

			for (auto it = firstParent.begin(); it != firstParent.end(); ++it) {
				if (secondOffspring.end() == std::find(secondOffspring.begin(), secondOffspring.end(), *it)) {
					secondOffspring.push_back(*it);
				}
			}

			Genotypes retList = { firstOffspring, secondOffspring };
			return retList;
		}

		static Genotype swap(Genotype genotype) {
			std::default_random_engine randGen;
			std::uniform_int_distribution<int> distribution(0, genotype.size() - 1);
			size_t swapIndex1 = distribution(randGen);
			size_t swapIndex2 = distribution(randGen);

			auto it1 = Common::iteratorForNthListElement<Types>(genotype, swapIndex1);
			auto it2 = Common::iteratorForNthListElement<Types>(genotype, swapIndex2);

			std::swap(it1, it2);

			return genotype;
		}
	};
}
