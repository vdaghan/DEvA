#pragma once

#include "Common.h"
#include "Individual.h"
#include "Specialisation.h"

#include <ranges>

namespace DEvA {
	template <typename Types>
	struct StandardParentSelectors {
		using Genotype = Types::Genotype;
		using GenotypeProxy = Types::GenotypeProxy;
		using GenotypeProxies = Types::GenotypeProxies;
		using IndividualPtrs = Types::IndividualPtrs;

		template <size_t N, size_t M>
		static IndividualPtrs bestNofM(IndividualPtrs domain) {
			bool const isSorted = std::ranges::is_sorted(domain.begin(), domain.end(), [](auto iptr1, auto iptr2) -> bool { return ((iptr1->fitness) < (iptr2->fitness)); });
			if (!isSorted) [[unlikely]] {
				std::stable_sort(domain.begin(), domain.end(), [](auto iptr1, auto iptr2) -> bool { return ((iptr1->fitness) < (iptr2->fitness)); });
			}

			static std::default_random_engine randGen;
			IndividualPtrs rest(domain);
			IndividualPtrs preimage;

			for (size_t i = 0; i < N; ++i) {
				std::uniform_int_distribution<int> distribution(0, rest.size() - 1);
				size_t randomIndex = distribution(randGen);
				auto it = rest.begin();
				for (size_t j = 0; j < randomIndex; ++j) {
					++it;
				}
				preimage.emplace_back(*it);
				rest.erase(it);
			}
			//Spec::RFGenotypePtrSet rfgps({ .domain = domain,.preimage = preimage, .rest = rest });
			return preimage;
		};
	};
};
