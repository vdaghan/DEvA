#pragma once

namespace DEvA {
	template <typename Types>
	struct StandardGenePoolSelectors {
		static Types::Generation all(Types::Generation gen) {
			return gen;
		}
	};
}
