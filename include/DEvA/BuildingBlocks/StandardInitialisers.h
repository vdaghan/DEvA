#pragma once

#include "DEvA/RandomNumberGenerator.h"
#include <DEvA/Specialisation.h>

#include <concepts>
#include <numeric>
#include <random>
#include <vector>

namespace DEvA {
	template <typename Types>
	struct StandardInitialisers {
		static typename Types::Genotypes empty(ParameterMap parameters) {
			return {};
		}

		static typename Types::Genotypes permutations(ParameterMap parameters)
		requires requires(typename Types::Genotype genotype) { genotype.emplace_back(std::size_t{}); } {
			std::size_t N(parameters.at("N").get<std::size_t>());
			std::size_t M(parameters.at("M").get<std::size_t>());
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
