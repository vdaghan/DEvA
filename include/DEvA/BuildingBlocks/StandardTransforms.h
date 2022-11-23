#pragma once

#include "DEvA/Specialisation.h"

namespace DEvA {
	template <typename Types>
	struct StandardTransforms {
		static typename Types::PhenotypeProxy copy(typename Types::GenotypeProxy in) {
			return in;
		}
	};
}
