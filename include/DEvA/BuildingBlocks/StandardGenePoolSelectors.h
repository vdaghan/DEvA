#pragma once

#include "DEvA/Specialisation.h"

namespace DEvA {
	template <typename Types>
	struct StandardGenePoolSelectors {
		static typename Types::Generation all(ParameterMap parameters, typename Types::Generation gen) {
			return gen;
		}
	};
}
