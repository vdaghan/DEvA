#pragma once

#include "Common.h"
#include "Individual.h"
#include "RandomNumberGenerator.h"
#include "Specialisation.h"

#include <algorithm>
#include <ranges>

namespace DEvA {
	template <typename Types>
	struct StandardParentSelectors {
		using Genotype = Types::Genotype;
		using GenotypeProxy = Types::GenotypeProxy;
		using GenotypeProxies = Types::GenotypeProxies;
		using IndividualPtrs = Types::IndividualPtrs;

		template <std::size_t N, std::size_t M>
		static IndividualPtrs bestNofM(IndividualPtrs domain) {
			IndividualPtrs retVal{domain};

			retVal.resize(std::min(M, retVal.size()));
			std::vector<Types::IndividualPtr> iptrs(retVal.begin(), retVal.end());
			RandomNumberGenerator::get()->shuffle(iptrs);
			iptrs.resize(N);
			retVal.clear();
			retVal.assign(iptrs.begin(), iptrs.end());

			return retVal;
		};

		template <std::size_t N>
		static IndividualPtrs bestNofAll(IndividualPtrs domain) {
			return bestNofM<N, std::numeric_limits<std::size_t>::max()>(domain);
		};
	};
};
