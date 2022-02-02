#pragma once

#include <any>
#include <optional>
#include <string>
#include <vector>

class Parameter {
	public:
		Parameter(std::string pName, std::any pValue) {
			name_ = pName;
			value_ = pValue;
		}

		std::string_view name() const { return name_; };
		std::any value() const { return value_; };
	private:
		std::string name_;
		std::any value_;
};

typedef std::vector<Parameter> Parameters;
typedef std::optional<Parameter> MaybeParameter;
