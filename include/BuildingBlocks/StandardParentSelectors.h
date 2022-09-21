#pragma once

#include "Common.h"
#include "Individual.h"
#include "RandomNumberGenerator.h"
#include "Specialisation.h"

#include <algorithm>
#include <ranges>
#include <vector>

namespace DEvA {
	template <typename Types>
	struct StandardParentSelectors {
		using Genotype = Types::Genotype;
		using GenotypeProxy = Types::GenotypeProxy;
		using GenotypeProxies = Types::GenotypeProxies;
		using IndividualPtr = Types::IndividualPtr;
		using IndividualPtrs = Types::IndividualPtrs;
		using FFitnessComparison = Types::FFitnessComparison;

		template <std::size_t N, std::size_t M>
		static IndividualPtrs bestNofM(FFitnessComparison comp, IndividualPtrs domain) {
			std::vector<IndividualPtr> tmp(domain.begin(), domain.end());
			RandomNumberGenerator::get()->shuffle(tmp);
			domain.clear();
			domain.assign(tmp.begin(), tmp.end());
			domain.resize(std::min(M, domain.size()));
			std::stable_sort(domain.begin(), domain.end(), [&](auto & lhs, auto & rhs){ return comp(lhs->fitness, rhs->fitness); });
			domain.resize(N);
			return domain;
		};

		template <std::size_t N>
		static IndividualPtrs randomN(FFitnessComparison comp, IndividualPtrs domain) {
			std::vector<IndividualPtr> tmp(domain.begin(), domain.end());
			RandomNumberGenerator::get()->shuffle(tmp);
			domain.clear();
			domain.assign(tmp.begin(), tmp.end());
			domain.resize(N);
			std::stable_sort(domain.begin(), domain.end(), [&](auto & lhs, auto & rhs) { return comp(lhs->fitness, rhs->fitness); });
			return domain;
		};

		template <std::size_t N>
		static IndividualPtrs bestNofAll(FFitnessComparison comp, IndividualPtrs domain) {
			return bestNofM<N, std::numeric_limits<std::size_t>::max()>(domain);
		};
	};
};
