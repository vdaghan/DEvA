#pragma once

#include "Common.h"
#include "Individual.h"
#include "RandomNumberGenerator.h"
#include "Specialisation.h"

#include <algorithm>
#include <ranges>
#include <string_view>
#include <vector>

namespace DEvA {
	template <typename Types>
	struct StandardParentSelectors {
		using Genotype = Types::Genotype;
		using GenotypeProxy = Types::GenotypeProxy;
		using GenotypeProxies = Types::GenotypeProxies;
		using IndividualPtr = Types::IndividualPtr;
		using IndividualPtrs = Types::IndividualPtrs;

		template <std::size_t N, std::size_t M>
		static IndividualPtrs bestNofM(std::string metricName, Types::MetricComparisonMap const & compMap, IndividualPtrs domain) {
			std::vector<IndividualPtr> tmp(domain.begin(), domain.end());
			RandomNumberGenerator::get()->shuffle(tmp);
			domain.clear();
			domain.assign(tmp.begin(), tmp.end());
			domain.resize(std::min(M, domain.size()));
			auto const & comp(compMap.at(metricName));
			std::stable_sort(domain.begin(), domain.end(), [&](auto & lhs, auto & rhs){ return comp(lhs->metrics.at(metricName), rhs->metrics.at(metricName)); });
			domain.resize(N);
			return domain;
		};

		template <std::size_t N>
		static IndividualPtrs randomN(std::string metricName, Types::MetricComparisonMap const & compMap, IndividualPtrs domain) {
			std::vector<IndividualPtr> tmp(domain.begin(), domain.end());
			RandomNumberGenerator::get()->shuffle(tmp);
			domain.clear();
			domain.assign(tmp.begin(), tmp.end());
			domain.resize(N);
			auto const& comp(compMap.at(metricName));
			std::stable_sort(domain.begin(), domain.end(), [&](auto & lhs, auto & rhs) { return comp(lhs->metrics.at(metricName), rhs->metrics.at(metricName)); });
			return domain;
		};

		template <std::size_t N>
		static IndividualPtrs bestNofAll(std::string metricName, Types::MetricComparisonMap const & compMap, IndividualPtrs domain) {
			return bestNofM<N, std::numeric_limits<std::size_t>::max()>(metricName, compMap, domain);
		};
	};
};
