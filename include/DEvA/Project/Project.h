#pragma once

#include <filesystem>
#include <memory>

#include "DEvA/Project/ProjectSettings.h"
#include "DEvA/Project/Run.h"

namespace DEvA {
	template <typename Types>
	struct Project {
		Project() {
			settings.projectName = "unnamed_project";
		}
		Project(std::filesystem::path const & filename) {
		}
		std::shared_ptr<Run<Types>> newRun() {
		}
		std::shared_ptr<Run<Types>> run;
		ProjectSettings settings;
	};
}
