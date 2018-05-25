#include "stdafx.h"

#include "common/logging.h"

#include "stdafx.h"

#include <boost/thread/mutex.hpp>
#include <spdlog/spdlog.h>
#include <memory>


namespace libgs {

namespace {

typedef std::shared_ptr<spdlog::logger> SpdLogger;
typedef std::map<const std::string /*name*/,
                 const std::shared_ptr<Logger> /*logger*/> LoggerMap;

const size_t kMegabytes = 1024 * 1024;

boost::mutex the_mutex;
LoggerMap the_map;

}  // unnamed namespace


class Logger::Impl {
 public:
  Impl() {}
  void Create(const std::string &name);
  void Create(const std::string &name, const std::string &filename,
              const TimeProperty &property);
  void Create(const std::string &name, const std::string &filename,
              const size_t max_size_in_mb,
              const size_t max_rotation_count);

  void Critical(const std::string &msg);
  void Critical(const std::stringstream &msg);

  void Error(const std::string &msg);
  void Error(const std::stringstream &msg);

  void Info(const std::string &msg);
  void Info(const std::stringstream &msg);

  void Warn(const std::string &msg);
  void Warn(const std::stringstream &msg);

  void Log(const Serverity &severity, const std::string &msg);
  void Log(const Serverity &severity,
           const std::stringstream &msg);

  const std::string &filename() const;

 private:
  std::string filename_;
  SpdLogger logger_;
  std::string name_;
};


///////////////////////////////////////////////////////////////////////////
// LOGGER IMPL

void Logger::Impl::Create(const std::string &name) {
  logger_ = spdlog::stdout_color_mt(name);
  BOOST_ASSERT(logger_);
  name_ = name;
}


void Logger::Impl::Create(const std::string &name,
                          const std::string &filename,
                          const TimeProperty &property) {
  logger_ = spdlog::daily_logger_mt(name, filename,
                                    property.hours,
                                    property.minutes);
  logger_->flush_on(spdlog::level::info);
  name_ = name;
}


void Logger::Impl::Create(const std::string &name,
                          const std::string &filename,
                          const size_t max_size_in_mb,
                          const size_t max_rotation_count) {
  // TODO: fix rotating logger flush.
  logger_ = spdlog::rotating_logger_mt(name, filename,
                                       max_size_in_mb * kMegabytes,
                                       max_rotation_count);
  logger_->flush_on(spdlog::level::info);
  name_ = name;
}


void Logger::Impl::Critical(const std::string &msg) {
  logger_->critical(msg);
}


void Logger::Impl::Critical(const std::stringstream &msg) {
  Critical(msg.str());
}


void Logger::Impl::Error(const std::string &msg) {
  logger_->error(msg);
}


void Logger::Impl::Error(const std::stringstream &msg) {
  Error(msg.str());
}


void Logger::Impl::Info(const std::string &msg) {
  logger_->info(msg);
}


void Logger::Impl::Info(const std::stringstream &msg) {
  Info(msg.str());
}


void Logger::Impl::Warn(const std::string &msg) {
  logger_->warn(msg);
}


void Logger::Impl::Warn(const std::stringstream &msg) {
  Warn(msg.str());
}


void Logger::Impl::Log(const Serverity &severity, const std::string &msg) {
  switch (severity) {
    case Serverity::kCritical:
    Critical(msg);
    return;

    case Serverity::kError:
    Error(msg);
    return;

    case Serverity::kInfo:
    Info(msg);
    return;

    case Serverity::kWarn:
    Warn(msg);
    return;
  }
}


void Logger::Impl::Log(const Serverity &severity,
                       const std::stringstream &msg) {
  Log(severity, msg.str());
}


const std::string &Logger::Impl::filename() const {
  return filename_;
}


///////////////////////////////////////////////////////////////////////////
// LOGGER

Logger::Logger(const std::string &name) {
  BOOST_ASSERT(!impl_);
  impl_ = std::make_unique<Logger::Impl>();
  impl_->Create(name);
}


Logger::Logger(const std::string &name, const std::string &filename,
               const TimeProperty &property) {
  BOOST_ASSERT(!impl_);
  impl_ = std::make_unique<Logger::Impl>();
  impl_->Create(name, filename, property);
}


Logger::Logger(const std::string &name, const std::string &filename,
               const size_t max_size_in_mb,
               const size_t max_rotation_count) {
  BOOST_ASSERT(!impl_);
  impl_ = std::make_unique<Logger::Impl>();
  impl_->Create(name);
}


const std::shared_ptr<Logger> Logger::Create(const std::string &name) {
  std::shared_ptr<Logger> logger = std::make_shared<Logger>(name);
  {
    boost::mutex::scoped_lock lock(the_mutex);
    the_map.insert(std::make_pair(name, logger));
  }
  return logger;
}


const std::shared_ptr<Logger> Logger::Create(
    const std::string &name, const std::string &filename,
    const TimeProperty &property) {
  std::shared_ptr<Logger> logger = std::make_shared<Logger>(
      name, filename, property);
  {
    boost::mutex::scoped_lock lock(the_mutex);
    the_map.insert(std::make_pair(name, logger));
  }
  return logger;
}


const std::shared_ptr<Logger> Logger::Create(
    const std::string &name, const std::string &filename,
    const size_t max_size_in_mb, const size_t max_rotation_count) {
  std::shared_ptr<Logger> logger = std::make_shared<Logger>(
      name, filename, max_size_in_mb, max_rotation_count);
  {
    boost::mutex::scoped_lock lock(the_mutex);
    the_map.insert(std::make_pair(name, logger));
  }
  return logger;
}


const std::shared_ptr<Logger> Logger::Find(const std::string &name) {
  {
    boost::mutex::scoped_lock lock(the_mutex);
    LoggerMap::const_iterator itr = the_map.find(name);
    if (itr != the_map.end()) {
      return itr->second;
    }
  }

  return nullptr;
}


void Logger::Critical(const std::string &msg) {
  impl_->Critical(msg);
}


void Logger::Critical(const std::stringstream &msg) {
  impl_->Critical(msg);
}


void Logger::Error(const std::string &msg) {
  impl_->Error(msg);
}


void Logger::Error(const std::stringstream &msg) {
  impl_->Error(msg);
}


void Logger::Info(const std::string &msg) {
  impl_->Info(msg);
}


void Logger::Info(const std::stringstream &msg) {
  impl_->Info(msg);
}


void Logger::Warn(const std::string &msg) {
  impl_->Warn(msg);
}


void Logger::Warn(const std::stringstream &msg) {
  impl_->Warn(msg);
}


void Logger::Log(const Serverity &severity, const std::string &msg) {
  impl_->Log(severity, msg);
}


void Logger::Log(const Serverity &severity, const std::stringstream &msg) {
  impl_->Log(severity, msg);
}


const std::string &Logger::filename() const {
  return impl_->filename();
}


void InitializeDefaultLoggers() {
  {
    auto logger = Logger::Create("console");
    BOOST_ASSERT(logger);
  }
  {
    Logger::TimeProperty property;
    property.hours = 0;
    property.minutes = 0;
    auto logger = Logger::Create("daily", "daily_logger.txt", property);
    BOOST_ASSERT(logger);
  }
  {
    auto logger = Logger::Create("rotate", "rotate_logger.txt", 5, 10);
    BOOST_ASSERT(logger);
  }
}


void ReleaseLoggers() {
  {
    boost::mutex::scoped_lock mutex(the_mutex);
    for (const auto &it: the_map) {
      the_map.erase(it.first);
    }
  }
}

}  // namespace libgs