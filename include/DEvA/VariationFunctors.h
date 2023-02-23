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
			std::string parentSelectionFunctionName(parameters.at("parentSelection").get<std::string>());
			variationFunctor.parentSelectionFunction = functions.parentSelection.parametrised.at(parentSelectionFunctionName);
			if (parameters.contains("variationFromGenotypes")) {
				std::string variationFromGenotypesFunctionName(parameters.at("variationFromGenotypes").get<std::string>());
				variationFunctor.variationFunctionFromGenotypes = functions.getVariationFromGenotypes(variationFromGenotypesFunctionName);
			}
			if (parameters.contains("variationFromIndividualPtrs")) {
				std::string variationFromIndividualPtrsFunctionName(parameters.at("variationFromIndividualPtrs").get<std::string>());
				variationFunctor.variationFunctionFromIndividualPtrs = functions.getVariationFromIndividualPtrs(variationFromIndividualPtrsFunctionName);
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
