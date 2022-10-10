#pragma once

#include <vector>

namespace DEvA {
	struct IndividualIdentifier {
		std::size_t generation;
		std::size_t identifier;
		friend bool operator<(IndividualIdentifier const& l, IndividualIdentifier const& r) {
			return std::tie(l.generation, l.identifier) < std::tie(r.generation, r.identifier);
		}
		friend bool operator>(IndividualIdentifier const& lhs, IndividualIdentifier const& rhs) {
			return rhs < lhs;
		}
		friend bool operator<=(IndividualIdentifier const& lhs, IndividualIdentifier const& rhs) {
			return !(lhs > rhs);
		}
		friend bool operator>=(IndividualIdentifier const& lhs, IndividualIdentifier const& rhs) {
			return !(lhs < rhs);
		}
		friend bool operator==(IndividualIdentifier const& lhs, IndividualIdentifier const& rhs) {
			return lhs.generation == rhs.generation and lhs.identifier == rhs.identifier;
		}
		friend bool operator!=(IndividualIdentifier const& lhs, IndividualIdentifier const& rhs) {
			return !(lhs == rhs);
		}
	};
	using IndividualIdentifiers = std::vector<IndividualIdentifier>;
}
