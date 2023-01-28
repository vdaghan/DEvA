#pragma once

#include <DEvA/Common.h>
#include <DEvA/Functions.h>
#include <DEvA/VariationFunctor.h>

#include <any>
#include <initializer_list>
#include <list>
#include <map>
#include <set>
#include <string>
#include <type_traits>
#include <utility>

namespace DEvA {
	template <typename Types>
	struct VariationFunctors {
		void defineVariationFunctor(std::string name, ParameterMap parameters, Functions<Types>& functions) {
			VariationFunctor<Types> variationFunctor{};
			variationFunctor.name = name;
			variationFunctor.numberOfParents = parameters.at("numberOfParents").get<std::size_t>();
			variationFunctor.probability = parameters.at("probability").get<double>();
			variationFunctor.removeParentsFromMatingPool = parameters.at("removeParentsFromMatingPool").get<bool>();
			std::string parentSelectionFunctionName(parameters.at("parentSelectionFunction").get<std::string>());
			variationFunctor.parentSelectionFunction = functions.parentSelectionFunctions.at(parentSelectionFunctionName);
			if (parameters.contains("variationFromGenotypesFunction")) {
				std::string variationFromGenotypesFunctionName(parameters.at("variationFromGenotypesFunction").get<std::string>());
				variationFunctor.variationFunctionFromGenotypes = functions.variationFromGenotypeFunctions.at(variationFromGenotypesFunctionName);
			}
			if (parameters.contains("variationFromIndividualPtrsFunction")) {
				std::string variationFromIndividualPtrsFunctionName(parameters.at("variationFromIndividualPtrsFunction").get<std::string>());
				variationFunctor.variationFunctionFromIndividualPtrs = functions.variationFromIndividualPtrsFunctions.at(variationFromIndividualPtrsFunctionName);
			}
			functors.emplace(std::make_pair(name, variationFunctor));
		}

		void use(std::initializer_list<std::string> names) {
			for (auto& name : names) {
				used.emplace(name);
			}
		}

		std::map<std::string, VariationFunctor<Types>> functors;
		std::set<std::string> used;
	};
}
