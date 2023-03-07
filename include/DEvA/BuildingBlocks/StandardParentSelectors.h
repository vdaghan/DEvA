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

		static IndividualPtrs bestNofM(ParameterMap parameters, IndividualPtrs domain) {
			std::size_t N(parameters.at("N").get<std::size_t>());
			std::size_t M(parameters.at("M").get<std::size_t>());
			std::string metricName(parameters.at("metric").get<std::string>());

			std::vector<IndividualPtr> tmp(domain.begin(), domain.end());
			RandomNumberGenerator::get()->shuffle(tmp);
			domain.clear();
			domain.assign(tmp.begin(), tmp.end());
			domain.resize(std::min(M, domain.size()));
			std::stable_sort(domain.begin(), domain.end(), [&](auto & lhs, auto & rhs){
				return lhs->metricMap.at(metricName).isBetterThan(rhs->metricMap.at(metricName));
			});
			domain.resize(N);
			return domain;
		};

		static IndividualPtrs randomN(ParameterMap parameters, IndividualPtrs domain) {
			std::size_t N(parameters.at("N").get<std::size_t>());
			std::string metricName(parameters.at("metric").get<std::string>());

			std::vector<IndividualPtr> tmp(domain.begin(), domain.end());
			RandomNumberGenerator::get()->shuffle(tmp);
			domain.clear();
			domain.assign(tmp.begin(), tmp.end());
			domain.resize(N);
			std::stable_sort(domain.begin(), domain.end(), [&](auto & lhs, auto & rhs) {
				return lhs->metricMap.at(metricName).isBetterThan(rhs->metricMap.at(metricName));
			});
			return domain;
		};

		static IndividualPtrs bestNofAll(ParameterMap parameters, IndividualPtrs domain) {
			std::size_t N(parameters.at("N").get<std::size_t>());
			std::string metricName(parameters.at("metric").get<std::string>());

			constexpr std::size_t maxSizeT(std::numeric_limits<std::size_t>::max());
			return bestNofM({{"N", N}, {"M", maxSizeT}, {"metric", metricName}}, domain);
		};
	};
}
