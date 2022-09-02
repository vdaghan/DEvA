#pragma once

#include "Common.h"
#include "Individual.h"
#include "Specialisation.h"

#include <algorithm>
#include <concepts>
#include <memory>
#include <random>

namespace DEvA {
	template <typename Types>
	struct StandardVariations {
		// For convenience
		using Genotype = Types::Genotype;
		using GenotypeProxy = Types::GenotypeProxy;
		using GenotypeProxies = Types::GenotypeProxies;

		//static GenotypeProxy copy(GenotypeProxy gpx) {
		//	return std::make_shared<Genotype>(*gpx);
		//}

		/*template <typename T>
		requires std::same_as<T, GenotypePtrs> and !std::is_bounded_array<decltype(*T::first())>
		static T cutAndCrossfill(T gptrs) {*/
		static GenotypeProxies cutAndCrossfill(GenotypeProxies gpxs) {
			//GenotypeProxy firstParent = std::make_shared<Genotype>(*(gpxs.front()));
			GenotypeProxy firstParent = gpxs.front();
			GenotypeProxy secondParent = gpxs.back();
			GenotypeProxy firstOffspring = std::make_shared<Genotype>();
			GenotypeProxy secondOffspring = std::make_shared<Genotype>();

			size_t genotypeSize = firstParent->size();
			std::random_device r;
			std::default_random_engine randGen(r());
			std::uniform_int_distribution<int> distribution(0, genotypeSize - 1);
			size_t crossoverIndex = distribution(randGen);

			auto itCrossover1 = Common::iteratorForNthListElement<Types>(*firstParent, crossoverIndex);
			auto itCrossover2 = Common::iteratorForNthListElement<Types>(*secondParent, crossoverIndex);
			auto it1 = firstParent->begin();
			auto it2 = secondParent->begin();
			size_t ind1(0);
			size_t ind2(0);

			while (ind1 <= crossoverIndex) {
				firstOffspring->push_back(*it1);
				secondOffspring->push_back(*it2);
				++it1;
				++it2;
				++ind1;
				++ind2;
			}

			for (auto it = secondParent->begin(); it != secondParent->end(); ++it) {
				if (firstOffspring->end() == std::find(firstOffspring->begin(), firstOffspring->end(), *it)) {
					firstOffspring->push_back(*it);
				}
			}

			for (auto it = firstParent->begin(); it != firstParent->end(); ++it) {
				if (secondOffspring->end() == std::find(secondOffspring->begin(), secondOffspring->end(), *it)) {
					secondOffspring->push_back(*it);
				}
			}

			GenotypeProxies retList = { firstOffspring, secondOffspring };
			return retList;
		}

		static GenotypeProxy swap(GenotypeProxy gpx) {
			GenotypeProxy offspring = Types::F::copy(gpx);

			std::default_random_engine randGen;
			std::uniform_int_distribution<int> distribution(0, offspring->size() - 1);
			size_t swapIndex1 = distribution(randGen);
			size_t swapIndex2 = distribution(randGen);

			auto it1 = Common::iteratorForNthListElement<Types>(*offspring, swapIndex1);
			auto it2 = Common::iteratorForNthListElement<Types>(*offspring, swapIndex2);

			std::swap(it1, it2);

			return offspring;
		}
	};
}
