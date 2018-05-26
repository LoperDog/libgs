#ifndef INCLUDE_LIBGS_COMMON_UUID_H_
#define INCLUDE_LIBGS_COMMON_UUID_H_


#include <string>

#include <boost/uuid/uuid.hpp>


namespace libgs {

class Uuid {
 public:
  static Uuid FromString(const std::string &uuid);
  static Uuid Generate();

  static boost::uuids::uuid kNilUuid;
  Uuid() {}
  Uuid(const boost::uuids::uuid &uuid);

  const bool IsNil() const;
  const std::string ToString() const;

  const boost::uuids::uuid &raw_uuid() const;

 private:
  boost::uuids::uuid uuid_;
};

}  // namespace libgs

#endif  // INCLUDE_LIBGS_COMMON_UUID_H_
