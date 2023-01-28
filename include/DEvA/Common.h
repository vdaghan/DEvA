#pragma once

#include <DEvA/JSON/Common.h>

#include <map>
#include <string>

namespace DEvA {
	using ParameterMap = std::map<std::string, JSON>;
}

namespace DEvA::Common {
	template <typename Types>
	auto iteratorForNthListElement(auto & lst, size_t N) {
		size_t i = 0;
		auto it = lst.begin();
		while (it != lst.end() and i != N) {
			++it;
			++i;
		}
		return it;
	}
}
