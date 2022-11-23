#pragma once

#include "DEvA/Common.h"
#include "DEvA/Individual.h"
#include "DEvA/RandomNumberGenerator.h"
#include "DEvA/Specialisation.h"

#include <algorithm>
#include <ranges>
#include <string_view>
#include <vector>

namespace DEvA {
	template <typename Types>
	struct StandardParentSelectors {
		using IndividualPtr = typename Types::IndividualPtr;
		using IndividualPtrs = typename Types::IndividualPtrs;

		template <std::size_t N, std::size_t M>
		static IndividualPtrs bestNofM(std::string metricName, IndividualPtrs domain) {
			std::vector<IndividualPtr> tmp(domain.begin(), domain.end());
			RandomNumberGenerator::get()->shuffle(tmp);
			domain.clear();
			domain.assign(tmp.begin(), tmp.end());
			domain.resize(std::min(M, domain.size()));
			std::stable_sort(domain.begin(), domain.end(), [&](auto & lhs, auto & rhs){
				return lhs->metricMap.at(metricName) < rhs->metricMap.at(metricName);
			});
			domain.resize(N);
			return domain;
		};

		template <std::size_t N>
		static IndividualPtrs randomN(std::string metricName, IndividualPtrs domain) {
			std::vector<IndividualPtr> tmp(domain.begin(), domain.end());
			RandomNumberGenerator::get()->shuffle(tmp);
			domain.clear();
			domain.assign(tmp.begin(), tmp.end());
			domain.resize(N);
			std::stable_sort(domain.begin(), domain.end(), [&](auto & lhs, auto & rhs) {
				return lhs->metricMap.at(metricName) < rhs->metricMap.at(metricName);
			});
			return domain;
		};

		template <std::size_t N>
		static IndividualPtrs bestNofAll(std::string metricName, IndividualPtrs domain) {
			return bestNofM<N, std::numeric_limits<std::size_t>::max()>(metricName, domain);
		};
	};
}
