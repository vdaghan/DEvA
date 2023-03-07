#pragma once

#include <expected>

namespace DEvA{
	enum class ErrorCode {
		OK,
		Fail,
		InvalidTransform,
		NotApplied,
		NoSuitableParentsToChoose,
		NotEnoughParentsToChoose,

		// JSON errors
		FileNotOpen,
		FileNotFound,
		ParseError,

		// Function composition errors
		SyntaxError,
		CompositionFailed,
		FunctionNotDefined
	};
	template <typename T> using Maybe = std::expected<T, ErrorCode>;
}
