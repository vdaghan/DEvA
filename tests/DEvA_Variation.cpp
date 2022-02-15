#include "Specialisation.h"
#include "Variation.h"

#include <gtest/gtest.h>
#include <string>

TEST(Variation, Constructor) {
	using Spec = DEvA::Specialisation<int, int, int>;
	using Genotype = Spec::Genotype;
	using GenotypePtr = Spec::GenotypePtr;
	using GenotypePtrSet = Spec::GenotypePtrSet;
	auto variationOperatorBase = [](int in) -> int {
		return 2 * in;
	};
	Spec::SVariation::VariationOperator variationOperator = [&](GenotypePtrSet in) -> GenotypePtrSet {
		int gen = **(in.begin());
		int out = variationOperatorBase(gen);
		GenotypePtrSet ret;
		ret.emplace(std::make_shared<Genotype>(out));
		return ret;
	};
	Spec::SVariation variation = Spec::SVariation(std::string("v"), variationOperator);

	GenotypePtr genptr = std::make_shared<Genotype>(1);
	GenotypePtrSet genptrset;
	genptrset.emplace(genptr);
	auto retptrlist = variation(genptrset);
	int retVal = **retptrlist.begin();
	EXPECT_EQ(retVal, 2);

	//EXPECT_EQ(true, true);
}
