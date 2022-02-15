#include "Parameters.h"

#include <gtest/gtest.h>

TEST(Parameters, Constructor) {
	DEvA::Parameters parameters;

	DEvA::Parameter parameter("aaa", std::make_any<std::string>("bbb"));
	parameters.setParameter(parameter);

	DEvA::MaybeParameter maybeParameter = parameters.getParameter("aaa");
	if (not maybeParameter) {
		FAIL() << "std::optional returns std::nullopt. It shouldn't...";
	}
	DEvA::Parameter retParam = maybeParameter.value();
	std::any retVal = retParam.value;
	EXPECT_EQ("bbb", std::any_cast<std::string>(retVal));

	EXPECT_EQ(true, true);
}
