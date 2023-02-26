#pragma once

#include <DEvA/Common.h>
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
	template <typename Types, typename PFunctionType, typename FunctionType>
	struct FunctionTemplates {
		void defineParametrisable(std::string name, PFunctionType function) {
			parametrisable.emplace(name, function);
		}
		void defineParametrised(std::string name, PFunctionType function, ParameterMap parameters) {
			parametrised.emplace(name, std::bind_front(function, parameters));
		}
		void define(std::string name, FunctionType function) {
			parametrised.emplace(name, function);
		}
		void use(std::string name) {
			used = name;
			usedSet.emplace(name);
		}
		std::map<std::string, PFunctionType> parametrisable{};
		std::map<std::string, FunctionType> parametrised{};
		std::set<std::string> usedSet{};
		std::string used{};
	};

	template <typename Types>
	struct Functions {
		FunctionTemplates<Types, typename Types::FPGenesis, typename Types::FGenesis> genesis;
		FunctionTemplates<Types, typename Types::FPGenePoolSelection, typename Types::FGenePoolSelection> genePoolSelection;
		FunctionTemplates<Types, typename Types::FPCreateGenotype, typename Types::FCreateGenotype> createGenotype;
		FunctionTemplates<Types, typename Types::FPTransform, typename Types::FTransform> transform;
		FunctionTemplates<Types, typename Types::FPParentSelection, typename Types::FParentSelection> parentSelection;
		FunctionTemplates<Types, typename Types::FPVariationFromGenotypes, typename Types::FVariationFromGenotypes> variationFromGenotypes;
		FunctionTemplates<Types, typename Types::FPVariationFromIndividualPtrs, typename Types::FVariationFromIndividualPtrs> variationFromIndividualPtrs;
		FunctionTemplates<Types, typename Types::FPSurvivorSelection, typename Types::FSurvivorSelection> survivorSelection;
		FunctionTemplates<Types, typename Types::FPSortIndividuals, typename Types::FSortIndividuals> sortIndividuals;
		FunctionTemplates<Types, typename Types::FPConvergenceCheck, typename Types::FConvergenceCheck> convergenceCheck;

		typename Types::FWGenesis genesisWrapper;
		typename Types::FWGenePoolSelection genePoolSelectionWrapper;
		typename Types::FWCreateGenotype createGenotypeWrapper;
		typename Types::FWTransform transformWrapper;
		typename Types::FWParentSelection parentSelectionWrapper;
		typename Types::FWVariationFromGenotypes variationFromGenotypesWrapper;
		typename Types::FWVariationFromIndividualPtrs variationFromIndividualPtrsWrapper;
		typename Types::FWSurvivorSelection survivorSelectionWrapper;
		typename Types::FWSortIndividuals sortIndividualsWrapper;
		typename Types::FWConvergenceCheck convergenceCheckWrapper;

		void use(std::initializer_list<std::string> names) {
			for (auto & name : names) {
				dependencies.emplace_back(Dependency{
					.condition = [&, name]() {
						auto checkLambda = [&, name](auto & container) {
							if (container.parametrised.contains(name)) {
								return true;
							}
							return false;
						};
						return checkLambda(genesis)
							|| checkLambda(genePoolSelection)
							|| checkLambda(createGenotype)
							|| checkLambda(transform)
							|| checkLambda(parentSelection)
							|| checkLambda(variationFromGenotypes)
							|| checkLambda(variationFromIndividualPtrs)
							|| checkLambda(survivorSelection)
							|| checkLambda(sortIndividuals)
							|| checkLambda(convergenceCheck);
					},
					.action = [&, name]() {
						auto useLambda = [&, name](auto & container) {
							if (container.parametrised.contains(name)) {
								container.use(name);
								return true;
							}
							return false;
						};
						return useLambda(genesis)
							|| useLambda(genePoolSelection)
							|| useLambda(createGenotype)
							|| useLambda(transform)
							|| useLambda(parentSelection)
							|| useLambda(variationFromGenotypes)
							|| useLambda(variationFromIndividualPtrs)
							|| useLambda(survivorSelection)
							|| useLambda(sortIndividuals)
							|| useLambda(convergenceCheck);
					}
				});
			}
		}

		typename Types::FGenesis getGenesis() {
			return [&]() {
				typename Types::Genotypes genotypes{};
				for (auto & name : genesis.usedSet) {
					auto g(genesis.parametrised.at(name));
					if (genesisWrapper) {
						g = std::bind_front(genesisWrapper, g);
					}
					auto genotypesFromName(g());
					genotypes.insert(genotypes.end(), genotypesFromName.begin(), genotypesFromName.end());
				}
				return genotypes;
			};
		}
		typename Types::FTransform getTransform() {
			if (transformWrapper) {
				return std::bind_front(transformWrapper, transform.parametrised.at(transform.used));
			}
			return transform.parametrised.at(transform.used);
		}
		typename Types::FSurvivorSelection getSurvivorSelection() {
			if (survivorSelectionWrapper) {
				return std::bind_front(survivorSelectionWrapper, survivorSelection.parametrised.at(survivorSelection.used));
			}
			return survivorSelection.parametrised.at(survivorSelection.used);
		}
		typename Types::FSortIndividuals getSortIndividuals() {
			if (sortIndividualsWrapper) {
				return std::bind_front(sortIndividualsWrapper, sortIndividuals.parametrised.at(sortIndividuals.used));
			}
			return sortIndividuals.parametrised.at(sortIndividuals.used);
		}
		typename Types::FConvergenceCheck getConvergenceCheck() {
			if (convergenceCheckWrapper) {
				return std::bind_front(convergenceCheckWrapper, convergenceCheck.parametrised.at(convergenceCheck.used));
			}
			return convergenceCheck.parametrised.at(convergenceCheck.used);
		}
		typename Types::FVariationFromGenotypes getVariationFromGenotypes(std::string functionName) {
			if (variationFromGenotypesWrapper) {
				return std::bind_front(variationFromGenotypesWrapper, variationFromGenotypes.parametrised.at(functionName));
			}
			return variationFromGenotypes.parametrised.at(functionName);
		}
		typename Types::FVariationFromIndividualPtrs getVariationFromIndividualPtrs(std::string functionName) {
			if (variationFromIndividualPtrsWrapper) {
				return std::bind_front(variationFromIndividualPtrsWrapper, variationFromIndividualPtrs.parametrised.at(functionName));
			}
			return variationFromIndividualPtrs.parametrised.at(functionName);
		}

		Dependencies dependencies{};
	};
}
