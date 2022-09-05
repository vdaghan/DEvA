#pragma once

#include "Specialisation.h"

namespace DEvA {
	template <typename Types>
	struct StandardConvergenceCheckers {
		template <auto N>
		static bool equalTo(Types::Fitness fitness) {
			return fitness == N;
		}
		template <auto N>
		static bool lessThan(Types::Fitness fitness) {
			return fitness < N;
		}
		template <auto N>
		static bool biggerThan(Types::Fitness fitness) {
			return fitness > N;
		}
	};
}
