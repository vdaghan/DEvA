#pragma once

#include <DEvA/Common.h>

#include <any>
#include <initializer_list>
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

		void use(std::initializer_list<std::string> names) {
			for (auto & name : names) {
				if (genesis.parametrised.contains(name)) {
					genesis.use(name);
				}
				else if (genePoolSelection.parametrised.contains(name)) {
					genePoolSelection.use(name);
				}
				else if (createGenotype.parametrised.contains(name)) {
					createGenotype.use(name);
				}
				else if (transform.parametrised.contains(name)) {
					transform.use(name);
				}
				else if (parentSelection.parametrised.contains(name)) {
					parentSelection.use(name);
				}
				else if (variationFromGenotypes.parametrised.contains(name)) {
					variationFromGenotypes.use(name);
				}
				else if (variationFromIndividualPtrs.parametrised.contains(name)) {
					variationFromIndividualPtrs.use(name);
				}
				else if (survivorSelection.parametrised.contains(name)) {
					survivorSelection.use(name);
				}
				else if (sortIndividuals.parametrised.contains(name)) {
					sortIndividuals.use(name);
				}
				else if (convergenceCheck.parametrised.contains(name)) {
					convergenceCheck.use(name);
				}
			}
		}

		typename Types::FGenesis getGenesis() {
			return [&]() {
				typename Types::Genotypes genotypes{};
				for (auto & name : genesis.usedSet) {
					auto & g(genesis.parametrised.at(name));
					auto genotypesFromName(g());
					genotypes.insert(genotypes.end(), genotypesFromName.begin(), genotypesFromName.end());
				}
				return genotypes;
			};
		}
		typename Types::FTransform getTransform() {
			return transform.parametrised.at(transform.used);
		}
		typename Types::FSurvivorSelection getSurvivorSelection() {
			return survivorSelection.parametrised.at(survivorSelection.used);
		}
		typename Types::FSortIndividuals getSortIndividuals() {
			return sortIndividuals.parametrised.at(sortIndividuals.used);
		}
		typename Types::FConvergenceCheck getConvergenceCheck() {
			return convergenceCheck.parametrised.at(convergenceCheck.used);
		}
	};
}
