#pragma once

#include "DEvA/Specialisation.h"

namespace DEvA {
	template <typename Types>
	struct StandardSurvivorSelectors {
		static void clamp(ParameterMap parameters, typename Types::IndividualPtrs & iptrs) {
			std::size_t N(parameters.at("N").get<std::size_t>());
			if (iptrs.size() > N) {
				iptrs.resize(N);
			}
		}
		static void all(ParameterMap parameters, typename Types::IndividualPtrs& iptrs) {
			return;
		}
	};
}
