#pragma once

#include <optional>
#include <string>

#include "DEvA/Parameter.h"

namespace DEvA {
	struct Parameters {
		MaybeParameter getParameter(std::string const & name) {
			if (parameters.contains(name)) {
				Parameter retVal = parameters.at(name);
				return retVal;
			}
			return std::nullopt;
		};
		Parameter& operator[](std::string const & name) {
			if (not parameters.contains(name)) {
				Parameter newparam(name);
				parameters.emplace(std::make_pair(name, newparam));
			}
			return parameters.at(name);
		}

		void setParameter(std::string const & name, Parameter parameter) {
			parameter.name = name;
			(*this)[name] = parameter;
		};
		void setParameter(Parameter const & parameter) {
			operator[](parameter.name) = parameter;
		};
		std::map<std::string, Parameter> parameters;
	};
}
