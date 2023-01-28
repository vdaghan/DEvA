#pragma once

#include "DEvA/Logger.h"
#include <DEvA/BuildingBlocks/StandardConvergenceCheckers.h>
#include <DEvA/BuildingBlocks/StandardGenePoolSelectors.h>
#include <DEvA/BuildingBlocks/StandardIndividualSorters.h>
#include <DEvA/BuildingBlocks/StandardInitialisers.h>
#include <DEvA/BuildingBlocks/StandardParentSelectors.h>
#include <DEvA/BuildingBlocks/StandardSurvivorSelectors.h>
#include <DEvA/BuildingBlocks/StandardTransforms.h>
#include <DEvA/BuildingBlocks/StandardVariations.h>

namespace DEvA {
	template <typename Types>
	void EvolutionaryAlgorithm<Types>::setupStandardFunctions() {
		functions.genesis.defineParametrisable("StdInitialiserEmpty", DEvA::StandardInitialisers<Types>::empty);
		functions.genesis.defineParametrisable("StdInitialiserPermutations", DEvA::StandardInitialisers<Types>::permutations);

		functions.transform.defineParametrised("StdTransformCopy", DEvA::StandardTransforms<Types>::copy, {});

		functions.genePoolSelection.defineParametrised("StdGenePoolSelectorAll", DEvA::StandardGenePoolSelectors<Types>::all, {});

		functions.parentSelection.defineParametrisable("StdParentSelectorBestNofM", DEvA::StandardParentSelectors<Types>::bestNofM);
		functions.parentSelection.defineParametrisable("StdParentSelectorRandomN", DEvA::StandardParentSelectors<Types>::randomN);
		functions.parentSelection.defineParametrisable("StdParentSelectorBestNofAll", DEvA::StandardParentSelectors<Types>::bestNofAll);

		functions.survivorSelection.defineParametrisable("StdSurvivorSelectorClamp", DEvA::StandardSurvivorSelectors<Types>::clamp);

		functions.sortIndividuals.defineParametrisable("StdIndividualSorterMetric", DEvA::StandardIndividualSorters<Types>::metricSort);

		functions.convergenceCheck.defineParametrisable("StdConvergenceCheckerEqualTo", DEvA::StandardConvergenceCheckers<Types>::equalTo);
		functions.convergenceCheck.defineParametrisable("StdConvergenceCheckerLessThan", DEvA::StandardConvergenceCheckers<Types>::lessThan);
		functions.convergenceCheck.defineParametrisable("StdConvergenceCheckerBiggerThan", DEvA::StandardConvergenceCheckers<Types>::biggerThan);
	}
}
