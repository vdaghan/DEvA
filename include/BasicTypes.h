#pragma once

#include <deque>
#include <functional>
#include <memory>
#include <unordered_set>

#include <any>
#include <list>
#include <map>
#include <optional>
#include <string>
#include <utility>

namespace DEvA {
	template <typename G, typename P, typename F>
	struct BasicTypes {
        BasicTypes() {};

		using Genotype = G;
		using GenotypePtr = std::shared_ptr<Genotype>;
        using GenotypePtrSet = std::unordered_set<GenotypePtr>;
        using FGenotypePtrSet = std::function<GenotypePtrSet(GenotypePtrSet)>;
        using GenotypePtrSets = std::deque<GenotypePtrSet>; //GenotypeSetDeque

		using Phenotype = P;
		using PhenotypePtr = std::shared_ptr<Phenotype>;

		using Fitness = F;

		using FCreateGenotype = std::function<GenotypePtr(void)>;
        using FTransform = std::function<PhenotypePtr(GenotypePtr)>;
        using FEvaluate = std::function<F(GenotypePtr)>;
		using FSlicer = std::function<GenotypePtrSets(GenotypePtrSet)>;
		using FSlicerReturn = std::pair<GenotypePtrSet, GenotypePtrSet>;
	};
    /*
    namespace Operators {
        template <typename Genotype>
        using FGenotypeSetDeque = std::function<GenotypeSetDeque<Genotype>(GenotypeSetDeque<Genotype>)>;

        template <typename Genotype>
        using DequeFGenotypeSet = std::deque<FGenotypeSetDeque<Genotype>>;

        template <typename Genotype>
        GenotypeSetDeque<Genotype> operator>>(GenotypeSet<Genotype> gs, FGenotypeSetDeque<Genotype> fgsd) {
            GenotypeSetDeque<Genotype> gsd({ gs });
            return fgsd(gsd);
        }

        template <typename Genotype>
        GenotypeSetDeque<Genotype> operator>>(GenotypeSetDeque<Genotype> gsd, FGenotypeSetDeque<Genotype> fgsd) {
            return fgsd(gsd);
        }

        template <typename Genotype>
        GenotypeSet<Genotype> operator>>(GenotypeSet<Genotype> gs, FGenotypeSet<Genotype> fgs) {
            return fgs(gs);
        }

        template <typename Genotype>
        GenotypeSetDeque<Genotype> operator>>(GenotypeSetDeque<Genotype> gsd, DequeFGenotypeSet<Genotype> dfgs) {
            GenotypeSetDeque<Genotype> ret;
            size_t s_dfgs = dfgs.size();
            size_t s_gsd = gsd.size();
            size_t m = std::min(s_dfgs, s_gsd);
            for (size_t i = 0; i < m; ++i) {
                FGenotypeSet<Genotype> f = dfgs[i];
                GenotypeSet<Genotype>& gs = gsd[i];
                ret.push_back(gs >> f);
            }
            for (size_t i = m; i < s_gsd; ++i) {
                ret.push_back(gsd[i]);
            }
            return ret;
        };

        template <typename Genotype>
        FGenotypeSet<Genotype> identityfgs = [](GenotypeSet<Genotype> gs) { return gs; };
    }*/
}
/*
#include "EvolutionaryAlgorithm.h"
#include "Genealogy.h"
#include "Individual.h"
#include "StandardVariations.h"

namespace DEvA {
    template <typename G, typename P, typename F>
    struct Abstractions {
        using Genotype = BaseTypes::Genotype;
        using GenotypePtr = BaseTypes::GenotypePtr;
        using GenotypePtrSet = std::unordered_set<GenotypePtr>;
        using FGenotypePtrSet = std::function<GenotypePtrSet(GenotypePtrSet)>;
        using GenotypePtrSets = std::deque<GenotypePtrSet>; //GenotypeSetDeque
    };
}*/
