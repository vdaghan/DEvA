#pragma once

#include <DEvA/Common.h>
#include <DEvA/Error.h>
#include "DEvA/BuildingBlocks/Dependency/Dependency.h"
#include "DEvA/BuildingBlocks/EAFunctions/FunctionComposer.h"

#include <any>
#include <initializer_list>
#include <functional>
#include <list>
#include <map>
#include <set>
#include <string>
#include <typeindex>
#include <type_traits>
#include <utility>

namespace DEvA {
	template <typename Types>
	struct Functions {
		FunctionComposer<typename Types::BPGenesis> genesis;
		FunctionComposer<typename Types::BPGenePoolSelection> genePoolSelection;
		FunctionComposer<typename Types::BPCreateGenotype> createGenotype;
		FunctionComposer<typename Types::BPTransform> transform;
		FunctionComposer<typename Types::BPParentSelection> parentSelection;
		FunctionComposer<typename Types::BPVariationFromGenotypes> variationFromGenotypes;
		FunctionComposer<typename Types::BPVariationFromIndividualPtrs> variationFromIndividualPtrs;
		FunctionComposer<typename Types::BPSurvivorSelection> survivorSelection;
		FunctionComposer<typename Types::BPSortIndividuals> sortIndividuals;
		FunctionComposer<typename Types::BPConvergenceCheck> convergenceCheck;

		Dependencies getDependencies() {
			Dependencies allDependencies{};

			auto addDependencyLambda = [&](auto & functionComposer) {
				if (functionComposer.dependency) {
					allDependencies.push_back(functionComposer.dependency.value());
				}
			};

			addDependencyLambda(genesis);
			addDependencyLambda(genePoolSelection);
			addDependencyLambda(createGenotype);
			addDependencyLambda(transform);
			addDependencyLambda(parentSelection);
			addDependencyLambda(variationFromGenotypes);
			addDependencyLambda(variationFromIndividualPtrs);
			addDependencyLambda(survivorSelection);
			addDependencyLambda(sortIndividuals);
			addDependencyLambda(convergenceCheck);

			return allDependencies;
		}
	};
}
