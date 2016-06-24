/*
* $URL$
* $Rev$
* $Author: s2946182 $
* $Date: 2007/11/12 15:03:42 $
* $Id: Logger.hh,v 1.1 2007/11/12 15:03:42 s2946182 Exp $
*/

#ifndef __LOGGER_HH
#define __LOGGER_HH

#include<iostream>
#include<string>
#include<vector>
#include<set>
#include<map>

#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>

#include <config.h>

#define LOG_ERROR(channel) if (channel.getLogLevel() >= logger::Error) channel(logger::error)
#define LOG_USER(channel)  if (channel.getLogLevel() >= logger::User)  channel(logger::user)

#ifdef ENABLE_DEBUG_LOGGING
#define LOG_DEBUG(channel) if (channel.getLogLevel() >= logger::Debug) channel(logger::debug)
#define LOG_ALL(channel)   if (channel.getLogLevel() >= logger::All)   channel(logger::all)
#else
#define LOG_DEBUG(channel) if (false) channel(logger::debug)
#define LOG_ALL(channel)   if (false) channel(logger::all)
#endif


namespace logger {

enum LogLevel
{
  Global = 0,
  Quiet,    // no output at all      - no errors either
  Error,    // print errors only     - no user output
  User,     // output for the user   - most suitable setting
  Debug,    // output for developer  - give detailed information
            //                         about what's going on
  All       // maximum verbosity     - everything that's to much
            //                         for debug-output
};

/**
 * Stream manipulators.
 */
enum Manip {

	delline // delete the content of the current line
};

// forward declaration
class LogChannel;

class Logger : public std::ostream {

public:

	Logger(std::streambuf* streamBuffer, const std::string& prefix);

	Logger(Logger& logger, const std::string& prefix);

	Logger& operator=(const Logger& logger);

	static void showChannelPrefix(bool show) {

		_showChannelPrefix = show;
	}

	static void showThreadId(bool show) {

		_showThreadId = show;
	}

	template <typename T>
	Logger& operator<<(T* t) {

		getBuffer() << t;

		return *this;
	}

	template <typename T>
	Logger& operator<<(const T& t) {

		getBuffer() << t;

		return *this;
	}

	Logger& operator<<(Manip op) {

		if (op == delline) {

			{
				boost::mutex::scoped_lock lock(FlushMutex);

				std::ostream& s = *this;

				// send cursor back
				s << "\33[2K\r";

				clearBuffer();
			}
		}

		return *this;
	}

	Logger& operator<<(std::ostream&(*fp)(std::ostream&)) {

		getBuffer() << fp;

		// the next operator<< will cause the prefix to be printed after a 
		// newline
		if (fp == &std::endl<std::ostream::char_type, std::ostream::traits_type>) {

			{
				boost::mutex::scoped_lock lock(FlushMutex);

				std::ostream& s = *this;

				s << getBuffer().str();
				s << std::flush;
			}

			clearBuffer();
		}

		// flush the buffer content
		if (fp == &std::flush<std::ostream::char_type, std::ostream::traits_type>) {

			{
				boost::mutex::scoped_lock lock(FlushMutex);

				std::ostream& s = *this;

				s << getBuffer().str();
				s << std::flush;
			}

			// clear the current buffer
			getBuffer().str("");
		}

		return *this;
	}

	Logger& operator<<(std::ios&(*fp)(std::ios&)) {

		getBuffer() << fp;

		return *this;
	}

	Logger& operator<<(std::ios_base&(*fp)(std::ios_base&)) {

		getBuffer() << fp;

		return *this;
	}

private:

	std::stringstream& getBuffer() {

		if (_buffer.get() == 0) {

			_buffer.reset(new std::stringstream());
			clearBuffer();
		}

		return (*_buffer);
	}

	void clearBuffer() {

		getBuffer().str("");

		if (_showChannelPrefix) {

			// fill the buffer with the prefix
			getBuffer() << getPrefix();
		}
	}

	std::string getPrefix() {

		if (_showThreadId)
			return boost::lexical_cast<std::string>(boost::this_thread::get_id()) + " " + _prefix;
		return _prefix;
	}

	// reference to the owning LogChannel's prefix
	const std::string& _prefix;

	// show the prefix for all channels
	static bool _showChannelPrefix;
	static bool _showThreadId;

	// thread local buffer
	boost::thread_specific_ptr<std::stringstream> _buffer;

	static boost::mutex FlushMutex;
};

class LogFileManager {

  public:

    ~LogFileManager();

    static std::filebuf* openFile(std::string filename);

  private:

    static std::map<std::string, std::filebuf*> filebuffers;

    static LogLevel getLogLevel(std::string loglevel);

    static LogChannel* getChannel(std::string name);

    static void printChains();

    static LogLevel  globalLogLevel;
};

class LogChannel {

  public:

    static std::set<LogChannel*>*  getChannels();

    LogChannel(std::string channelName, std::string prefix = "");

    Logger& operator()(LogLevel level);

    std::string getName() { return _channelName; };

    void  setLogLevel(LogLevel level);

    const LogLevel& getLogLevel();

    void  redirectToFile(std::string filename);

  private:

    friend struct LoggerCleanup;

    static  std::set<LogChannel*>*  logChannels;

    static  LogFileManager  logFileManager;

    std::string _channelName;

    std::string _prefix;

    Logger  _error;

    Logger  _user;

    Logger  _debug;

    Logger  _all;

    LogLevel  _level;
};

class LogManager {

  public:

    static void init();

    static void setGlobalLogLevel(LogLevel logLevel);

    static const LogLevel& getGlobalLogLevel();

  private:

    static void setChainLogLevel(LogChannel* chain, LogLevel level);

    static void redirectChainToFile(LogChannel* chain, std::string filename);

    static LogLevel getLogLevel(std::string level);

    static std::set<LogChannel*> getChannels(std::string channelName);

    static void printChannels();


    static LogLevel globalLogLevel;
};

struct LoggerCleanup {

	~LoggerCleanup() {

		if (LogChannel::logChannels != 0)
			delete LogChannel::logChannels;
	}
};

extern LoggerCleanup loggerCleanup;

typedef std::set<LogChannel*>::iterator channel_it;

extern LogChannel out;

extern LogLevel error;
extern LogLevel user;
extern LogLevel debug;
extern LogLevel all;

} // namespace logger

#endif  // #ifndef __LOGGER_HH
