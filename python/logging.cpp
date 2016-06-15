#include "logging.h"

namespace pylp {

logger::LogChannel pylog("pylog", "[pylp] ");

logger::LogLevel getLogLevel() {
	return logger::LogManager::getGlobalLogLevel();
}

void setLogLevel(logger::LogLevel logLevel) {
	logger::LogManager::setGlobalLogLevel(logLevel);
}

} // namespace pylp
