#pragma once

#include "DEvA/Common.h"

#include <map>
#include <set>
#include <string>

namespace DEvA {
	struct Dependency {
		std::function<bool()> condition;
		std::function<void()> action;
		inline bool isSatisfied() {
			if (not satisfied and condition()) {
				satisfied = true;
				action();
			}
			return satisfied;
		}
		bool satisfied{ false };
	};

	using Dependencies = std::list<Dependency>;
}