#pragma once

#include <expected>

namespace DEvA{
	enum class ErrorCode {
		OK,
		InvalidTransform,
		NotApplied,
		NotEnoughParentsToChoose
	};
	template <typename T> using Maybe = std::expected<T, ErrorCode>;
}
