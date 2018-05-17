#ifndef INCLUDE_LIBGS_COMMON_LOGGING_H_
#define INCLUDE_LIBGS_COMMON_LOGGING_H_

#include <memory>
#include <sstream>


namespace libgs {

class Logger {
 public:
  enum class Serverity {
    CRITICAL,
    ERROR,
    INFO,
    WARN
  };

  // TODO: append syslog
  enum class Type {
    kConsole,
    kDaily,
    kRotate
  };

  struct TimeProperty {
    size_t hours;
    size_t minutes;
  };

  // Console Logger
  static const std::shared_ptr<Logger> Create(const std::string &name);

  // TODO: change to time component;
  // Daily Logger
  static const std::shared_ptr<Logger> Create(
      const std::string &name, const std::string &filename,
      const TimeProperty &property);

  // Rotate Logger
  static const std::shared_ptr<Logger> Create(
      const std::string &name, const std::string &filename,
      const size_t max_size_in_mb, const size_t max_rotation_count);

  static const std::shared_ptr<Logger> Find(const std::string &name);

  explicit Logger(const std::string &name);

  Logger(const std::string &name, const std::string &filename,
         const TimeProperty &property);

  Logger(const std::string &name, const std::string &filename,
         const size_t max_size_in_mb, const size_t max_rotation_count);

  void Critical(const std::string &msg);
  void Critical(const std::stringstream &msg);

  void Error(const std::string &msg);
  void Error(const std::stringstream &msg);

  void Info(const std::string &msg);
  void Info(const std::stringstream &msg);

  void Warn(const std::string &msg);
  void Warn(const std::stringstream &msg);

  void Log(const Serverity &severity, const std::string &msg);
  void Log(const Serverity &severity, const std::stringstream &msg);

  const std::string &filename() const;
  const std::string &name() const;

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};


void InitializeDefaultLoggers();
void ReleaseLoggers();

}  // namespace libgs


#endif  // INCLUDE_LIBGS_COMMON_LOGGING_H_