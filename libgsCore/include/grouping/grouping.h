#ifndef INCLUDE_LIBGS_GROUPING_GROUPING_H_
#define INCLUDE_LIBGS_GROUPING_GROUPING_H_

#include <functional>
#include <memory>

namespace libgs
{

class Peer;
class Uuid;

enum class error_code : uint64_t;

class Grouping
{
  using ResultCallback = std::function<void(bool, error_code ec)>;
 public:
  Grouping();
  ~Grouping() = default;

  // Enter Peer 
  void EnterPeer(
    std::shared_ptr<Peer> peer,  // Target Peer
    ResultCallback cb  // Result Callback
    );

  void ExitPeer(
    const Uuid& uuid, // Target Peer's Uuid
    ResultCallback cb // Result Callback
    );

  std::size_t GetSize();

 private:
  struct GroupingImpl;
  std::shared_ptr<GroupingImpl> pimpl_;
};
}

#endif