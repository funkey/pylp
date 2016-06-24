/*
* $URL$
* $Rev$
* $Author: s2946182 $
* $Date: 2008/01/15 15:41:13 $
* $Id: Logger.cc,v 1.3 2008/01/15 15:41:13 s2946182 Exp $
*/

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

#include "Logger.h"
#include "exceptions.h"
#ifdef HAVE_GIT_SHA1
#include <git_sha1.h>
#endif

namespace logger {

Logger glutton(0, "");

// Initialize global logging-streams:
LogLevel  error = Error;
LogLevel  user  = User;
LogLevel  debug = Debug;
LogLevel  all   = All;

LogChannel  out("default");

// Declare static objects
std::map<std::string, std::filebuf*>  LogFileManager::filebuffers;
LogLevel                              LogManager::globalLogLevel = User;
LogFileManager                        LogChannel::logFileManager;
std::set<LogChannel*>*                LogChannel::logChannels = 0;

LoggerCleanup loggerCleanup;

// Implementation - LogChannel

LogChannel::LogChannel(std::string channelName, std::string prefix) :
  _channelName(channelName),
  _prefix(prefix),
  _error(std::cerr.rdbuf(), _prefix),
  _user(std::cout.rdbuf(), _prefix),
  _debug(std::cout.rdbuf(), _prefix),
  _all(std::cout.rdbuf(), _prefix),
  _level(Global)
{
  getChannels()->insert(this);
}

std::set<LogChannel*>*
LogChannel::getChannels() {

  if (logChannels == 0)
    logChannels = new std::set<LogChannel*>();

  return logChannels;
}

Logger&
LogChannel::operator()(LogLevel level) {

  LogLevel  myLevel =
    (_level == Global ?
      LogManager::getGlobalLogLevel() : _level);

  switch (level) {
    case Error:
      if (myLevel >= Error)
        return _error;
      break;
    case User:
      if (myLevel >= User)
        return _user;
      break;
    case Debug:
      if (myLevel >= Debug)
        return _debug;
      break;
    case All:
      if (myLevel >= All)
        return _all;
      break;
    default:
      break;
  }
  return glutton;
}

void
LogChannel::setLogLevel(LogLevel level) {

  _level = level;
}

const LogLevel&
LogChannel::getLogLevel() {

  if (_level == Global)
    return LogManager::getGlobalLogLevel();

  return _level;
}

void
LogChannel::redirectToFile(std::string filename) {

  Logger fileLogger(LogFileManager::openFile(filename), _prefix);

  _error = fileLogger;
  _user  = fileLogger;
  _debug = fileLogger;
  _all   = fileLogger;
}

// Implementation - LogFileManager

LogFileManager::~LogFileManager() {

  std::map<std::string, std::filebuf*>::iterator i;

  for (i = filebuffers.begin(); i !=  filebuffers.end(); i++) {
    (*i).second->close();
    delete (*i).second;
  }
}

std::filebuf*
LogFileManager::openFile(std::string filename) {

  if (filebuffers.find(filename) != filebuffers.end())
    return filebuffers[filename];

  std::filebuf* filebuffer = new std::filebuf();
  if (!filebuffer->open(filename.c_str(), std::ios_base::out)) {

    LOG_ERROR(out) << "[LogFileManager] Unable to open \"" << filename << "\"" << std::endl;
    BOOST_THROW_EXCEPTION(IOError() << error_message(std::string("[LogFileManager] Attempt to open file \"") + filename + "\" failed."));
  }
  filebuffers[filename] = filebuffer;
  return filebuffer;
}


// Implementation - Logger

boost::mutex Logger::FlushMutex;
bool Logger::_showChannelPrefix = true;
bool Logger::_showThreadId      = false;

Logger::Logger(std::streambuf* streamBuffer, const std::string& prefix) :
  std::ostream(streamBuffer),
  _prefix(prefix)
{
  // Empty
}

Logger::Logger(Logger& logger, const std::string& prefix) :
  std::ostream(logger.rdbuf()),
  _prefix(prefix)
{
  // Empty
}

Logger&
Logger::operator=(const Logger& logger) {

  rdbuf(logger.rdbuf());
  return *this;
}

void
LogManager::init()
{
  // set global log level
  setGlobalLogLevel(User);
  out.setLogLevel(User);

  // read from program options
  Logger::showChannelPrefix(true);
  Logger::showThreadId(false);

#ifdef HAVE_GIT_SHA1
	LOG_USER(out) << "[LogManager] git sha1 of this build: " << __git_sha1 << std::endl;
#endif
}

void
LogManager::setGlobalLogLevel(LogLevel level) {

  globalLogLevel = level;
}

const LogLevel&
LogManager::getGlobalLogLevel() {

  return globalLogLevel;
}

LogLevel
LogManager::getLogLevel(std::string strLevel) {

    if (strLevel == "all")
      return All;
    else if (strLevel == "user")
      return User;
    else if (strLevel == "debug")
      return Debug;
    else if (strLevel == "error")
      return Error;
    else if (strLevel == "none")
      return Quiet;
    else {
      LOG_ERROR(out) << "[Logger] Unknown log level \"" << strLevel << "\"." << std::endl;
      BOOST_THROW_EXCEPTION(UsageError() << error_message("[Logger] Invalid log level"));
    }
}

std::set<LogChannel*>
LogManager::getChannels(std::string channelName) {

  std::set<LogChannel*> channels;

  for (channel_it i = LogChannel::getChannels()->begin();
       i != LogChannel::getChannels()->end(); i++) {
    if ( (*i)->getName() == channelName)
      channels.insert(*i);
  }

  if (channels.size() > 0)
    return channels;

  LOG_ERROR(out) << "[LogManager] No channel \"" << channelName << "\" available." << std::endl;
  printChannels();
  BOOST_THROW_EXCEPTION(UsageError() << error_message(std::string("[LogManager] Invalid channel name: ") + channelName));
}

void
LogManager::printChannels() {

  if (LogChannel::getChannels()->size() == 0) {
    LOG_USER(out) << "No output channels for this application available." << std::endl;
    return;
  }

  std::string prevChannelName = "";

  LOG_USER(out) << std::endl << "Valid output channels are:" << std::endl << "\t";
  for (channel_it i = LogChannel::getChannels()->begin();
       i != LogChannel::getChannels()->end(); i++) {
    if ((*i)->getName() != prevChannelName) {
      LOG_USER(out) << (*i)->getName() << " ";
      prevChannelName = (*i)->getName();
    }
  }
  LOG_USER(out) << std::endl << std::endl;
}

} // namespace logger

// End of file
