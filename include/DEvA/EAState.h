#pragma once

#include <atomic>

struct EAState {
	std::atomic<std::size_t> currentGeneration;
	std::atomic<std::size_t> nextIdentifier;
};
