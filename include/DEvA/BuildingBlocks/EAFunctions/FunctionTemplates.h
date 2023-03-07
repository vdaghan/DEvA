#pragma once

#include <DEvA/Common.h>
#include <DEvA/Error.h>
#include "DEvA/BuildingBlocks/Dependency/Dependency.h"

#include <any>
#include <initializer_list>
#include <functional>
#include <list>
#include <map>
#include <set>
#include <string>
#include <type_traits>
#include <utility>

namespace DEvA {
	template <typename B>
	struct FunctionTemplates {
		using Blueprint = B;
		using Function = typename Blueprint::Function;
		using Parametrisable = typename Blueprint::Parametrisable;
		void defineParametrisable(std::string name, Parametrisable function) {
			parametrisable.emplace(name, function);
		}
		bool containsParametrisable(std::string name) const {
			return parametrisable.contains(name);
		}
		void defineParametrised(std::string name, Parametrisable function, ParameterMap parameters) {
			parametrised.emplace(name, std::bind_front(function, parameters));
		}
		bool defineParametrised(std::string parametrisedName, std::string parametrisableName, ParameterMap parameters) {
			if (not containsParametrisable(parametrisableName)) {
				return false;
			}
			defineParametrised(parametrisedName, parametrisable.at(parametrisableName), parameters);
			return true;
		}
		bool containsParametrised(std::string name) const {
			return parametrised.contains(name);
		}
		void definePlain(std::string name, Function function) {
			parametrised.emplace(name, function);
		}
		Function getParametrised(std::string name) {
			return tryWrap(parametrised.at(name));
		}
		Function tryWrap(Function f) {
			if (wrapper) {
				return std::bind_front(wrapper, f);
			} else {
				return f;
			}

		}
		std::map<std::string, Parametrisable> parametrisable{};
		std::map<std::string, Function> parametrised{};
		typename Blueprint::Wrapper wrapper;
	};
}
