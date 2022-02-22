#pragma once

#include "Specialisation.h"

namespace DEvA {
	template <typename Types>
	struct StandardSurvivorSelectors {
		template <size_t N>
		static void clamp(Types::IndividualPtrs& iptrs) {
			while (iptrs.size() > N) {
				iptrs.erase(iptrs.begin());
			}
		}
	};
}
