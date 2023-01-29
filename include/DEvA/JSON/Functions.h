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
				defineFunctionLambda("genesis", functions.genesis);
				defineFunctionLambda("genePoolSelection", functions.genePoolSelection);
				defineFunctionLambda("createGenotype", functions.createGenotype);
				defineFunctionLambda("transform", functions.transform);
				defineFunctionLambda("parentSelection", functions.parentSelection);
				defineFunctionLambda("variationFromGenotype", functions.variationFromGenotypes);
				defineFunctionLambda("variationFromIndividualPtrs", functions.variationFromIndividualPtrs);
				defineFunctionLambda("survivorSelection", functions.survivorSelection);
				defineFunctionLambda("individualSort", functions.sortIndividuals);
				defineFunctionLambda("convergenceCheck", functions.convergenceCheck);
			}
		}
		if (functionsJSON.contains("used")) {
			auto & usedFunctions(functionsJSON.at("used"));
			for (auto& function : usedFunctions) {
				functions.use({ function });
			}
		}
	}
}
