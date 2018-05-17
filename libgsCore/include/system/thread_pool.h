#ifndef INCLUDE_SYSTEM_THREAD_POOL_H_
#define INCLUDE_SYSTEM_THREAD_POOL_H_

#include <functional>
#include <string>
#include <memory>


namespace libgs {

class ThreadPool: public std::enable_shared_from_this<ThreadPool> {
 public:
  using Function = std::function<void()>;
  using CreateCallback = std::function<void(
      const std::shared_ptr<ThreadPool> &/*thread_pool*/)>;

  static void Create(const std::string &name,
                     const size_t thread_size,
                     const CreateCallback &cb);

  static const std::shared_ptr<ThreadPool> &Get(const std::string &name);

  ThreadPool();

  void Initialize(const std::string &name,
                  const size_t thread_size,
                  const CreateCallback &cb);

  void Post(const Function &function,
            const std::string &serialization_tag="");

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

}  // namespace libgs

#endif  // INCLUDE_SYSTEM_THREAD_POOL_H_