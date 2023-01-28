#pragma once

#include <DEvA/VariationFunctors.h>
#include <DEvA/JSON/Common.h>

namespace DEvA {
	template <typename Types>
	void importVariationFunctors(JSON & j, VariationFunctors<Types>& variationFunctors) {
		if (not j.contains("variationFunctors")) {
			return;
		}
		auto & variationFunctorsJSON(j.at("variationFunctors"));
		if (variationFunctorsJSON.contains("definitions")) {
			auto & definitions(variationFunctorsJSON.at("definitions"));
			for (auto definitionIt = definitions.begin(); definitionIt != definitions.end(); ++definitionIt) {
				std::string functionType(definitionIt.key());
				auto & functionsOfType(definitionIt.value());
			}
		}
	}
}
