#ifndef INCLUDE_LIBGS_COMMON_ERROR_H_
#define INCLUDE_LIBGS_COMMON_ERROR_H_

namespace libgs
{

enum class error_code : uint64_t
{
  ERROR_NO_ERROR = 0,

  //////////////
  // Grouping //
  //////////////
  ERROR_NULL_PEER = 30001,    // Peer is nullptr
  ERROR_MAX_PEER_NUMBER,      // Over Max Number of group
  ERROR_EMPTY_GROUP,          // Group is Empty
};

}


#endif