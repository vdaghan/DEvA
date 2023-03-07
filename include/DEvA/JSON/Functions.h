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
				auto defineFunctionLambda = [&](std::string type, auto & functionComposer) {
					if (not (type == functionType)) {
						return;
					}
					for (auto functionIt(functionsOfType.begin()); functionIt != functionsOfType.end(); ++functionIt) {
						auto const & functionName(functionIt.key());
						auto const & functionValue(functionIt.value());
						auto const & parametrisableFunctionName(functionValue.at("function"));
						functionComposer.defineParametrised(functionName, parametrisableFunctionName, functionValue.at("parameters"));
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
		if (functionsJSON.contains("compile")) {
			auto & compilations(functionsJSON.at("compile"));
			for (auto compilationIt = compilations.begin(); compilationIt != compilations.end(); ++compilationIt) {
				std::string functionType(compilationIt.key());
				auto compilationRecipe(compilationIt.value().get<std::vector<std::string>>());
				auto compileFunctionLambda = [&](std::string type, auto & functionComposer) {
					if (not (type == functionType)) {
						return;
					}
					functionComposer.compile(type+"Compiled", compilationRecipe);
				};
				compileFunctionLambda("genesis", functions.genesis);
				compileFunctionLambda("genePoolSelection", functions.genePoolSelection);
				compileFunctionLambda("createGenotype", functions.createGenotype);
				compileFunctionLambda("transform", functions.transform);
				compileFunctionLambda("parentSelection", functions.parentSelection);
				compileFunctionLambda("variationFromGenotype", functions.variationFromGenotypes);
				compileFunctionLambda("variationFromIndividualPtrs", functions.variationFromIndividualPtrs);
				compileFunctionLambda("survivorSelection", functions.survivorSelection);
				compileFunctionLambda("individualSort", functions.sortIndividuals);
				compileFunctionLambda("convergenceCheck", functions.convergenceCheck);
			}
		}
	}
}
