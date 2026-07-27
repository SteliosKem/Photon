#pragma once

#include "Common.h"

namespace Photon {
	enum class MessageSeverity {
		INFO,
		WARNING,
		ERROR
	};

	class Logger {
	public:
		static void log(const string& message, MessageSeverity severity);
		static void info(const string& message);
		static void warning(const string& message);
		static void error(const string& message);
	};
}