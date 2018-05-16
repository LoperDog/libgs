
#include "common.h"

#ifndef UUID_UUID_H
#define UUID_UUID_H

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

class object
{
public:
  object()
    : tag(boost::uuids::random_generator()())
    , state(0)
  {}

  explicit object(int state)
    : tag(boost::uuids::random_generator()())
    , state(state)
  {}

  object(object const& rhs)
    : tag(rhs.tag)
    , state(rhs.state)
  {}

  bool operator==(object const& rhs) const {
    return tag == rhs.tag;
  }

  object& operator=(object const& rhs) {
    tag = rhs.tag;
    state = rhs.state;
  }

  int get_state() const { return state; }
  void set_state(int new_state) { state = new_state; }

private:
  boost::uuids::uuid tag;

  int state;
};
#endif // !UUID_UUID_H
