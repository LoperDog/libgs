#ifndef NETWORK_METHOD_H
#define NETWORK_METHOD_H

#include <boost/enable_shared_from_this.hpp>

namespace app {
  // NOTE[loperdog] : 사용자의 이벤트를 매핑하기위해 상속 형태를 정한 클래스
  class NetworkMethod : public boost::enable_shared_from_this<NetworkMethod>
  {
  private:
    NetworkMethod() {};
  public:
    boost::shared_ptr<NetworkMethod> GetThis()
    {
      return shared_from_this();
    }
  };
}


#endif // !NETWORK_METHOD_H
