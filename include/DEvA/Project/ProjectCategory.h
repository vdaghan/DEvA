#pragma once

#include <list>
#include <string>

namespace DEvA {
	struct ProjectCategory {
		std::string name{};
		std::list<ProjectCategory> subCategories;
	};
}
