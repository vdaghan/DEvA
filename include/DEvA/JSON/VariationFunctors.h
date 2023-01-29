#pragma once

#include <DEvA/Functions.h>
#include <DEvA/VariationFunctor.h>
#include <DEvA/VariationFunctors.h>
#include <DEvA/JSON/Common.h>

namespace DEvA {
	template <typename Types>
	void importVariationFunctors(JSON & j, Functions<Types> & functions, VariationFunctors<Types> & variationFunctors) {
		if (not j.contains("variationFunctors")) {
			return;
		}
		auto & variationFunctorsJSON(j.at("variationFunctors"));
		if (variationFunctorsJSON.contains("definitions")) {
			auto & definitions(variationFunctorsJSON.at("definitions"));
			for (auto definitionIt = definitions.begin(); definitionIt != definitions.end(); ++definitionIt) {
				std::string functorName(definitionIt.key());
				auto & parameters(definitionIt.value());
				variationFunctors.defineVariationFunctor(functorName, parameters, functions);
			}
		}
		if (variationFunctorsJSON.contains("used")) {
			auto & usedVariationFunctors(variationFunctorsJSON.at("used"));
			for (auto & variationFunctor : usedVariationFunctors) {
				variationFunctors.use({variationFunctor});
			}
		}
	}
}
