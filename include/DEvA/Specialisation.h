#pragma once

#include <deque>
#include <map>
#include <memory>
#include <string>
#include <variant>

#include "DEvA/Common.h"
#include "DEvA/Concepts.h"

#include "DEvA/EAStatistics.h"
#include "DEvA/Error.h"
#include "DEvA/EvolutionaryAlgorithm.h"
#include "DEvA/Functions.h"
#include "DEvA/Individual.h"
#include "DEvA/IndividualIdentifier.h"
#include "DEvA/Metric.h"
#include "DEvA/Parameters.h"
#include "DEvA/BuildingBlocks/EAFunctions/FunctionBlueprint.h"
#include "DEvA/BuildingBlocks/StandardConvergenceCheckers.h"
#include "DEvA/BuildingBlocks/StandardInitialisers.h"
#include "DEvA/BuildingBlocks/StandardParentSelectors.h"
#include "DEvA/BuildingBlocks/StandardSurvivorSelectors.h"
#include "DEvA/BuildingBlocks/StandardTransforms.h"
#include "DEvA/BuildingBlocks/StandardVariations.h"
#include "DEvA/VariationFunctor.h"

#include <DEvA/JSON/Common.h>

namespace DEvA {

    template <typename T> class EvolutionaryAlgorithm;
    template <typename T> struct Individual;
    template <typename T> struct StandardInitialisers;
    template <typename T> struct StandardVariations;
    template <typename T> struct StandardTransforms;

    template <typename T>
    struct Parametrised {
        T value;
        Parameters parameters;
    };

    template <CSpecification S>
    struct Specialisation {
        using F = S;
        S f;
        // Repeat basic types used throughout code for better UX
        using Spec = Specialisation<S>;
        using Genotype = typename S::Genotype;
        using Genotypes = std::list<Genotype>;
        using Phenotype = typename S::Phenotype;
        using MaybePhenotype = Maybe<Phenotype>;

        // Repeat non-POD types used throughout code for better UX
        using SEvolutionaryAlgorithm = EvolutionaryAlgorithm<Spec>;
        using SIndividual = Individual<Spec>;

        using IndividualPtr = std::shared_ptr<SIndividual>;
        using IndividualPtrs = std::list<IndividualPtr>;
        using IndividualWPtr = std::weak_ptr<SIndividual>;
        using IndividualWPtrs = std::list<IndividualWPtr>;

        using Generation = IndividualPtrs;
        using Generations = std::deque<Generation>;
        using Genealogy = Generations;

        using SStandardVariations = StandardVariations<Spec>;

        using SVariationInfo = VariationInfo<Spec>;
        using SMaybeVariationInfo = Maybe<VariationInfo<Spec>>;
        using SVariationFunctor = VariationFunctor<Spec>;

        // Metric types
        using SMetric = Metric<Spec>;
        using SMetricMap = MetricMap<Spec>;
        using SMetricFunctor = MetricFunctor<Spec>;

        // EA function blueprints
        using BPGenesis = FunctionBlueprint<Genotypes>;
        using BPGenePoolSelection = FunctionBlueprint<Generation, Generation>;
        using BPCreateGenotype = FunctionBlueprint<Genotype>;
        using BPTransform = FunctionBlueprint<MaybePhenotype, Genotype>;
        using BPParentSelection = FunctionBlueprint<IndividualPtrs, IndividualPtrs>;
        using BPVariationFromGenotypes = FunctionBlueprint<Genotypes, Genotypes>;
        using BPVariationFromIndividualPtrs = FunctionBlueprint<Genotypes, IndividualPtrs>;
        using BPSurvivorSelection = FunctionBlueprint<IndividualPtrs, IndividualPtrs>;
        using BPSortIndividuals = FunctionBlueprint<bool, IndividualPtr, IndividualPtr>;
        using BPConvergenceCheck = FunctionBlueprint<bool, SMetricMap const &>;

        // Callbacks
        using CVoid = std::function<void()>;
        using CEAStatsUpdate = std::function<void(EAStatistics<Spec> const &, EAStatisticsUpdateType)>;
        using COnEpoch = std::function<void(std::size_t)>;
        using COnVariation = std::function<void(VariationInfo<Spec> const &)>;
        using COnEvaluate = std::function<void(IndividualIdentifier)>;
        using CVariant = std::variant<
            CVoid,
            CEAStatsUpdate,
            COnEpoch,
            COnVariation,
            COnEvaluate>;
        using CVariantMap = std::map<Callback, CVariant>;
    };
}
