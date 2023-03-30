#pragma once

#include "spdlog/fmt/ostr.h"
#include <spdlog/fmt/bundled/format.h>

#include <vector>

namespace DEvA {
	struct IndividualIdentifier {
		std::size_t generation;
		std::size_t identifier;
		friend bool operator<(IndividualIdentifier l, IndividualIdentifier r) {
			return std::tie(l.generation, l.identifier) < std::tie(r.generation, r.identifier);
		}
		friend bool operator>(IndividualIdentifier lhs, IndividualIdentifier rhs) {
			return rhs < lhs;
		}
		friend bool operator<=(IndividualIdentifier lhs, IndividualIdentifier rhs) {
			return !(lhs > rhs);
		}
		friend bool operator>=(IndividualIdentifier lhs, IndividualIdentifier rhs) {
			return !(lhs < rhs);
		}
		friend bool operator==(IndividualIdentifier lhs, IndividualIdentifier rhs) {
			return lhs.generation == rhs.generation and lhs.identifier == rhs.identifier;
		}
		friend bool operator!=(IndividualIdentifier lhs, IndividualIdentifier rhs) {
			return !(lhs == rhs);
		}
	};
	using IndividualIdentifiers = std::vector<IndividualIdentifier>;
}

template<> struct fmt::formatter<DEvA::IndividualIdentifier> {
	constexpr auto parse(format_parse_context & ctx) -> decltype(ctx.begin()) {
		return ctx.end();
	}
	template <typename FormatContext>
	auto format(DEvA::IndividualIdentifier const & input, FormatContext & ctx) -> decltype(ctx.out()) {
		return fmt::format_to(ctx.out(), "({}, {})", input.generation, input.identifier);
	}
};
