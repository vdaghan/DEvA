#pragma once

#include <DEvA/Error.h>
#include <DEvA/Functions.h>
#include <DEvA/JSON/Common.h>
#include "DEvA/BuildingBlocks/Dependency/Dependency.h"
#include "DEvA/BuildingBlocks/EAFunctions/FunctionTemplates.h"
#include "DEvA/BuildingBlocks/EAFunctions/FunctionBlueprint.h"
#include "DEvA/BuildingBlocks/EAFunctions/Compositions/Compose.h"

#include <algorithm>
#include <concepts>
#include <functional>
#include <list>
#include <optional>
#include <stack>
#include <variant>

namespace DEvA {
	template <typename Blueprint>
	class FunctionComposer : public FunctionTemplates<Blueprint> {
		public:
			using FT = FunctionTemplates<Blueprint>;
			using Function = typename Blueprint::Function;
			using ReturnType = typename Blueprint::ReturnType;
			enum class OperatorType { Composition, Intersection, Union, Difference, SymmetricDifference };
			using StackVariant = std::variant<Function, std::string>;

			std::optional<Dependency> dependency{};

			bool validate(std::vector<std::string> composition) {
				for (auto & node : composition) {
					if ("compose" == node) {
						if constexpr (not CEndomorphism<Blueprint>) {
							return false;
						}
					} else if (not FT::parametrised.contains(node)) {
						return false;
					}
				}
				return true;
			}

			void compile(std::string functionName, std::vector<std::string> composition) {
				std::reverse(composition.begin(), composition.end());

				auto conditionLambda = [this, composition]() -> bool {
					return validate(composition);
				};

				auto actionLambda = [this, functionName, composition]() -> void {
					std::stack<StackVariant> stack{};

					for (auto & node : composition) {
						if ("compose" == node) {
							stack.push(node);
						} else {
							stack.push(FT::parametrised.at(node));
						}
					}

					auto popLambda = [&stack]() -> StackVariant {
						StackVariant retVal(stack.top());
						stack.pop();
						return retVal;
					};

					auto isStackTopAFunction = [&stack]() -> bool {
						return std::holds_alternative<Function>(stack.top());
					};
					auto isStackTopAnOperator = [&stack]() -> bool {
						return std::holds_alternative<std::string>(stack.top());
					};
					auto getStackTopAsFunction = [&stack, &popLambda]() -> Function {
						StackVariant sV(popLambda());
						return std::get<Function>(sV);
					};
					while (stack.size() > 1) {
						auto f1 = getStackTopAsFunction();
						auto f2 = getStackTopAsFunction();
						auto op = std::get<std::string>(popLambda());
						if ("compose" == op) {
							if constexpr (CEndomorphism<Blueprint>) {
								stack.push(Compose<Blueprint>()(f1, f2));
							}
						}
					}

					composed = getStackTopAsFunction();
					FT::parametrised.emplace(functionName, composed);
				};

				dependency = Dependency{
					.explanation = functionName + " composition",
					.condition = conditionLambda,
					.action = actionLambda
				};
			}

			Function get() {
				if (FT::wrapper) {
					return std::bind_front(FT::wrapper, composed);
				} else {
					return composed;
				}
			}
		private:
			static inline ReturnType differenceOf(ReturnType input1, ReturnType input2) {
				std::stable_sort(input1.begin(), input1.end());
				std::unique(input1.begin(), input1.end());
				std::stable_sort(input2.begin(), input2.end());
				std::unique(input2.begin(), input2.end());

				ReturnType retVal{};
				retVal.reserve(input1.size());
				std::set_difference(input1.begin(), input1.end(), input2.begin(), input2.end(), std::back_inserter(retVal));

				std::unique(retVal.begin(), retVal.end());
				return retVal;
			}
			static inline ReturnType intersectionOf(ReturnType input1, ReturnType input2) {
				std::stable_sort(input1.begin(), input1.end());
				std::unique(input1.begin(), input1.end());
				std::stable_sort(input2.begin(), input2.end());
				std::unique(input2.begin(), input2.end());

				ReturnType retVal{};
				retVal.reserve(std::min(input1.size(), input2.size()));
				std::set_intersection(input1.begin(), input1.end(), input2.begin(), input2.end(), std::back_inserter(retVal));

				std::unique(retVal.begin(), retVal.end());
				return retVal;
			}
			static inline ReturnType symmetricDifferenceOf(ReturnType input1, ReturnType input2) {
				std::stable_sort(input1.begin(), input1.end());
				std::unique(input1.begin(), input1.end());
				std::stable_sort(input2.begin(), input2.end());
				std::unique(input2.begin(), input2.end());

				ReturnType retVal{};
				retVal.reserve(input1.size() + input2.size());
				std::set_symmetric_difference(input1.begin(), input1.end(), input2.begin(), input2.end(), std::back_inserter(retVal));

				std::unique(retVal.begin(), retVal.end());
				return retVal;
			}
			static inline ReturnType unionOf(ReturnType input1, ReturnType input2) {
				std::stable_sort(input1.begin(), input1.end());
				std::unique(input1.begin(), input1.end());
				std::stable_sort(input2.begin(), input2.end());
				std::unique(input2.begin(), input2.end());

				ReturnType retVal{};
				retVal.reserve(input1.size() + input2.size());
				std::set_union(input1.begin(), input1.end(), input2.begin(), input2.end(), std::back_inserter(retVal));

				std::unique(retVal.begin(), retVal.end());
				return retVal;
			}

			Function composed{};
	};
}
