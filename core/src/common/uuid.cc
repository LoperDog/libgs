#include "core/include/libgs/common/uuid.h"

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>


namespace libgs {

Uuid Uuid::FromString(const std::string &uuid) {
  boost::uuids::string_generator gen;
  return Uuid(gen(uuid));
}


Uuid Uuid::Generate() {
  // TODO(inkeun): 풀링 하도록 추후 변경
  boost::uuids::random_generator gen;
  return Uuid(gen());
}


Uuid::Uuid(const boost::uuids::uuid &uuid) {
  uuid_ = uuid;
}


const bool Uuid::IsNil() const {
  return uuid_ == Uuid::kNilUuid;
}


const std::string Uuid::ToString() const {
  // TODO(inkeun): BAD CAST 인 경우 처리 추가
  return boost::lexical_cast<std::string>(uuid_);
}


const boost::uuids::uuid &Uuid::raw_uuid() const {
  return uuid_;
}


boost::uuids::uuid Uuid::kNilUuid = boost::uuids::nil_uuid();

}  // namespace libgs