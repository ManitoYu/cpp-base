#include <net/InetAddress.h>

#define BOOST_TEST_MODULE InetAddressTest
#include <boost/test/included/unit_test.hpp>

using std::string;
using base::net::InetAddress;

BOOST_AUTO_TEST_CASE(testInetAddress) {
  InetAddress addr0(1234);
  BOOST_CHECK_EQUAL(addr0.toIp(), string("0.0.0.0"));
  BOOST_CHECK_EQUAL(addr0.toIpPort(), string("0.0.0.0:1234"));
  BOOST_CHECK_EQUAL(addr0.toPort(), 1234);

  InetAddress addr1(4321, true);
  BOOST_CHECK_EQUAL(addr1.toIp(), string("127.0.0.1"));
  BOOST_CHECK_EQUAL(addr1.toIpPort(), string("127.0.0.1:4321"));
  BOOST_CHECK_EQUAL(addr1.toPort(), 4321);

  InetAddress addr2("1.2.3.4", 8888);
  BOOST_CHECK_EQUAL(addr2.toIp(), string("1.2.3.4"));
  BOOST_CHECK_EQUAL(addr2.toIpPort(), string("1.2.3.4:8888"));
  BOOST_CHECK_EQUAL(addr2.toPort(), 8888);
}
