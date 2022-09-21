#pragma once

#include <deque>
#include <memory>
#include <initializer_list>

#include "Concepts.h"

#include "Error.h"
#include "EvolutionaryAlgorithm.h"
#include "Individual.h"
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
        using Fitness = S::Fitness;
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
        using Generations = std::list<Generation>;
        using Genealogy = Generations;

        using SStandardVariations = StandardVariations<Spec>;

		using SVariationInfo = VariationInfo<Spec>;
		using SVariationFunctor = VariationFunctor<Spec>;

		// Specification function types
		using FGenotypeFromProxy = std::function<Genotype & (GenotypeProxy)>;
		using FPhenotypeFromProxy = std::function<Phenotype & (PhenotypeProxy)>;

		// EA function types
        using FGenesis = std::function<Generation(void)>;
		using FGenePoolSelection = std::function<Generation(Generation)>;
        using FCreateGenotype = std::function<GenotypeProxy(void)>;
        using FTransform = std::function<MaybePhenotypeProxy(GenotypeProxy)>;
		using FEvaluate = std::function<Fitness(GenotypeProxy)>;
		using FFitnessComparison = std::function<bool(Fitness const &, Fitness const &)>;
        using FParentSelection = std::function<IndividualPtrs(FFitnessComparison, IndividualPtrs)>;
        //using FVariation = std::function<GenotypeProxies(GenotypeProxies)>;
		using FVariation = std::function<GenotypeProxies(GenotypeProxies)>;
        using FSurvivorSelection = std::function<void(IndividualPtrs&)>;
        using FConvergenceCheck = std::function<bool(Fitness)>;

        // Callbacks
		using CVoid = std::function<void(void)>;
        using COnEpoch = std::function<void(std::size_t)>;

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
