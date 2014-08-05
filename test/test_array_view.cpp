#include <boost/test/unit_test.hpp>
#include <type_traits>
#include "jbms/array_view.hpp"
#include <string>
#include <array>
#include <vector>
#include <list>

static_assert(jbms::is_contiguous_range<std::string>::value == true, "");
static_assert(jbms::is_contiguous_range<std::vector<int>>::value == true, "");
static_assert(jbms::is_contiguous_range<std::array<int,5>>::value == true, "");
static_assert(jbms::is_contiguous_range<boost::iterator_range<int *>>::value == true, "");
static_assert(jbms::is_contiguous_range<int[5]>::value == true, "");
static_assert(jbms::is_contiguous_range<std::list<int>>::value == false, "");

static_assert(std::is_convertible<std::string,jbms::array_view<char>>::value == true,"");
static_assert(std::is_convertible<std::string,jbms::array_view<void>>::value == true,"");
static_assert(std::is_convertible<std::vector<int>,jbms::array_view<int>>::value == true,"");
static_assert(std::is_convertible<std::string,jbms::array_view<unsigned char>>::value == false,"");
static_assert(std::is_convertible<std::array<int,5> &,jbms::array_view<int>>::value == true,"");
static_assert(std::is_convertible<unsigned char (&)[5],jbms::array_view<unsigned char>>::value == true,"");

static_assert(std::is_convertible<const unsigned char (&)[5],jbms::array_view<const unsigned char>>::value == true,"");
static_assert(std::is_convertible<const unsigned char (&)[5],jbms::array_view<unsigned char>>::value == false,"");
static_assert(std::is_convertible<const jbms::array_view<int>,jbms::array_view<int>>::value == true,"");
static_assert(std::is_convertible<const jbms::array_view<int>,jbms::array_view<const int>>::value == true,"");
static_assert(std::is_convertible<jbms::array_view<const int>,jbms::array_view<const void>>::value == true,"");
static_assert(std::is_convertible<jbms::array_view<const int>,jbms::array_view<void>>::value == false,"");


BOOST_AUTO_TEST_CASE(test_make_view) {

  {
    std::string x = "Hello";
    auto y = jbms::make_view(x);
    static_assert(std::is_same<decltype(y),jbms::array_view<char>>::value == true,"");
    BOOST_REQUIRE_EQUAL(y.data(), x.data());
    BOOST_REQUIRE_EQUAL(y.size(), x.size());
  }

  {
    std::vector<char> x{'H','e','l','l','o'};
    auto y = jbms::make_view(x);
    static_assert(std::is_same<decltype(y),jbms::array_view<char>>::value == true,"");
    BOOST_REQUIRE_EQUAL(y.data(), x.data());
    BOOST_REQUIRE_EQUAL(y.size(), x.size());
  }

  {
    std::array<char,5> x{'H','e','l','l','o'};
    auto y = jbms::make_view(x);
    static_assert(std::is_same<decltype(y),jbms::array_view<char>>::value == true,"");
    BOOST_REQUIRE_EQUAL(y.data(), x.data());
    BOOST_REQUIRE_EQUAL(y.size(), x.size());
  }

  {
    char x[] = {'H','e','l','l','o'};
    auto y = jbms::make_view(x);
    static_assert(std::is_same<decltype(y),jbms::array_view<char>>::value == true,"");
    BOOST_REQUIRE_EQUAL(y.data(), &x[0]);
    BOOST_REQUIRE_EQUAL(y.size(), 5);
  }


}

BOOST_AUTO_TEST_CASE(test_slice) {

  std::string x = "Hello";
  std::string y = "Hello";

  auto v = jbms::make_view(x);

  BOOST_REQUIRE(v == x);
  BOOST_REQUIRE(!(v == y));

  BOOST_REQUIRE_EQUAL(v.unchecked_slice(1,3), jbms::make_view(&x[1], &x[4]));
  BOOST_REQUIRE_EQUAL(v.unchecked_slice_after(1), jbms::make_view(&x[1], &x[5]));
  BOOST_REQUIRE_EQUAL(v.unchecked_slice_before(2), jbms::make_view(&x[0], &x[2]));

  BOOST_REQUIRE_EQUAL(v.slice(1,3), jbms::make_view(&x[1], &x[4]));
  BOOST_REQUIRE_EQUAL(v.slice_after(1), jbms::make_view(&x[1], &x[5]));
  BOOST_REQUIRE_EQUAL(v.slice_before(2), jbms::make_view(&x[0], &x[2]));

  BOOST_REQUIRE_EQUAL(v.checked_slice(1,3), jbms::make_view(&x[1], &x[4]));
  BOOST_REQUIRE_EQUAL(v.checked_slice_after(1), jbms::make_view(&x[1], &x[5]));
  BOOST_REQUIRE_EQUAL(v.checked_slice_before(2), jbms::make_view(&x[0], &x[2]));

  BOOST_REQUIRE_EQUAL(v.slice(-3,10), v);
  BOOST_REQUIRE_EQUAL(v.slice(-3,7), jbms::make_view(&x[0],&x[4]));
  BOOST_REQUIRE_EQUAL(v.slice(-3,2), jbms::make_view(&x[0], &x[0]));
  BOOST_REQUIRE_EQUAL(v.slice(-3,5), jbms::make_view(&x[0], &x[2]));
  BOOST_REQUIRE_EQUAL(v.slice_after(10), jbms::make_view(&x[5], &x[5]));
  BOOST_REQUIRE_EQUAL(v.slice_before(10), v);
  BOOST_REQUIRE_EQUAL(v.slice_before(-3), jbms::make_view(&x[0], &x[0]));

  BOOST_REQUIRE(v.slice_after(10).empty());
}
