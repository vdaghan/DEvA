#pragma once

#include "Specialisation.h"

#include <numeric>
#include <random>
#include <vector>

namespace DEvA {
	template <typename Types>
	struct StandardInitialisers {
		template <size_t N, size_t M>
		static Types::Generation permutations() {
			std::default_random_engine randGen;
			auto createRandomGenotype = [&]() -> Types::GenotypePtr {
				typename Types::GenotypePtr gptr = std::make_shared<Types::Genotype>();
				std::vector<int> toPick(N);
				std::iota(toPick.begin(), toPick.end(), 0);
				for (size_t i = 0; i < N; ++i) {
					std::uniform_int_distribution<size_t> distribution(0, N-1-i);
					size_t randomIndex = distribution(randGen);
					int randPosition = toPick[randomIndex];
					std::erase(toPick, randPosition);
					gptr->emplace_back(i) = randPosition;
				}
				return gptr;
			};
			typename Types::Generation generation;
			for (size_t i = 0; i < M; ++i) {
				typename Types::GenotypePtr gptr = createRandomGenotype();
				generation.emplace_back(std::make_shared<Types::SIndividual>(gptr));
			}
			return generation;
		}
	};
}
