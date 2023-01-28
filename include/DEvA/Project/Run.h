#pragma once

#include "DEvA/Project/Datastore.h"
#include "DEvA/Project/Filestore.h"

#include <chrono>
#include <iostream>
#include <list>
#include <memory>
#include <string>

namespace DEvA {
	template <typename Types>
	struct Run {
		void filestore(std::filesystem::path const & directory) {
			std::filesystem::path path(directory / getTimeAsString());
			datastore = std::make_shared<Filestore>(path);
		}
		std::string getTimeAsString() const {
			auto const now(std::chrono::system_clock::now());
			return std::format("{0:%Y}{0:%m}{0:%d}_{0:%H}{0:%M}", now);
		}
		std::shared_ptr<Datastore> datastore{};
	};
}
