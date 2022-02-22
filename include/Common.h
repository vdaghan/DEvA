#pragma once

namespace DEvA {
	namespace Common {
		template <typename Types>
		auto iteratorForNthListElement(auto& lst, size_t N) {
			size_t i = 0;
			auto it = lst.begin();
			while (it != lst.end() and i != N) {
				++it;
				++i;
			}
			return it;
		}
	}
}
