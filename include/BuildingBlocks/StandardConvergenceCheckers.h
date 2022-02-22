#pragma once

#include "Specialisation.h"

namespace DEvA {
	template <typename Types>
	struct StandardConvergenceCheckers {
		template <auto N>
		static bool equalTo(Types::Fitness fitness) {
			return fitness == N;
		}
	};
}
