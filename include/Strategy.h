#pragma once

#include <functional>

#include "Generation.h"
#include "IdentityFunctions.h"
#include "StrategyFunctions.h"

enum class Strategy_ParentRejection { CUSTOM, NONE, WORST_N, WORST_PERCENTAGE };
enum class Strategy_ParentGrouping { CUSTOM, NONE };

/// Parent selection strategies define who are eligible to be a parent:
/// CUSTOM: User provided strategy
/// ALL: Whole previous generation is a candidate
/// FITNESS_PROPORTIONAL: 
/// FITNESS_PROPORTIONAL_WINDOWED: 
enum class Strategy_ParentSelection { CUSTOM, ALL, FITNESS_PROPORTIONAL, FITNESS_PROPORTIONAL_WINDOWED, TOURNAMENT };

/// Partner selection strategies define who will be mating with whom
/// CUSTOM: User provided strategy
/// FURTHEST: 
/// RANDOM: 
enum class Strategy_PartnerSelection { CUSTOM, FURTHEST, RANDOM };

/// Population size strategies define how the population size is maintained
/// CUSTOM: User provided strategy
/// AUTOMATIC: 
/// FIXED: Fixed population size
enum class Strategy_PopulationSize { CUSTOM, AUTOMATIC, FIXED };

/// Population model strategies ...
/// CUSTOM: User provided strategy
/// FULLSTEADYSTATE: 
/// PARTIALSTEADYSTATE: 
/// GENERATIONAL: 
/// GAVaPS: 
enum class Strategy_PopulationModel { CUSTOM, FULLSTEADYSTATE, PARTIALSTEADYSTATE, GENERATIONAL, GAVaPS };

/// Survivor selection strategies define who will survive to make up the next generation
/// CUSTOM: User provided strategy
/// REPLACE_WORST: 
/// AGE_BASED: 
/// FITNESS_BASED: 
enum class Strategy_SurvivorSelection { CUSTOM, REPLACE_WORST, AGE_BASED, FITNESS_BASED };

class Strategy {
	public:
		void setStrategy(Strategy_ParentRejection, ParentSelectionFunction = IdentityParentSelection);
		void setStrategy(Strategy_ParentGrouping, ParentSelectionFunction = IdentityParentSelection);
		void setStrategy(Strategy_ParentSelection, ParentSelectionFunction = IdentityParentSelection);
		GenerationPtr apply(GenerationPtr const) const;
	private:
		Strategy_ParentRejection parentRejectionStrategy;
		Strategy_ParentGrouping parentGroupingStrategy;
		Strategy_ParentSelection parentSelectionStrategy;
		Strategy_PartnerSelection partnerSelectionStrategy;
		Strategy_PopulationSize populationSizeStrategy;
		Strategy_PopulationModel populationModelStrategy;
		Strategy_SurvivorSelection survivorSelectionStrategy;

		IndividualPtrList const selectParents(IndividualPtrList const) const;
		std::vector<IndividualPtrList> selectPartners(IndividualPtrList const);
		IndividualPtrList createOffsprings(std::vector<IndividualPtrList> const) const;
		IndividualPtrList selectSurvivors(IndividualPtrList const) const;
};
