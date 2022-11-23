#pragma once

namespace DEvA {
	template <typename Types>
	struct StandardGenePoolSelectors {
		static typename Types::Generation all(typename Types::Generation gen) {
			return gen;
		}
	};
}
