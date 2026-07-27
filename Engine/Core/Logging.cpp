#include "Logging.h"
#include <spdlog/spdlog.h>

namespace Photon {
	void Logger::log(const string& message, MessageSeverity severity) {
		switch (severity) {
		case MessageSeverity::INFO: info(message); return;
		case MessageSeverity::WARNING: warning(message); return;
		case MessageSeverity::ERROR: error(message); return;
		}
	}

	void Logger::info(const string& message) {
		spdlog::info(message);
	}

	void Logger::warning(const string& message) {
		spdlog::warn(message);
	}

	void Logger::error(const string& message) {
		spdlog::error(message);
	}

}