#pragma once

#include <DEvA/Common.h>
#include <DEvA/Functions.h>
#include <DEvA/VariationFunctor.h>
#include "DEvA/BuildingBlocks/Dependency/Dependency.h"

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
			functors.emplace(std::make_pair(name, variationFunctor));
			
			auto & variationFunctorRef(functors.at(name));
			std::string parentSelectionFunctionName(parameters.at("parentSelection").get<std::string>());
			dependencies.emplace_back(Dependency{
				.explanation = "VariationFunctor::parentSelectionFunction: " + parentSelectionFunctionName,
				.condition = [&, parentSelectionFunctionName]() {
					return functions.parentSelection.containsParametrised(parentSelectionFunctionName);
				},
				.action = [&, parentSelectionFunctionName]() {
					variationFunctorRef.parentSelectionFunction = functions.parentSelection.getParametrised(parentSelectionFunctionName);
				}
			});
			if (parameters.contains("variationFromGenotypes")) {
				std::string variationFromGenotypesFunctionName(parameters.at("variationFromGenotypes").get<std::string>());
				dependencies.emplace_back(Dependency{
					.explanation = "VariationFunctor::variationFunctionFromGenotypes: " + variationFromGenotypesFunctionName,
					.condition = [&, variationFromGenotypesFunctionName]() {
						return functions.variationFromGenotypes.containsParametrised(variationFromGenotypesFunctionName);
					},
					.action = [&, variationFromGenotypesFunctionName]() {
						variationFunctorRef.variationFunctionFromGenotypes = functions.variationFromGenotypes.getParametrised(variationFromGenotypesFunctionName);
					}
				});
			}
			if (parameters.contains("variationFromIndividualPtrs")) {
				std::string variationFromIndividualPtrsFunctionName(parameters.at("variationFromIndividualPtrs").get<std::string>());
				dependencies.emplace_back(Dependency{
					.explanation = "VariationFunctor::variationFunctionFromIndividualPtrs: " + variationFromIndividualPtrsFunctionName,
					.condition = [&, variationFromIndividualPtrsFunctionName]() {
						return functions.variationFromIndividualPtrs.containsParametrised(variationFromIndividualPtrsFunctionName);
					},
					.action = [&, variationFromIndividualPtrsFunctionName]() {
						variationFunctorRef.variationFunctionFromIndividualPtrs = functions.variationFromIndividualPtrs.getParametrised(variationFromIndividualPtrsFunctionName);
					}
				});
			}
		}

		void use(std::initializer_list<std::string> names) {
			for (auto& name : names) {
				used.emplace(name);
			}
		}

		std::map<std::string, VariationFunctor<Types>> functors;
		std::set<std::string> used;

		Dependencies dependencies{};
	};
}
