#ifndef INCLUDE_LIBGS_COMMON_PEER_H_
#define INCLUDE_LIBGS_COMMON_PEER_H_
#include <memory>

namespace libgs
{

class Uuid;

class Peer
{
 public:
  Peer(const Uuid& uuid);
  ~Peer() = default;

  Uuid GetUuid();

 private:
  struct PeerImpl;
  std::shared_ptr<PeerImpl> pimpl_;
};

}

#endif