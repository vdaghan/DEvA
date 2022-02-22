#pragma once

#include "Genealogy.h"
#include "Individual.h"

#include <algorithm>
#include <iostream>
#include <memory>
#include <ranges>

namespace DEvA {
	enum class StepResult { StepCount, Convergence };

	template <typename Types>
	class EvolutionaryAlgorithm {
		public:
			using SGenealogy = Genealogy<Types>;
			using GenesisFunction = Types::FGenesis;
			using TransformFunction = Types::FTransform;
			using EvaluationFunction = Types::FEvaluate;
			using ParentSelectionFunction = Types::FParentSelection;
			using VariationFunction = Types::FVariation;
			using SurvivorSelectionFunction = Types::FSurvivorSelection;
			using ConvergenceCheckFunction = Types::FConvergenceCheck;

			void setGenesisFunction(GenesisFunction gfunc) { genesisFunction = gfunc; };
			void setTransformFunction(TransformFunction tfunc) { transformFunction = tfunc; };
			void setEvaluationFunction(EvaluationFunction efunc) { evaluationFunction = efunc; };
			void setParentSelectionFunction(ParentSelectionFunction psfunc) { parentSelectionFunction = psfunc; };
			void setVariationFunction(VariationFunction vfunc) { variationFunction = vfunc; };
			void setSurvivorSelectionFunction(SurvivorSelectionFunction ssfunc) { survivorSelectionFunction = ssfunc; };
			void setConvergenceCheckFunction(ConvergenceCheckFunction ccfunc) { convergenceCheckFunction = ccfunc; };

			StepResult search(size_t count) {
				for (size_t i(0); i < count; ++i) {
					typename Types::Generation newGeneration{};
					if (0 == genealogy.generations.size()) [[unlikely]] {
						genealogy += genesisFunction();
						std::cout << "Genesis: " << genealogy.generations.back().size() << " individuals.\n";
					} else [[likely]] {
						// Parent selection
						typename Types::IndividualPtrs parents = parentSelectionFunction(genealogy.generations.back());
						// Variation
						typename Types::GenotypePtrs parentGenotypes{};
						// TODO: This does not work; inspect why
						//std::transform(parents.begin(), parents.end(), newGenotypes.end(), [](Types::IndividualPtr& ind) { return ind->genotype;});
						for (auto it = parents.begin(); it != parents.end(); ++it) {
							parentGenotypes.push_back((*it)->genotype);
						}
						typename Types::GenotypePtrs newGenotypes{};
						newGenotypes = variationFunction(parentGenotypes);
						typename Types::IndividualPtrs newOffsprings{};
						// TODO: This does not work; inspect why
						//std::transform(newGenotypes.begin(), newGenotypes.end(), newOffsprings.begin(), [&](Types::GenotypePtr gptr) { return Individual<Types>(parents, gptr); });
						for (auto it = newGenotypes.begin(); it != newGenotypes.end(); ++it) {
							typename Types::IndividualPtr newIndividual = std::make_shared<Individual<Types>>(*it);
							newIndividual->setParents(parents);
							newOffsprings.emplace_back(newIndividual);
						}
						//std::cout << newOffsprings.size() << " new offsprings being added to gene pool.\n";
						newOffsprings.insert(newOffsprings.end(), genealogy.generations.back().begin(), genealogy.generations.back().end());
						genealogy += newOffsprings;
					}
					auto& lastGen(genealogy.generations.back());
					//std::cout << "Transforming...\n";
					std::for_each(lastGen.begin(), lastGen.end(), [&](auto& iptr) { iptr->phenotype = transformFunction(iptr->genotype);});
					//std::cout << "Evaluating...\n";
					std::for_each(lastGen.begin(), lastGen.end(), [&](auto& iptr) { iptr->fitness = evaluationFunction(iptr->phenotype);});
					//std::cout << "Sorting...\n";
					std::stable_sort(lastGen.begin(), lastGen.end(), [](auto iptr1, auto iptr2) -> bool { return ((iptr1->fitness) > (iptr2->fitness)); });

					// Survivor selection
					if (1 != genealogy.generations.size()) [[likely]] {
						survivorSelectionFunction(genealogy.generations.back());
					}
					auto bestIndividual = genealogy.generations.back().back();
					bestGenotype = bestIndividual->genotype;
					bestPhenotype = bestIndividual->phenotype;
					bestFitness = bestIndividual->fitness;
					//std::cout << "Best fitness: " << bestFitness << "\n";

					if (convergenceCheckFunction(bestFitness)) [[unlikely]] {
						return StepResult::Convergence;
					}
				}
				return StepResult::StepCount;
			};

			Types::GenotypePtr bestGenotype;
			Types::PhenotypePtr bestPhenotype;
			Types::Fitness bestFitness;
			SGenealogy genealogy;
		private:
			GenesisFunction genesisFunction;
			TransformFunction transformFunction;
			EvaluationFunction evaluationFunction;
			ParentSelectionFunction parentSelectionFunction;
			VariationFunction variationFunction;
			SurvivorSelectionFunction survivorSelectionFunction;
			ConvergenceCheckFunction convergenceCheckFunction;

	};
}
