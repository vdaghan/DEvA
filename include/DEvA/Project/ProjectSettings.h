#pragma once

#include <string>

namespace DEvA {
	enum class TargetType {Folder};
	struct ProjectSettings {
		std::string projectName{};
		TargetType targetType{};
		std::string target{};
	};
}
