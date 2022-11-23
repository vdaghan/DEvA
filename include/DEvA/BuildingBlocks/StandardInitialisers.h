#pragma once

#include "DEvA/RandomNumberGenerator.h"
#include "DEvA/Specialisation.h"

#include <numeric>
#include <random>
#include <vector>

namespace DEvA {
	template <typename Types>
	struct StandardInitialisers {
		template <std::size_t N, std::size_t M>
		static typename Types::Genotypes permutations() {
			auto createRandomGenotype = [&]() -> typename Types::Genotype {
				typename Types::Genotype genotype{};
				std::vector<int> toPick(N);
				std::iota(toPick.begin(), toPick.end(), 0);
				for (std::size_t i = 0; i < N; ++i) {
					auto const randomIndex = RandomNumberGenerator::get()->getIntBetween<std::size_t>(0, N - 1 - i);
					int randPosition = toPick[randomIndex];
					std::erase(toPick, randPosition);
					genotype.emplace_back(i) = randPosition;
				}
				return genotype;
			};
			typename Types::Genotypes genotypes{};
			for (size_t i = 0; i < M; ++i) {
				genotypes.emplace_back(createRandomGenotype());
			}
			return genotypes;
		}
	};
}
