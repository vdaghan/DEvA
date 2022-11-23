#pragma once

#include "DEvA/Specialisation.h"

namespace DEvA {
	template <typename Types>
	struct StandardConvergenceCheckers {
		template <auto N>
		static bool equalTo(typename Types::Fitness fitness) {
			return fitness == N;
		}
		template <auto N>
		static bool lessThan(typename Types::Fitness fitness) {
			return fitness < N;
		}
		template <auto N>
		static bool biggerThan(typename Types::Fitness fitness) {
			return fitness > N;
		}
	};
}
