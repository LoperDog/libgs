#ifndef BOOST_TEST_MAIN
#define BOOST_TEST_MAIN
#endif

#include <boost/test/included/unit_test.hpp>

#include <string>

#include <include/libgs/common/uuid.h>


using boost::unit_test::test_suite;
using libgs::Uuid;


BOOST_AUTO_TEST_SUITE(UUID_TEST);


BOOST_AUTO_TEST_CASE(RANDOM_GENETRATE)
{
  Uuid uuid = Uuid::Generate();

  BOOST_ASSERT(!uuid.IsNil());
}


BOOST_AUTO_TEST_CASE(TO_STRING)
{
  Uuid uuid = Uuid::Generate();

  std::string uuid_str = uuid.ToString();

  BOOST_ASSERT(!uuid_str.empty());

  Uuid uuid2 = Uuid::FromString(uuid_str);

  BOOST_ASSERT(uuid.raw_uuid() == uuid2.raw_uuid());
}


BOOST_AUTO_TEST_SUITE_END()
