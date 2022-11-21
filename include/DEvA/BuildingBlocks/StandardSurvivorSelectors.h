#pragma once

#include "DEvA/Specialisation.h"

namespace DEvA {
	template <typename Types>
	struct StandardSurvivorSelectors {
		template <size_t N>
		static void clamp(Types::IndividualPtrs& iptrs) {
			if (iptrs.size() > N) {
				iptrs.resize(N);
			}
		}
	};
}
