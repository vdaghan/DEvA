#pragma once

#include <expected>

namespace DEvA{
	enum class ErrorCode {
		OK,
		InvalidTransform
	};
	template <typename T> using Maybe = std::expected<T, ErrorCode>;
}
