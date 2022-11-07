#pragma once

#include <deque>
#include <initializer_list>
#include <map>
#include <memory>
#include <type_traits>

#include "Concepts.h"

#include "EAStatistics.h"
#include "Error.h"
#include "EvolutionaryAlgorithm.h"
#include "Individual.h"
#include "IndividualIdentifier.h"
#include "Parameters.h"
#include "StandardConvergenceCheckers.h"
#include "StandardInitialisers.h"
#include "StandardParentSelectors.h"
#include "StandardSurvivorSelectors.h"
#include "StandardTransforms.h"
#include "StandardVariations.h"
#include "VariationFunctor.h"

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
        using MetricVariant = S::MetricVariant;
        using MetricVariantMap = std::map<std::string, MetricVariant>;
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

		// Specification function types
		using FGenotypeFromProxy = std::function<Genotype & (GenotypeProxy)>;
		using FPhenotypeFromProxy = std::function<Phenotype & (PhenotypeProxy)>;

		// EA function types
        using FGenesis = std::function<GenotypeProxies(void)>;
		using FGenePoolSelection = std::function<Generation(Generation)>;
        using FCreateGenotype = std::function<GenotypeProxy(void)>;
        using FTransform = std::function<MaybePhenotypeProxy(GenotypeProxy)>;
        using FEvaluateIndividualFromGenotypeProxy = std::function<MetricVariantMap(GenotypeProxy)>;
        using FEvaluateIndividualFromIndividualPtr = std::function<MetricVariantMap(IndividualPtr)>;
        using FEvaluateGeneration = std::function<MetricVariantMap(Generation)>;
        using FMetricComparison = std::function<bool(MetricVariant, MetricVariant)>;
        using MetricComparisonMap = std::map<std::string, FMetricComparison>;
        using FParentSelection = std::function<IndividualPtrs(MetricComparisonMap const &, IndividualPtrs)>;
        using FVariationFromGenotypeProxies = std::function<GenotypeProxies(GenotypeProxies)>;
        using FVariationFromIndividualPtrs = std::function<GenotypeProxies(IndividualPtrs)>;
        using FSurvivorSelection = std::function<void(IndividualPtrs&)>;
        using FSortIndividuals = std::function<bool(IndividualPtr, IndividualPtr)>;
        using FConvergenceCheck = std::function<bool(MetricVariantMap const &)>;
        using FVariant = std::variant<
            FGenesis,
            FGenePoolSelection,
            FCreateGenotype,
            FTransform,
            FEvaluateIndividualFromGenotypeProxy,
            FEvaluateIndividualFromIndividualPtr,
            FEvaluateGeneration,
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
        using CVariant = std::variant<
            CVoid,
            CEAStatsUpdate,
            COnEpoch,
            COnVariation>;
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
    /*
    template <typename G, typename P, typename F>
    typename Specialisation<G, P, F>::GenotypePtrSets operator>>(typename Specialisation<G, P, F>::GenotypePtrSet gs, typename Specialisation<G, P, F>::FGenotypePtrSetDeque fgsd) {
        Specialisation<G, P, F>::GenotypePtrSets gpsd({ gs });
        return fgsd(gpsd);
    }

    template <typename G, typename P, typename F>
    typename Specialisation<G, P, F>::GenotypePtrSets operator>>(typename Specialisation<G, P, F>::GenotypePtrSets gsd, typename Specialisation<G, P, F>::FGenotypePtrSetDeque fgsd) {
        return fgsd(gsd);
    }

    template <typename G, typename P, typename F>
    typename Specialisation<G, P, F>::GenotypePtrSet operator>>(typename Specialisation<G, P, F>::GenotypePtrSet gs, typename Specialisation<G, P, F>::FGenotypePtrSet fgs) {
        return fgs(gs);
    }

    template <typename G, typename P, typename F>
    typename Specialisation<G, P, F>::GenotypePtrSets operator>>(typename Specialisation<G, P, F>::GenotypePtrSets gsd, typename Specialisation<G, P, F>::DequeFGenotypePtrSet dfgs) {
        typename Specialisation<G, P, F>::GenotypePtrSets ret;
        size_t s_dfgs = dfgs.size();
        size_t s_gsd = gsd.size();
        size_t m = std::min(s_dfgs, s_gsd);
        for (size_t i = 0; i < m; ++i) {
            typename Specialisation<G, P, F>::FGenotypePtrSet f = dfgs[i];
            typename Specialisation<G, P, F>::GenotypePtrSet& gs = gsd[i];
            ret.push_back(gs >> f);
        }
        for (size_t i = m; i < s_gsd; ++i) {
            ret.push_back(gsd[i]);
        }
        return ret;
    };*/
}
