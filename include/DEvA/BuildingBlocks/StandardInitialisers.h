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
		static Types::GenotypeProxies permutations() {
			auto createRandomGenotype = [&]() -> Types::GenotypeProxy {
				typename Types::GenotypeProxy gpx = std::make_shared<Types::Genotype>();
				std::vector<int> toPick(N);
				std::iota(toPick.begin(), toPick.end(), 0);
				for (std::size_t i = 0; i < N; ++i) {
					std::size_t randomIndex = RandomNumberGenerator::get()->getIntBetween<std::size_t>(0, N - 1 - i);
					int randPosition = toPick[randomIndex];
					std::erase(toPick, randPosition);
					gpx->emplace_back(i) = randPosition;
				}
				return gpx;
			};
			typename Types::GenotypeProxies proxies{};
			for (size_t i = 0; i < M; ++i) {
				proxies.emplace_back(createRandomGenotype());
			}
			return proxies;
		}
	};
}
