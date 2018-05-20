#include "stdafx.h"

#include "grouping/grouping.h"

#include <map>

#include "common/error.h"
#include "common/uuid.h"
#include "common/peer.h"

namespace libgs
{
struct Grouping::GroupingImpl
{
  std::map<Uuid, std::shared_ptr<Peer>> peers;

  std::size_t max_number;
};

Grouping::Grouping()
  : pimpl_(std::make_shared<Grouping::GroupingImpl>())
{

}

void Grouping::EnterPeer(
  std::shared_ptr<Peer> peer,
  ResultCallback cb)
{
  if(peer == nullptr)
  {
    cb(false, error_code::ERROR_NULL_PEER);
    return;
  }
  BOOST_ASSERT(peer);

  if(pimpl_->max_number >= pimpl_->peers.size())
  {
    cb(false, error_code::ERROR_MAX_PEER_NUMBER);
    return;
  }

  pimpl_->peers.insert(std::make_pair(peer->GetUuid(), peer));

  cb(true, error_code::ERROR_NO_ERROR);
}

void Grouping::ExitPeer(
  const Uuid& uuid,
  ResultCallback cb)
{
  if(pimpl_->peers.size() == 0)
  {
    cb(false, error_code::ERROR_EMPTY_GROUP);
    return;
  }

  pimpl_->peers.erase(uuid);

  cb(true, error_code::ERROR_NO_ERROR);
}

std::size_t Grouping::GetSize()
{
  return pimpl_->peers.size();
}

}


