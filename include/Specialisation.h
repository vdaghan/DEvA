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
        using Genotype = BT::Genotype;
        using GenotypePtr = BT::GenotypePtr;
        using GenotypePtrSet = BT::GenotypePtrSet;
        using GenotypePtrSets = BT::GenotypePtrSets;
        using Phenotype = BT::Phenotype;
        using PhenotypePtr = BT::PhenotypePtr;
        using Fitness = BT::Fitness;

        using FGenotypePtrSet = std::function<GenotypePtrSet(GenotypePtrSet)>;
        using GenotypePtrSetDeque = std::deque<GenotypePtrSet>;
        using FGenotypePtrSetDeque = std::function<GenotypePtrSetDeque(GenotypePtrSetDeque)>;
        using DequeFGenotypePtrSet = std::deque<FGenotypePtrSet>;

        // Repeat non-POD types used throughout code for better UX
        using SEvolutionaryAlgorithm = EvolutionaryAlgorithm<BT>;
        using SIndividual = Individual<BT>;
        using SGenealogy = Genealogy<BT>;
        using SVariation = Variation<BT>;

        using IndividualPtr = SIndividual::IndividualPtr;
        using IndividualPtrSet = SIndividual::IndividualPtrSet;

        using Generation = IndividualPtrSet;
        using Generations = std::list<Generation>;

        //using FGenesis = std::function<Generation(void)>;
        using FGenesis = SGenealogy::GenesisFunction;






        using SEvolutionaryAlgorithm = EvolutionaryAlgorithm<BT>;
        using SGenealogy = Genealogy<BT>;
        using SIndividual = Individual<BT>;
        using SStandardVariations = StandardVariations<BT>;

        using SIndividualPtr = Individual<BT>::IndividualPtr;
        using SIndividualPtrSet = Individual<BT>::IndividualPtrSet;
        using Generation = SIndividual::IndividualPtrSet;
        using GenesisFunction = SGenealogy::GenesisFunction;

        using FCreateGenotype = BT::FCreateGenotype;
        //using PlaceGenotypeFunction = std::function<void(SGenealogy&, GenotypePtr)>;


        static GenotypePtrSet identity(GenotypePtrSet gps) { return gps; };
        static GenotypePtrSet toPointerSet(std::initializer_list<Genotype> gl) {
            /*GenotypePtrSet gptrs;

            return std::make_shared<Genotype>(gl);*/
            return GenotypePtrSet({ std::make_shared<Genotype>() });
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
