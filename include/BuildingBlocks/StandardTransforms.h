#pragma once

#include "Specialisation.h"

namespace DEvA {
	template <typename Types>
	struct StandardTransforms {
		static Types::PhenotypePtr copy(Types::GenotypePtr in) {
			return in;
		}
	};
}
