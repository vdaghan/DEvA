#include "Specialisation.h"
#include "Variation.h"

#include <gtest/gtest.h>
#include <string>

TEST(Variation, Constructor) {
	using Spec = DEvA::Specialisation<int, int, int>;
	using Genotype = Spec::Genotype;
	using GenotypePtr = Spec::GenotypePtr;
	using GenotypePtrs = Spec::GenotypePtrs;
	auto variationOperatorBase = [](int in) -> int {
		return 2 * in;
	};
	Spec::SVariation::VariationOperator variationOperator = [&](GenotypePtrs in) -> GenotypePtrs {
		int gen = **(in.begin());
		int out = variationOperatorBase(gen);
		GenotypePtrs ret;
		ret.emplace_back(std::make_shared<Genotype>(out));
		return ret;
	};
	Spec::SVariation variation = Spec::SVariation(std::string("v"), variationOperator);

	GenotypePtr genptr = std::make_shared<Genotype>(1);
	GenotypePtrs genptrs;
	genptrs.emplace_back(genptr);
	auto retptrlist = variation(genptrs);
	int retVal = **retptrlist.begin();
	EXPECT_EQ(retVal, 2);

	//EXPECT_EQ(true, true);
}
