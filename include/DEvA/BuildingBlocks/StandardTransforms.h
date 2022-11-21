#pragma once

#include "DEvA/Specialisation.h"

namespace DEvA {
	template <typename Types>
	struct StandardTransforms {
		static Types::PhenotypeProxy copy(Types::GenotypeProxy in) {
			return in;
		}
	};
}
