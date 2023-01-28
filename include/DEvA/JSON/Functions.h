#pragma once

#include <DEvA/Functions.h>
#include <DEvA/JSON/Common.h>

namespace DEvA {
	template <typename Types>
	void importFunctions(JSON & j, Functions<Types> & functions) {
		if (not j.contains("functions")) {
			return;
		}
		auto & functionsJSON(j.at("functions"));
		if (functionsJSON.contains("definitions")) {
			auto & definitions(functionsJSON.at("definitions"));
			for (auto definitionIt = definitions.begin(); definitionIt != definitions.end(); ++definitionIt) {
				std::string functionType(definitionIt.key());
				auto & functionsOfType(definitionIt.value());
				auto defineFunctionLambda = [&](std::string type, auto & functionTemplate) {
					if (type == functionType) {
						for (auto functionIt(functionsOfType.begin()); functionIt != functionsOfType.end(); ++functionIt) {
							auto const & functionName(functionIt.key());
							auto const & functionValue(functionIt.value());
							auto const & parametrisableFunctionName(functionValue.at("function"));
							if (not functionTemplate.parametrisable.contains(parametrisableFunctionName)) {
								continue;
							}
							auto parametrisableFunction(functionTemplate.parametrisable.at(parametrisableFunctionName));
							functionTemplate.defineParametrised(functionName, parametrisableFunction, functionValue.at("parameters"));
						}
					}
				};
				defineFunctionLambda("genesisFunctions", functions.genesis);
				defineFunctionLambda("transformFunctions", functions.transform);
				defineFunctionLambda("survivorSelectors", functions.survivorSelection);
				defineFunctionLambda("individualSortFunctions", functions.sortIndividuals);
				defineFunctionLambda("convergenceCheckFunctions", functions.convergenceCheck);
			}
		}
		if (functionsJSON.contains("used")) {
			auto & usedFunctionTypes(functionsJSON.at("used"));
			auto useLambda = [&](std::string functionType) {
				if (usedFunctionTypes.contains(functionType)) {
					auto & usedFunctions(usedFunctionTypes.at(functionType));
					for (auto & function : usedFunctions) {
						functions.use({ function });
					}
				}
			};
			useLambda("genesisFunctions");
			useLambda("genePoolSelectionFunctions");
			useLambda("createGenotypeFunctions");
			useLambda("transformFunctions");
			useLambda("parentSelectionFunctions");
			useLambda("variationFromGenotypeFunctions");
			useLambda("variationFromIndividualPtrsFunctions");
			useLambda("survivorSelectionFunctions");
			useLambda("individualSortFunctions");
			useLambda("convergenceCheckFunctions");
		}
	}
}
