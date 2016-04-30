#define BOOST_TEST_MODULE example

#include <base/LogStream.h>
#include <boost/test/included/unit_test.hpp>

using namespace base;

BOOST_AUTO_TEST_CASE(testLogStreamBooleans) {
  LogStream os;
  const LogStream::Buffer& buf = os.buffer();

  BOOST_CHECK_EQUAL(buf.toString(), string(""));
  os << true;
  BOOST_CHECK_EQUAL(buf.toString(), string("1"));
  os << '\n';
  BOOST_CHECK_EQUAL(buf.toString(), string("1\n"));
  os << false;
  BOOST_CHECK_EQUAL(buf.toString(), string("1\n0"));
}

BOOST_AUTO_TEST_CASE(testLogStreamIntegers) {
  LogStream os;
  const LogStream::Buffer& buf = os.buffer();

  BOOST_CHECK_EQUAL(buf.toString(), string(""));
  os << 1;
  BOOST_CHECK_EQUAL(buf.toString(), string("1"));
  os << 0;
  BOOST_CHECK_EQUAL(buf.toString(), string("10"));
  os << -1;
  BOOST_CHECK_EQUAL(buf.toString(), string("10-1"));

  os.resetBuffer();

  os << 0 << " " << 123 << 'x' << 0x64;
  BOOST_CHECK_EQUAL(buf.toString(), string("0 123x100"));
}
