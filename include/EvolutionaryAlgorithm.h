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

			void setGenesisFunction(Types::FGenesis gfunc) { genesisFunction = gfunc; };
			void setTransformFunction(Types::FTransform tfunc) { transformFunction = tfunc; };
			void setEvaluationFunction(Types::FEvaluate efunc) { evaluationFunction = efunc; };
			void setParentSelectionFunction(Types::FParentSelection psfunc) { parentSelectionFunction = psfunc; };
			void setVariationFunction(Types::FVariation vfunc) { variationFunction = vfunc; };
			void setSurvivorSelectionFunction(Types::FSurvivorSelection ssfunc) { survivorSelectionFunction = ssfunc; };
			void setConvergenceCheckFunction(Types::FConvergenceCheck ccfunc) { convergenceCheckFunction = ccfunc; };

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
						//std::transform(parents.begin(), parents.end(), parentGenotypes.end(), [](Types::IndividualPtr ind) { return ind->genotype;});
						for (auto it = parents.begin(); it != parents.end(); ++it) {
							parentGenotypes.push_back((*it)->genotype);
						}
						typename Types::GenotypePtrs newGenotypes = variationFunction(parentGenotypes);
						typename Types::IndividualPtrs newOffsprings{};
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
			Types::FGenesis genesisFunction;
			Types::FTransform transformFunction;
			Types::FEvaluate evaluationFunction;
			Types::FParentSelection parentSelectionFunction;
			Types::FVariation variationFunction;
			Types::FSurvivorSelection survivorSelectionFunction;
			Types::FConvergenceCheck convergenceCheckFunction;

	};
}
