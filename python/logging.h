#ifndef CANDIDATE_MC_PYTHON_LOGGING_H__
#define CANDIDATE_MC_PYTHON_LOGGING_H__

#include <util/Logger.h>

namespace pylp {

extern logger::LogChannel pylog;

/**
 * Get the log level of the python wrappers.
 */
logger::LogLevel getLogLevel();

/**
 * Set the log level of the python wrappers.
 */
void setLogLevel(logger::LogLevel logLevel);

} // namespace pylp

#endif // CANDIDATE_MC_PYTHON_LOGGING_H__

