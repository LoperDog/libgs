#include "stdafx.h"

#include "common/peer.h"

#include "common/uuid.h"

namespace libgs
{

struct Peer::PeerImpl
{
  Uuid uuid;
  PeerImpl(const Uuid& _uuid)
    : uuid(_uuid) {}
  ~PeerImpl() = default;
};

Peer::Peer(const Uuid& uuid)
  : pimpl_(std::make_shared<Peer::PeerImpl>(uuid))
{

}

Uuid Peer::GetUuid()
{
  return pimpl_->uuid;
}

}