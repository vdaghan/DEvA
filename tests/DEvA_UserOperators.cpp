#include "Specialisation.h"

#include <gtest/gtest.h>

TEST(UserOperators, hgfh) {
    /*using Genotype = int;
    using Phenotype = int;
    using Fitness = int;
    using Spec = DEvA::Specialisation<Genotype, Phenotype, Fitness>;

    using GS = Spec::GenotypePtrSet;
    using FGS = Spec::FGenotypePtrSet;
    using GSD = Spec::GenotypePtrSetDeque;
    using FGSD = Spec::FGenotypePtrSetDeque;
    using DFGS = Spec::DequeFGenotypePtrSet;


    GS gs = Spec::toPointerSet({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 });

    FGSD fgsd = [](GSD gsd) {
        GS gs1 = Spec::toPointerSet({  1, 3, 5, 7, 9 });
        GS gs2 = Spec::toPointerSet({  0, 2, 4, 6, 8 });
        GS gs3 = Spec::toPointerSet({ 10,11,12,13,14 });
        return GSD({ gs1, gs2, gs3 });
    };

    FGS fgs = [](GS gs) {
        GS ret;
        for (auto it = gs.begin(); it != gs.end(); ++it) {
            ret.emplace(**it * 2);
        }
        return ret;
    };*/

    /* {
        using namespace DEvA;
        GSD gl2 = gs >> fgsd >> DFGS{ Spec::identity, fgs };
    }

    for (size_t i = 0; i < gl2.size(); ++i) {
        for (auto it = gl2[i].begin(); it != gl2[i].end(); ++it) {
            std::cout << *it << " ";
        }
        std::cout << std::endl;
    }*/
    EXPECT_TRUE(true);
}
