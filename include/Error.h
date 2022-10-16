#pragma once

#include <expected>

namespace DEvA{
	enum class ErrorCode {
		OK,
		Fail,
		InvalidTransform,
		NotApplied,
		NotEnoughParentsToChoose
	};
	template <typename T> using Maybe = std::expected<T, ErrorCode>;
}
