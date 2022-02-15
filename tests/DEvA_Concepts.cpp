#include <algorithm>
#include <deque>
#include <functional>
#include <string>
#include <unordered_Set>
#include <utility>

#include "Concepts.h"

#include <gtest/gtest.h>

TEST(Concepts, CCallable) {
	bool comp1 = DEvA::CCallable<std::function<int(int)>, int>;
	EXPECT_TRUE(comp1);
	bool comp2 = DEvA::CCallable<std::function<int(void)>>;
	EXPECT_TRUE(comp2);
	bool comp3 = DEvA::CCallable<std::function<void(int)>, int>;
	EXPECT_TRUE(comp3);
	bool comp4 = DEvA::CCallable<int>;
	EXPECT_FALSE(comp4);
	bool comp5 = DEvA::CCallable<std::deque<int>>;
	EXPECT_FALSE(comp5);
}

TEST(Concepts, CComposable) {
	using FIntInt = std::function<int(int)>;
	using FIntString = std::function<std::string(int)>;

	bool comp1 = DEvA::CComposable<FIntInt, FIntInt>;
	EXPECT_TRUE(comp1);
	bool comp2 = DEvA::CComposable<FIntString, FIntInt>;
	EXPECT_TRUE(comp2);
	bool comp3 = DEvA::CComposable<FIntInt, FIntString>;
	EXPECT_FALSE(comp3);
}

TEST(Concepts, CReturns) {
	using FIntInt = std::function<int(int)>;
	using FIntString = std::function<std::string(int)>;
	using FIntIntDeque = std::function<std::deque<int>(int)>;

	bool comp1 = DEvA::CReturns<FIntInt, int>;
	EXPECT_TRUE(comp1);
	bool comp2 = DEvA::CReturns<FIntString, std::string>;
	EXPECT_TRUE(comp2);
	bool comp3 = DEvA::CReturns<FIntIntDeque, int>;
	EXPECT_FALSE(comp3);
}

TEST(Concepts, CGenotype) {
	using Genotype = int;
	using GS = std::unordered_set<Genotype>;
	using DGS = std::deque<std::unordered_set<Genotype>>;

	bool comp1 = DEvA::CGenotype<Genotype, int>;
	EXPECT_TRUE(comp1);
	bool comp2 = DEvA::CGenotype<Genotype, GS>;
	EXPECT_FALSE(comp2);
	bool comp3 = DEvA::CGenotype<Genotype, decltype(*(GS().begin()))>;
	EXPECT_TRUE(comp3);
	bool comp4 = DEvA::CGenotype<Genotype, DGS>;
	EXPECT_FALSE(comp4);
	bool comp5 = DEvA::CGenotype<Genotype, decltype(*DGS().begin())>;
	EXPECT_FALSE(comp5);
}

TEST(Concepts, CContainer) {
	using Genotype = int;
	using GS = std::unordered_set<Genotype>;
	using DGS = std::deque<std::unordered_set<Genotype>>;

	bool comp1 = DEvA::CContainer<Genotype>; // false
	EXPECT_FALSE(comp1);
	bool comp2 = DEvA::CContainer<GS>; // true
	EXPECT_TRUE(comp2);
	bool comp3 = DEvA::CContainer<DGS>; // true
	EXPECT_TRUE(comp3);
}

TEST(Concepts, TypeOfElements) {
	using FIntInt = std::function<int(int)>;
	using DFIntInt = std::deque<FIntInt>;
	using DInt = std::deque<int>;

	bool comp1 = std::same_as<DEvA::TypeOfElements<DFIntInt>, FIntInt>;
	EXPECT_TRUE(comp1);
	bool comp2 = std::same_as<DEvA::TypeOfElements<DInt>, int>;
	EXPECT_TRUE(comp2);
}

TEST(Concepts, CContainerOfCallables) {
	using FIntInt = std::function<int(int)>;
	using DFIntInt = std::deque<FIntInt>;
	using DInt = std::deque<int>;

	bool comp1 = DEvA::CContainerOfCallables<FIntInt>; // false
	EXPECT_FALSE(comp1);
	bool comp2 = DEvA::CContainerOfCallables<DFIntInt, int>; // false
	EXPECT_TRUE(comp2);
	bool comp3 = DEvA::CContainerOfCallables<DInt>; // false
	EXPECT_FALSE(comp3);
}

TEST(Concepts, CGenotypeContainer) {
	using Genotype = int;
	using GS = std::unordered_set<Genotype>;
	using DGS = std::deque<std::unordered_set<Genotype>>;

	bool comp1 = DEvA::CGenotypeContainer<Genotype, Genotype>;
	EXPECT_FALSE(comp1);
	bool comp2 = DEvA::CGenotypeContainer<Genotype, GS>;
	EXPECT_TRUE(comp2);
	bool comp3 = DEvA::CGenotypeContainer<Genotype, DGS>;
	EXPECT_FALSE(comp3);
}

TEST(Concepts, toFunction) {
	using G = int;
	using SG = std::unordered_set<int>;
	using DSG = std::deque<std::unordered_set<int>>;
	using F = std::function<SG(SG)>;
	using DF = std::deque<F>;

	DSG dsg({ { 0, 1, 2, 3, 4 }, {0, 1, 2, 3, 4}, { 5, 6, 7, 8, 9 }});
	F f1 = [](SG sg) {
		SG ret;
		std::for_each(sg.begin(), sg.end(), [&](G g) { ret.emplace(g * 2); });
		return ret;
	};
	F f2 = [](SG sg) {
		SG ret;
		std::for_each(sg.begin(), sg.end(), [&](G g) { ret.emplace(g + 2); });
		return ret;
	};
	F f3 = [](SG sg) {
		SG ret;
		std::for_each(sg.begin(), sg.end(), [&](G g) { ret.emplace(g); });
		return ret;
	};
	DF df({f1, f2, f3});
	auto func = DEvA::toFunction(df);
	DSG ret = func(dsg);
	EXPECT_EQ(ret, DSG({ { 0, 2, 4, 6, 8}, {2, 3, 4, 5, 6}, {5, 6, 7, 8, 9} }));
}
