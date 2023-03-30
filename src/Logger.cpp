#include "DEvA/Logger.h"

#include "spdlog/async.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

void initialiseLogger() {
	auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs.txt", true);
	std::vector<spdlog::sink_ptr> sinks{ console_sink, file_sink };

	std::string pattern("[%Y%m%d %H:%M:%S.%f][%^%l%$] %v");
	//spdlog::default_logger()->sinks().push_back(console_sink);
	spdlog::default_logger()->sinks().push_back(file_sink);
	spdlog::default_logger()->set_pattern(pattern);
	spdlog::default_logger()->set_level(spdlog::level::trace);
	spdlog::default_logger()->flush_on(spdlog::level::trace);
};
