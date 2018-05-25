#include "stdafx.h"

#include "system/thread_pool.h"

#include <boost/asio/io_service.hpp>
#include <boost/asio/strand.hpp>

#include <thread>
#include <map>
#include <mutex>
#include <vector>


namespace libgs {

namespace {

using ThreadPoolMap = std::map<
    const std::string /*name*/, const std::shared_ptr<ThreadPool> >;


// IoServiceWorker는 ThreadPool::Impl 의 mutex 로 보호 받는다.
class IoServiceWorker
    : public std::enable_shared_from_this<IoServiceWorker> {

 public:
  IoServiceWorker(const std::shared_ptr<boost::asio::io_service> &io_service);

  void Start();
  void Run();
  void Join();

 private:
  std::shared_ptr<std::thread> thread_;
  std::shared_ptr<boost::asio::io_service> io_service_;
};


IoServiceWorker::IoServiceWorker(
    const std::shared_ptr<boost::asio::io_service> &io_service)
    : io_service_(io_service) {

}


void IoServiceWorker::Start() {
  thread_ = std::make_shared<std::thread>(
      &IoServiceWorker::Run, shared_from_this());

}


void IoServiceWorker::Run() {
  BOOST_ASSERT(io_service_);
  std::shared_ptr<boost::asio::io_service::work> work =
      std::make_shared<boost::asio::io_service::work>(*io_service_);
  io_service_->run();
}


// 여기서는 ThreadJoin을 시키는 것이다. io service를 멈추지는 않는다.
void IoServiceWorker::Join() {
  // TODO(inkeun): join을 구현한다.
  // BOOST_ASSERT(thread_);
  // thread_->join();
  // thread_.reset();
}


std::mutex the_thread_pool_mutex;
ThreadPoolMap the_thread_pools;

}  // unnamed namespace


class ThreadPool::Impl {
 public:
  void Initialize(const std::string &name,
                  const size_t thread_size,
                  const std::function<void(const bool/*error*/)> &cb);

  void Post(const Function &function);

  void Post(const Function &function,
            const std::string &serialization_tag);

  void Start();
  void Stop();

  const std::shared_ptr<boost::asio::io_service> io_service();

 private:
  using StrandMap = std::map<
      const std::string /*name*/,
      const std::shared_ptr<boost::asio::strand> /*strand*/>;

  const std::shared_ptr<boost::asio::strand> &CreateStrand(
      const std::string &name);

  const std::shared_ptr<boost::asio::strand> &GetOrCreateStrand(
      const std::string &name);

  mutable std::mutex mutex_;
  std::string name_;
  bool initialized_;
  bool running_;

  std::shared_ptr<boost::asio::io_service> io_service_;
  std::shared_ptr<IoServiceWorker> io_worker_;
  StrandMap strands_;
};


////////////////////////////////////////////////////////////////////////////////
// ThreadPool::Impl

void ThreadPool::Impl::Initialize(
    const std::string &name, const size_t thread_size,
    const std::function<void(const bool/*error*/)> &cb) {
  BOOST_ASSERT(!name.empty() && thread_size > 0);
  BOOST_ASSERT(!io_service_);

  std::lock_guard<std::mutex> lock(mutex_);
  name_ = name;
  io_service_ = std::make_shared<boost::asio::io_service>();
  initialized_ = true;
  Start();
  cb(false);
}


void ThreadPool::Impl::Post(const Function &function) {
  io_service_->post(function);
}


void ThreadPool::Impl::Post(const Function &function,
                            const std::string &serialization_tag) {
  const std::shared_ptr<boost::asio::strand> &strand =
      GetOrCreateStrand(serialization_tag);

  BOOST_ASSERT(strand);
  strand->post(function);
}


// 락을 잡은 체로 호출한다.
void ThreadPool::Impl::Start() {
  io_worker_ = std::make_shared<IoServiceWorker>(io_service_);
  io_worker_->Start();
}


void ThreadPool::Impl::Stop() {

}


const std::shared_ptr<boost::asio::io_service>
    ThreadPool::Impl::io_service() {
  std::lock_guard<std::mutex> lock(mutex_);
  return io_service_;
}


// 락을 잡고 호출한다.
const std::shared_ptr<boost::asio::strand> &
    ThreadPool::Impl::CreateStrand(
        const std::string &name) {
  using StrandMapInsertResult = std::pair<StrandMap::iterator, bool>;
  BOOST_ASSERT(!name.empty());
  StrandMapInsertResult result = strands_.insert(
      std::make_pair(
          name, std::make_shared<boost::asio::strand>(*io_service_)));

  BOOST_ASSERT(result.second);
  return result.first->second;
}


const std::shared_ptr<boost::asio::strand> &
    ThreadPool::Impl::GetOrCreateStrand(
        const std::string &name) {
  std::lock_guard<std::mutex> lock(mutex_);

  StrandMap::const_iterator itr = strands_.find(name);

  if (itr != strands_.end()) {
    return itr->second;
  }

  return CreateStrand(name);
}


////////////////////////////////////////////////////////////////////////////////
// ThreadPool

void ThreadPool::Create(const std::string &name,
                        const size_t thread_size,
                        const CreateCallback &cb) {
  using InsertResult = std::pair<ThreadPoolMap::iterator, bool>;

  std::shared_ptr<ThreadPool> pool = std::make_shared<ThreadPool>();
  InsertResult result;
  {
    std::lock_guard<std::mutex> lock(the_thread_pool_mutex);
    result = the_thread_pools.insert(std::make_pair(name, pool));
  }

  BOOST_ASSERT(result.second);
  if (!result.second) {
    cb(false);
    return;
  }

  pool->Initialize(name, thread_size, cb);
}


const std::shared_ptr<ThreadPool> &ThreadPool::Get(const std::string &name) {
  static const std::shared_ptr<ThreadPool> kNullThreadPool;

  std::lock_guard<std::mutex> lock(the_thread_pool_mutex);
  const auto itr = the_thread_pools.find(name);
  if (itr == the_thread_pools.end()) {
    BOOST_ASSERT(false);
    return kNullThreadPool;
  }

  return itr->second;
}


ThreadPool::ThreadPool() {
  impl_ = std::make_unique<Impl>();
}


void ThreadPool::Initialize(const std::string &name,
                            const size_t thread_size,
                            const CreateCallback &cb) {
  std::shared_ptr<ThreadPool> pool = shared_from_this();
  const auto impl_cb = [pool, cb](const bool error) {
    if (error) {
      cb(nullptr);
    }
    cb(pool);
  };

  impl_->Initialize(name, thread_size, impl_cb);
}


void ThreadPool::Post(const Function &function,
                      const std::string &serialization_tag) {
  if (serialization_tag.empty()) {
    impl_->Post(function);
  } else {
    impl_->Post(function, serialization_tag);
  }
}


const std::shared_ptr<boost::asio::io_service>
    ThreadPool::io_service() {
  return impl_->io_service();
}

}  // namespace libgs
