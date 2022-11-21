#pragma once

#include <format>
#include <functional>
#include <string>

namespace DEvA {
	enum class LogType {Trace, Debug, Info, Warning, Error, Critical};

	struct Logger {
		std::function<void(LogType, std::string)> callback;
		template <typename ... Args> void trace(std::string fstr, Args && ... args) {
			if (callback) callback(LogType::Trace, std::vformat(fstr, std::make_format_args(args...)));
		};
		template <typename ... Args> void debug(std::string fstr, Args && ... args) {
			if (callback) callback(LogType::Debug, std::vformat(fstr, std::make_format_args(args...)));
		};
		template <typename ... Args> void info(std::string fstr, Args && ... args) {
			if (callback) callback(LogType::Info, std::vformat(fstr, std::make_format_args(args...)));
		};
		template <typename ... Args> void warn(std::string fstr, Args && ... args) {
			if (callback) callback(LogType::Warning, std::vformat(fstr, std::make_format_args(args...)));
		};
		template <typename ... Args> void error(std::string fstr, Args && ... args) {
			if (callback) callback(LogType::Error, std::vformat(fstr, std::make_format_args(args...)));
		};
		template <typename ... Args> void critical(std::string fstr, Args && ... args) {
			if (callback) callback(LogType::Critical, std::vformat(fstr, std::make_format_args(args...)));
		};
	};
}
