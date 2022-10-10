#pragma once

#include "IndividualIdentifier.h"
#include "VParameters.h"

#include <list>
#include <memory>
#include <vector>

namespace DEvA {
	template <typename Types, typename VP>
	class Individual : public std::enable_shared_from_this<Individual<Types, VP>> {
		public:
			Individual(std::size_t gen, std::size_t id, Types::GenotypeProxy gpx)
					: id(IndividualIdentifier{.generation = gen, .identifier = id})
					, genotypeProxy(gpx) {
			};
			void setParents(Types::IndividualPtrs parents_) {
				parents = parents_;
				for (auto it = parents.begin(); it != parents.end(); ++it) {
					(* it)->children.push_back(this->shared_from_this());
				}
			}

			bool isInvalid() {
				bool invalidTransform = std::unexpected(ErrorCode::InvalidTransform) == maybePhenotypeProxy;
				return invalidTransform;
			};

			IndividualIdentifier const id;
			Types::GenotypeProxy const genotypeProxy;
			Types::MaybePhenotypeProxy maybePhenotypeProxy;
			Types::Fitness fitness;
			Types::IndividualPtrs parents;
			Types::IndividualWPtrs children;
		private:
			VP vparameters;
	};


	template <typename Types, typename VP>
	bool operator==(Individual<Types, VP> const & lhs, Individual<Types, VP> const & rhs) {
		return lhs.id == rhs.id;
	}
	template <typename Types, typename VP>
	bool operator==(std::shared_ptr<Individual<Types, VP>> const & lhs, std::shared_ptr<Individual<Types, VP>> const & rhs) {
		return *lhs == *rhs;
	}
}
