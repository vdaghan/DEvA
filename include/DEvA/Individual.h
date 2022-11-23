#pragma once

#include "DEvA/IndividualIdentifier.h"
#include "DEvA/Metric.h"
#include "DEvA/VariationFunctor.h"
#include "DEvA/VParameters.h"

#include <list>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace DEvA {
	template <typename Types>
	struct Individual : public std::enable_shared_from_this<Individual<Types>> {
		Individual(IndividualIdentifier individualIdentifier, typename Types::GenotypeProxy gpx)
			: id(individualIdentifier)
			, genotypeProxy(gpx) {
		};
		Individual(Individual && other) noexcept = default;
		void setParents(typename Types::IndividualPtrs parents_) {
			parents = parents_;
		}

		[[nodiscard]] bool isInvalid() {
			bool invalidTransform = std::unexpected(ErrorCode::InvalidTransform) == maybePhenotypeProxy;
			return invalidTransform;
		}

		IndividualIdentifier id;
		VariationInfo<Types> variationInfo;
		typename Types::GenotypeProxy genotypeProxy;
		typename Types::Genotype genotype;
		typename Types::MaybePhenotypeProxy maybePhenotypeProxy;
		typename Types::SMetricMap metricMap;
		std::vector<IndividualIdentifier> parentIdentifiers;
		typename Types::IndividualPtrs parents;
	};


	template <typename Types>
	bool operator==(Individual<Types> const & lhs, Individual<Types> const & rhs) {
		return lhs.id == rhs.id;
	}
	template <typename Types>
	bool operator==(std::shared_ptr<Individual<Types>> const & lhs, std::shared_ptr<Individual<Types>> const & rhs) {
		return *lhs == *rhs;
	}
}
