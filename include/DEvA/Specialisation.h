#pragma once

#include <deque>
#include <initializer_list>
#include <map>
#include <memory>
#include <type_traits>

#include "DEvA/Concepts.h"

#include "DEvA/EAStatistics.h"
#include "DEvA/Error.h"
#include "DEvA/EvolutionaryAlgorithm.h"
#include "DEvA/Individual.h"
#include "DEvA/IndividualIdentifier.h"
#include "DEvA/Metric.h"
#include "DEvA/Parameters.h"
#include "DEvA/BuildingBlocks/StandardConvergenceCheckers.h"
#include "DEvA/BuildingBlocks/StandardInitialisers.h"
#include "DEvA/BuildingBlocks/StandardParentSelectors.h"
#include "DEvA/BuildingBlocks/StandardSurvivorSelectors.h"
#include "DEvA/BuildingBlocks/StandardTransforms.h"
#include "DEvA/BuildingBlocks/StandardVariations.h"
#include "DEvA/VariationFunctor.h"

namespace DEvA {
    template <typename T> class EvolutionaryAlgorithm;
    template <typename T, typename IP> class Individual;
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
        using Genotype = S::Genotype;
        using GenotypeProxy = S::GenotypeProxy;
        using GenotypeProxies = std::list<GenotypeProxy>;
        using GenotypeProxiesDeque = std::deque<GenotypeProxies>;
        using Phenotype = S::Phenotype;
        using PhenotypeProxy = S::PhenotypeProxy;
        using MaybePhenotypeProxy = Maybe<PhenotypeProxy>;
        using Distance = S::Distance;
        using DistanceMatrix = std::map<IndividualIdentifier, std::map<IndividualIdentifier, Distance>>;
        using IndividualParameters = S::IndividualParameters;

        using FGenotypePtrSet = std::function<GenotypeProxies(GenotypeProxies)>;
        using FGenotypePtrSetDeque = std::function<GenotypeProxiesDeque(GenotypeProxiesDeque)>;
        using DequeFGenotypePtrSet = std::deque<FGenotypePtrSet>;

        // Repeat non-POD types used throughout code for better UX
        using SEvolutionaryAlgorithm = EvolutionaryAlgorithm<Spec>;
        using SIndividual = Individual<Spec, IndividualParameters>;

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

        // Specification function types
        using FGenotypeFromProxy = std::function<Genotype & (GenotypeProxy)>;
        using FPhenotypeFromProxy = std::function<Phenotype & (PhenotypeProxy)>;

        // EA function types
        using FGenesis = std::function<GenotypeProxies(void)>;
        using FGenePoolSelection = std::function<Generation(Generation)>;
        using FCreateGenotype = std::function<GenotypeProxy(void)>;
        using FTransform = std::function<MaybePhenotypeProxy(GenotypeProxy)>;
        using FParentSelection = std::function<IndividualPtrs(IndividualPtrs)>;
        using FVariationFromGenotypeProxies = std::function<GenotypeProxies(GenotypeProxies)>;
        using FVariationFromIndividualPtrs = std::function<GenotypeProxies(IndividualPtrs)>;
        using FSurvivorSelection = std::function<void(IndividualPtrs&)>;
        using FSortIndividuals = std::function<bool(IndividualPtr, IndividualPtr)>;
        using FConvergenceCheck = std::function<bool(SMetricMap const &)>;
        using FVariant = std::variant<
            FGenesis,
            FGenePoolSelection,
            FCreateGenotype,
            FTransform,
            FParentSelection,
            FVariationFromGenotypeProxies,
            FVariationFromIndividualPtrs,
            FSurvivorSelection,
            FSortIndividuals,
            FConvergenceCheck>;
        using FVariantMap = std::map<EAFunction, FVariant>;

        // Callbacks
        using CVoid = std::function<void(void)>;
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

        struct RFGenotypeProxySet {
			GenotypeProxies domain;
			GenotypeProxies preimage;
			GenotypeProxies rest;
			GenotypeProxies image;
        };
        //using RNonSurjectiveFGenotypePtrSet = std::pair<GenotypePtrSet, GenotypePtrSet>;

        static GenotypeProxies identity(GenotypeProxies gps) { return gps; };
        static GenotypeProxies toPointerSet(std::initializer_list<Genotype> gl) {
            return GenotypeProxies({ std::make_shared<Genotype>() });
        };
    };
}
