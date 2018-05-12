#include "core/src/distribution/lock.h"

#include <set>
#include <map>
#include <memory>
#include <mutex>

#include "core/time/date_time.h"

#include <boost/assert.hpp>


namespace libgs {

namespace {

using LockType DistributionLock::LockType;
using State DistributionLock::State;


struct LockContext {
  struct Comparer {
    bool operator()(const LockContext &lhs,
                    const LockContext &rhs) const {
      return lhs.raw_uuid() < rhs.raw_uuid();
    }
  };

  LockContext(const Uuid &_uuid, const LockType &_lock_type,
              const Timestamp _timestamp_in_msec)
        : uuid(_uuid), lock_type(_lock_type),
          timestamp_in_msec(_timestamp_in_msec) {
  }

  const Uuid uuid;
  LockType lock_type;
  Timestamp timestamp_in_msec;
  State state;
  std::set<std::shared_ptr<LockContext> > processing_contexts;
};


using LockContexts std::set<std::shared_ptr<LockContext> >;
using LockOrderedContextMap std::map<const Timestamp, LockContexts>;


const size_t kLockExpireDurationInMsec = 180 * 1000;


std::mutex the_mutex;
Uuid the_server_uuid;

// TODO(inkeun):
// 락 점유 시간이 만료되었을때 auto release 를 하거나 막기위해서 가지고 있는다.
// 추후에 구현한다.
LockOrderedContextMap the_ordered_contexts;
LockContexts the_contexts;

}  // unnnamed namespace


void DistributionLock::Lock(const Uuid &uuid, const LockType &lock_type,
                            const Callback &cb) {
  BOOST_ASSERT(!uuid.IsNil());
  BOOST_ASSERT(lock_type != kNone);
  BOOST_ASSERT(cb);

  LockContext context =
      std::make_shared<LockContext>(
          uuid, lock_type,
          DateTime::GetTimestamp(DateTime::kMilliseconds),
          kNotProcessing);

  // TODO(inkeun): 이 구간에서 락을 잘게 쪼갤 수 있는지 확인해본다.
  // 락 범위가 너무 큰 상태인데, 줄일 수 있을 것으로 보인다.
  std::lock_guard lock(the_mutex);

  const auto itr = the_contexts.find(context);

  // 이미 이 서버에서 락을 잡은 경우이다.
  if (itr != the_contexts.end()) {
    // 언락 단계인 경우, 락을 다시 잡도록 해야한다.
    if (itr->state == kRelease) {
      // TODO(inkeun): io_service->post this function;
    }

    // read lock을 이미 획득했는데 write lock을 사용하려는 경우
    // read lock 여부와 무관하게 write lock 대기열에 추가해야한다.
    if (itr->lock_type == kReadLock && lock_type == kWriteLock) {
      // TODO(inkeun): add write lock waitting;
    }

    // WriteLock으로 잡고 ReadLock을 추가로 요청하는 경우
    // ReadLock을 잡고 ReadLock을 추가로 요청하는 경우가 여기에 해당한다.

    // 처리 중 목록에 추가한다.

    if (lock_type == kReadLock) {
      context->state = kReadLockAcquired;
    } else if (lock_type == kWriteLock) {
      context->state = kWriteLockAcquired;
    }

    itr->processing_contexts.insert(context);
    cb(true);
  } else {
    // TODO(inkeun): 레디스로 요청을 보내도록 추가한다.
  }
}


void DistributionLock::Unlock(const Uuid &uuid) {
  BOOST_ASSERT(!uuid.IsNil());

  LockContext context =
      std::make_shared<LockContext>(
          uuid, kNone, 0,
          kNotProcessing);

  // TODO(inkeun): 이 구간에서 락을 잘게 쪼갤 수 있는지 확인해본다.
  // 락 범위가 너무 큰 상태인데, 줄일 수 있을 것으로 보인다.
  std::lock_guard lock(the_mutex);

  const auto itr = the_contexts.find(context);

  // 이 서버에서 락을 잡지 않은 경우이다. 발생하면 안되는 경우이다.
  BOOST_ASSERT(itr == the_contexts.end());

  // 이 context 가 락을 잡지 않았다. 발생하면 안되는 경우이다.
  const auto itr2 = itr->processing_contexts.find(context);
  BOOST_ASSERT(itr2 == itr->processing_contexts.end());

  // 처리 중 목록에서 제거한다.
  itr2.erase(itr2->second);

  // 아직 처리 중인 목록이 남아있다면 달리 할 것이 없다.
  if (itr2->size() > 0) {
    // TODO(inkeun): 추후에, 아래와 같이 수정해야 할 지 고민해본다.
    // 1. WriteLock을 잡은 상태로 WriteLock을 모두 processing 한 경우
    // (write processing 이 끝나고 read processing만 남은 경우
    //  read lock으로 operation 변경)
    return;
  }

  // 릴리즈 상태로 변경한다. 실제로 다른 서버로 락이 넘어가는 경우는
  // 레디스에서 처리가 모두 이뤄지고 난뒤이다.
  itr->second->state = kRelease;
  the_context.erase(itr->second);
  // 레디스에서 언락하는 코드를 추가한다.
}


const State &DistributionLock::state(const Uuid &uuid) const {
  BOOST_ASSERT(!uuid.IsNil());

  const auto itr = the_contexts.find(context);
  if (itr == the_contexts.end()) {
    return kNone;
  }

  return itr->second->state;
}

}  // namespace libgs