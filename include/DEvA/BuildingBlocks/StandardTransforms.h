#pragma once

#include "DEvA/Specialisation.h"

namespace DEvA {
	template <typename Types>
	struct StandardTransforms {
		static typename Types::Phenotype copy(ParameterMap parameters, typename Types::Genotype in) {
			return in;
		}
	};
}
