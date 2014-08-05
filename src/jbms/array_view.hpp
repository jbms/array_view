#ifndef HEADER_GUARD_a51a7157f86a31d744e2508bdac9271d
#define HEADER_GUARD_a51a7157f86a31d744e2508bdac9271d

#include <boost/range/reference.hpp>
#include "jbms/is_contiguous.hpp"
#include <array>
#include <string>
#include <vector>
#include <type_traits>

namespace jbms {

template <class T>
class array_view;

template <class T>
struct is_contiguous_range<array_view<T>> : std::true_type {};

namespace array_view_detail {

template <class T>
struct base_type { using type = T; };

// We back array_view<void> by uint8_t *
template <>
struct base_type<void> { using type = uint8_t; };

// We back array_view<const void> by uint8_t const *
template <>
struct base_type<const void> { using type = const uint8_t; };

template <class T, class U>
struct is_constructible {
  constexpr static bool value = ((std::is_same<T, U>::value || std::is_same<T, const U>::value) ||
                                 (std::is_same<T,void>::value && !std::is_const<U>::value) ||
                                 std::is_same<T,const void>::value);
};

template <class T>
inline T *advance_pointer(T *x, std::ptrdiff_t n) {
  return x + n;
}

inline uint8_t *advance_pointer(void *x, std::ptrdiff_t n) {
  return (uint8_t *)x + n;
}

inline const uint8_t *advance_pointer(const void *x, std::ptrdiff_t n) {
  return (const uint8_t *)x + n;
}

}

template <class T>
class array_view  {
  using base_type_ = typename array_view_detail::base_type<T>::type;
public:
  using value_type = std::remove_const_t<base_type_>;
  using reference = base_type_ &;
  using const_reference = reference;
  using pointer = base_type_ *;
  using const_pointer = pointer;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using iterator = pointer;
  using const_iterator = pointer;

  constexpr array_view() : begin_(nullptr), end_(nullptr) {}

  // array_view<const void> can be constructed from any array_view<T>
  // array_view<void> can be constructed from any array_view<T> with non-const T
  template <class U, bool EnableCondition = array_view_detail::is_constructible<T,U>::value,
            std::enable_if_t<EnableCondition> * = nullptr>
  constexpr array_view(array_view<U> const &other)
    : begin_((value_type *)other.begin()), end_((value_type *)other.end()) {}

  template <class U, bool EnableCondition = array_view_detail::is_constructible<T,U>::value,
            std::enable_if_t<EnableCondition> * = nullptr>
  constexpr array_view(U *first, std::ptrdiff_t size)
    : begin_((value_type *)first), end_(array_view_detail::advance_pointer(first, size))
  {}

  constexpr array_view(T *begin_, T *end_)
    : begin_((value_type *)begin_), end_((value_type *)end_)
  {}

  template <
      class Other,
      bool EnableCondition =
          (is_contiguous_range<std::remove_const_t<std::remove_reference_t<Other>>>::value &&
           array_view_detail::is_constructible<
              T,
              std::remove_reference_t<typename boost::range_reference<std::remove_reference_t<Other>>::type>>::value),
      std::enable_if_t<EnableCondition> * = nullptr>
  array_view(Other &&other)
    : begin_((value_type *)&*boost::begin(other)), end_((value_type *)&*boost::end(other)) {}

  constexpr pointer begin() const { return begin_; }
  constexpr pointer end() const { return end_; }

  constexpr size_t size() const { return end_ - begin_; }
  constexpr bool empty() const { return end_ == begin_; }
  constexpr explicit operator bool () const { return !empty(); }

  constexpr pointer data() const { return begin_; }

  array_view &advance_begin(std::ptrdiff_t n) {
    begin_ += n;
    return *this;
  }
  array_view &advance_end(std::ptrdiff_t n) {
    end_ += n;
    return *this;
  }

  // No checking
  constexpr array_view unchecked_slice(ptrdiff_t pos, ptrdiff_t len) const {
    return { this->begin() + pos, this->begin() + pos + len };
  }

  constexpr array_view unchecked_slice_before(ptrdiff_t pos) const {
    return { this->begin(), this->begin() + pos };
  }

  constexpr array_view unchecked_slice_after(ptrdiff_t pos) const {
    return { this->begin() + pos, this->end() };
  }

  // unchecked
  constexpr reference operator[](ptrdiff_t pos) const {
    return this->begin()[pos];
  }

  constexpr reference front() const { return this->begin()[0]; }
  constexpr reference back() const { return this->end()[-1]; }


  // checked
  constexpr reference at(ptrdiff_t pos) const {
    return *((pos >= 0 && pos <= ptrdiff_t(size())) ? (begin_ + pos) : throw std::out_of_range("jbms::array_view"));
  }

  // Bounds are checked, exception thrown if out of bounds
  constexpr array_view checked_slice(ptrdiff_t pos, ptrdiff_t len) const {
    return { &at(pos), &at(pos + len) };
  }

  constexpr array_view checked_slice_before(ptrdiff_t pos) const {
    return { this->begin(), &at(pos) };
  }

  constexpr array_view checked_slice_after(ptrdiff_t pos) const {
    return { &at(pos), this->end() };
  }

private:
  constexpr pointer bounded_ptr_(ptrdiff_t pos) {
    return begin() + std::min(ptrdiff_t(size()), std::max(pos, ptrdiff_t(0)));
  }
public:
  // Result is truncated to fit within bounds
  // Arguments are of a signed type to avoid silent conversion of negative values
  constexpr array_view slice(ptrdiff_t pos, ptrdiff_t len) const {
    return { bounded_ptr_(pos), bounded_ptr_(pos + len) };
  }

  constexpr array_view slice_before(ptrdiff_t pos) const {
    return { this->begin(), bounded_ptr_(pos) };
  }

  constexpr array_view slice_after(ptrdiff_t pos) const {
    return { bounded_ptr_(pos), this->end() };
  }

private:
  pointer begin_, end_;
};

template <class CharT, class Traits, class T>
std::ostream &operator<<(std::basic_ostream<CharT,Traits> &os,
                         array_view<T> const &x) {
  os << "{";
  bool is_first = true;
  for (auto &&v : x) {
    if (!is_first)
      os << ", ";
    is_first = false;
    os << v;
  }
  os << "}";
  return os;
}

template <class T, class U,
          std::enable_if_t<std::is_convertible<array_view<T>,array_view<const U>>::value ||
                           std::is_convertible<array_view<U>,array_view<const T>>::value> * = nullptr>
constexpr bool operator==(array_view<T> const &a, array_view<U> const &b) {
  return a.begin() == b.begin() && a.end() == b.end();
}

template <class T, class U,
          std::enable_if_t<std::is_convertible<array_view<T>,array_view<const U>>::value ||
                           std::is_convertible<array_view<U>,array_view<const T>>::value> * = nullptr>
constexpr bool operator!=(array_view<T> const &a, array_view<U> const &b) {
  return !(a == b);
}

template <class T, class Other,
          std::enable_if_t<std::is_convertible<Other,array_view<const T>>::value> * = nullptr>
constexpr bool operator==(array_view<T> const &a, Other const &b) {
  return a.begin() == &*boost::begin(b) && a.end() == &*boost::end(b);
}

template <class T, class Other,
          std::enable_if_t<std::is_convertible<Other,array_view<const T>>::value> * = nullptr>
constexpr bool operator==(Other const &b, array_view<T> const &a) {
  return a.begin() == &*boost::begin(b) && a.end() == &*boost::end(b);
}

template <class T, class Other,
          std::enable_if_t<std::is_convertible<Other,array_view<const T>>::value> * = nullptr>
constexpr bool operator!=(array_view<T> const &a, Other const &b) {
  return !(a == b);
}

template <class T, class Other,
          std::enable_if_t<std::is_convertible<Other,array_view<const T>>::value> * = nullptr>
constexpr bool operator!=(Other const &b, array_view<T> const &a) {
  return !(a == b);
}

template <class T>
array_view<T> make_view(T *data, std::ptrdiff_t n) {
  return { data, n };
}

template <class T>
array_view<T> make_view(T *begin, T *end) {
  return { begin, end };
}

template <class Other,
          bool EnableCondition = is_contiguous_range<std::remove_const_t<std::remove_reference_t<Other>>>::value,
          std::enable_if_t<EnableCondition> * = nullptr>
array_view<std::remove_reference_t<typename boost::range_reference<std::remove_reference_t<Other>>::type>>
make_view(Other &&other) {
  return { other };
}
}

#endif /* HEADER GUARD */
