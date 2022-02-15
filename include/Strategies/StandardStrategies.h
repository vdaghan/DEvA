#pragma once

#include "Individual.h"

namespace DEvA {
	namespace Strategies {
		template <typename Types>
		Individual<Types>::IndividualPtrList IdentityParentSelection(Individual<Types>::IndividualPtrList individuals) {
			return individuals;
		}
	}
}
