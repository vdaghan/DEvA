#pragma once

#include <deque>
#include <memory>
#include <initializer_list>

#include "Concepts.h"

#include "EvolutionaryAlgorithm.h"
#include "Individual.h"
#include "Parameters.h"
#include "StandardConvergenceCheckers.h"
#include "StandardInitialisers.h"
#include "StandardParentSelectors.h"
#include "StandardSurvivorSelectors.h"
#include "StandardTransforms.h"
#include "StandardVariations.h"

namespace DEvA {
    template <typename T> class EvolutionaryAlgorithm;
    template <typename T, typename IP> class Individual;
    template <typename T> class StandardInitialisers;
    template <typename T> class StandardVariations;
    template <typename T> class StandardTransforms;

    template <typename T>
    struct Parametrised {
        T value;
        Parameters parameters;
    };

    template <typename G, typename P, typename F, typename IP>
    struct Specialisation {
        // Repeat basic types used throughout code for better UX
        using Spec = Specialisation<G, P, F, IP>;
        using Genotype = G;
        using GenotypePtr = std::shared_ptr<Genotype>;
        using GenotypePtrs = std::list<GenotypePtr>;
        using GenotypePtrsDeque = std::deque<GenotypePtrs>;
        using Phenotype = P;
        using PhenotypePtr = std::shared_ptr<Phenotype>;
        using Fitness = F;
        using IndividualParameters = IP;

        using FGenotypePtrSet = std::function<GenotypePtrs(GenotypePtrs)>;
        using FGenotypePtrSetDeque = std::function<GenotypePtrsDeque(GenotypePtrsDeque)>;
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

        using FGenesis = std::function<Generation(void)>;
        using FCreateGenotype = std::function<GenotypePtr(void)>;
        using FTransform = std::function<PhenotypePtr(GenotypePtr)>;
        using FEvaluate = std::function<Fitness(GenotypePtr)>;
        using FParentSelection = std::function<IndividualPtrs(IndividualPtrs)>;
        using FVariation = std::function<GenotypePtrs(GenotypePtrs)>;
        using FSurvivorSelection = std::function<void(IndividualPtrs&)>;
        using FConvergenceCheck = std::function<bool(Fitness)>;

        // Callbacks
        using COnEpoch = std::function<void(Generation &)>;

        struct RFGenotypePtrSet {
            GenotypePtrs domain;
            GenotypePtrs preimage;
            GenotypePtrs rest;
            GenotypePtrs image;
        };
        //using RNonSurjectiveFGenotypePtrSet = std::pair<GenotypePtrSet, GenotypePtrSet>;

        static GenotypePtrs identity(GenotypePtrs gps) { return gps; };
        static GenotypePtrs toPointerSet(std::initializer_list<Genotype> gl) {
            return GenotypePtrs({ std::make_shared<Genotype>() });
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
