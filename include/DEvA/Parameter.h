#pragma once

#include <any>
#include <map>
#include <optional>
#include <string>

namespace DEvA {
	struct Parameter {
		//Parameter() = delete;
		Parameter(std::string pName) : name(pName) {}
		Parameter(std::string pName, std::any pValue) : name(pName), value(pValue) {}

		std::string name;
		std::any value;
	};
	using MaybeParameter = std::optional<Parameter>;
}