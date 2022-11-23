#pragma once

#include "DEvA/IndividualIdentifier.h"
#include "DEvA/Metric.h"
#include "DEvA/VariationFunctor.h"

#include <memory>
#include <vector>

namespace DEvA {
	template <typename Types>
	struct Individual : public std::enable_shared_from_this<Individual<Types>> {
        explicit Individual(IndividualIdentifier const individualIdentifier) : id(individualIdentifier) {}
		explicit Individual(Individual const &) = delete;
		Individual & operator=(Individual const &) = delete;
		Individual(Individual &&) noexcept = default;
		Individual & operator=(Individual &&) noexcept = default;
		~Individual() = default;
		void setParents(typename Types::IndividualPtrs parents_) {
			parents = parents_;
		}

		[[nodiscard]] bool isInvalid() {
			bool const invalidTransform = std::unexpected(ErrorCode::InvalidTransform) == maybePhenotype;
			return invalidTransform;
		}

		IndividualIdentifier id;
		VariationInfo<Types> variationInfo;
		typename Types::Genotype genotype;
		typename Types::MaybePhenotype maybePhenotype;
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
