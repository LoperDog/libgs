#include <functional>
#include "core/include/libgs/common/uuid.h"


namespace libgs {

class DistributionLock {
 public:
  enum class LockType {
    kNone,
    kReadLock,
    kWriteLock,
  };

  enum class State {
    kNotProcessing,  // 아무 처리도 되지 않은 상태
    kWaitingForAcquire,  // 다른 서버가 이미 락을 획득한 경우
    kWaitingForAcquire2,
    // 이미 락을 가진 상태에서 expire 연장을 요청하는 경우
    kReadLockAcquired,
    kWriteLockAcquired,
    kRelease
  };

  typedef std::function<void(const bool /*success*/)> Callback;
  void Lock(const Uuid &uuid, const LockType &lock_type,
            const Callback &cb);
  void Unlock(const Uuid &uuid);

  const State &state(const Uuid &uuid) const;
};

}  // namespace libgs