#pragma once

#include "DEvA/BuildingBlocks/EAFunctions/FunctionBlueprint.h"

#include <concepts>

namespace DEvA {
	template <typename Blueprint>
	requires CEndomorphism<Blueprint>
	struct Compose {
		using Function = typename Blueprint::Function;
		using ReturnType = typename Blueprint::ReturnType;
		static constexpr std::size_t arity{ 2 };
		Function operator()(Function f1, Function f2) {
			auto composeLambda = [f1, f2](ReturnType arg) {
				return f1(f2(arg));
			};
			return composeLambda;
		}
	};
}
