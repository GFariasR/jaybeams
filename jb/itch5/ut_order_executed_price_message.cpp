#include <jb/itch5/order_executed_price_message.hpp>
#include <jb/itch5/testing/data.hpp>

#include <boost/test/unit_test.hpp>

/**
 * @test Verify that the jb::itch5::order_executed_price_message decoder works
 * as expected.
 */
BOOST_AUTO_TEST_CASE(decode_order_executed_price_message) {
  using namespace jb::itch5;
  using namespace std::chrono;

  auto buf = jb::itch5::testing::order_executed_price();
  auto expected_ts = jb::itch5::testing::expected_ts();

  auto x =
      decoder<true, order_executed_price_message>::r(buf.second, buf.first, 0);
  BOOST_CHECK_EQUAL(
      x.header.message_type, order_executed_price_message::message_type);
  BOOST_CHECK_EQUAL(x.header.stock_locate, 0);
  BOOST_CHECK_EQUAL(x.header.tracking_number, 1);
  BOOST_CHECK_EQUAL(x.header.timestamp.ts.count(), expected_ts.count());
  BOOST_CHECK_EQUAL(x.order_reference_number, 42ULL);
  BOOST_CHECK_EQUAL(x.executed_shares, 300);
  BOOST_CHECK_EQUAL(x.match_number, 317ULL);
  BOOST_CHECK_EQUAL(x.printable, printable_t(u'Y'));
  BOOST_CHECK_EQUAL(x.execution_price, price4_t(1230500));

  x = decoder<false, order_executed_price_message>::r(buf.second, buf.first, 0);
  BOOST_CHECK_EQUAL(
      x.header.message_type, order_executed_price_message::message_type);
  BOOST_CHECK_EQUAL(x.header.stock_locate, 0);
  BOOST_CHECK_EQUAL(x.header.tracking_number, 1);
  BOOST_CHECK_EQUAL(x.header.timestamp.ts.count(), expected_ts.count());
  BOOST_CHECK_EQUAL(x.order_reference_number, 42ULL);
  BOOST_CHECK_EQUAL(x.order_reference_number, 42ULL);
  BOOST_CHECK_EQUAL(x.executed_shares, 300);
  BOOST_CHECK_EQUAL(x.match_number, 317ULL);
  BOOST_CHECK_EQUAL(x.printable, printable_t(u'Y'));
  BOOST_CHECK_EQUAL(x.execution_price, price4_t(1230500));
}

/**
 * @test Verify that jb::itch5::order_executed_price_message iostream
 * operator works as expected.
 */
BOOST_AUTO_TEST_CASE(stream_order_executed_price_message) {
  using namespace std::chrono;
  using namespace jb::itch5;

  auto buf = jb::itch5::testing::order_executed_price();
  auto tmp =
      decoder<false, order_executed_price_message>::r(buf.second, buf.first, 0);
  std::ostringstream os;
  os << tmp;
  BOOST_CHECK_EQUAL(
      os.str(), "message_type=C,stock_locate=0"
                ",tracking_number=1,timestamp=113231.123456789"
                ",order_reference_number=42"
                ",executed_shares=300"
                ",match_number=317"
                ",printable=Y"
                ",execution_price=123.0500");
}
