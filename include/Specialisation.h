#pragma once

#include <memory>
#include <initializer_list>

#include "BasicTypes.h"
#include "Concepts.h"

#include "EvolutionaryAlgorithm.h"
#include "Genealogy.h"
#include "Individual.h"
#include "Parameter.h"
#include "StandardVariations.h"
#include "StandardParentSelectors.h"
#include "Variation.h"

namespace DEvA {
    template <typename G, typename P, typename F> class BasicTypes;
    template <typename T> class EvolutionaryAlgorithm;
    template <typename T> class Genealogy;
    template <typename T> class Individual;
    template <typename T> class StandardVariations;

    template <typename G, typename P, typename F>
    struct Specialisation {
        // Repeat basic types used throughout code for better UX
        using BT = BasicTypes<G, P, F>;
        using Spec = Specialisation<G, P, F>;
        using Genotype = G;
        using GenotypePtr = std::shared_ptr<Genotype>;
        using GenotypePtrs = std::list<GenotypePtr>;
        template <size_t N> using GenotypePtrArray = std::array<GenotypePtr, N>;
        using GenotypePtrsDeque = std::deque<GenotypePtrs>;
        using Phenotype = P;
        using PhenotypePtr = std::shared_ptr<Phenotype>;
        using Fitness = F;

        using FGenotypePtrSet = std::function<GenotypePtrs(GenotypePtrs)>;
        using FGenotypePtrSetDeque = std::function<GenotypePtrsDeque(GenotypePtrsDeque)>;
        using DequeFGenotypePtrSet = std::deque<FGenotypePtrSet>;

        // Repeat non-POD types used throughout code for better UX
        using SEvolutionaryAlgorithm = EvolutionaryAlgorithm<Spec>;
        using SIndividual = Individual<Spec>;
        using SGenealogy = Genealogy<Spec>;
        using SVariation = Variation<Spec>;

        //using IndividualPtr = SIndividual::IndividualPtr;
        //using IndividualPtrs = SIndividual::IndividualPtrs;
        using IndividualPtr = std::shared_ptr<SIndividual>;
        using IndividualPtrs = std::list<IndividualPtr>;
        using IndividualWPtr = std::weak_ptr<SIndividual>;
        using IndividualWPtrs = std::list<IndividualWPtr>;

        using Generation = IndividualPtrs;
        using Generations = std::list<Generation>;

        using SStandardVariations = StandardVariations<Spec>;

        using FGenesis = std::function<Generation(void)>;
        using FCreateGenotype = std::function<GenotypePtr(void)>;
        using FTransform = std::function<PhenotypePtr(GenotypePtr)>;
        using FEvaluate = std::function<Fitness(GenotypePtr)>;
        using FParentSelection = std::function<IndividualPtrs(IndividualPtrs)>;
        using FVariation = std::function<GenotypePtrs(GenotypePtrs)>;
        using FSurvivorSelection = std::function<void(IndividualPtrs&)>;
        using FConvergenceCheck = std::function<bool(Fitness)>;
        struct RFGenotypePtrSet {
            GenotypePtrs domain;
            GenotypePtrs preimage;
            GenotypePtrs rest;
            GenotypePtrs image;
        };
        //using RNonSurjectiveFGenotypePtrSet = std::pair<GenotypePtrSet, GenotypePtrSet>;

        static GenotypePtrs identity(GenotypePtrs gps) { return gps; };
        static GenotypePtrs toPointerSet(std::initializer_list<Genotype> gl) {
            /*GenotypePtrSet gptrs;

            return std::make_shared<Genotype>(gl);*/
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
