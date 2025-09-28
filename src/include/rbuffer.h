#ifndef RBUFFER_H
#define RBUFFER_H
#include <array>
#include <assert.h>
#include <functional>

template <typename T, std::size_t m_size> class Rbuffer {
  std::array<T, m_size> m_buf;
  std::size_t m_pos{0};
  int m_last{-1};
  bool m_empty{true};

public:
  Rbuffer(){};

  using criteria = std::function<bool(const T &)>;

  std::size_t size() const { return m_size; }
  auto begin() { return m_buf.begin(); }
  auto end() { return m_buf.end(); }
  bool empty() const {return m_empty;}
  T &operator[](std::size_t i) { return m_buf[i]; }

  void pushback(const T &value) {
    m_buf[m_pos] = value;
    m_last = m_pos;
    ++m_pos;
    m_pos %= m_size;
    m_empty = false;
  }
  void pushback(T &&value) {
    m_buf[m_pos] = (std::move(value));
    m_last = m_pos;
    ++m_pos;
    m_pos %= m_size;
    m_empty = false;
  }

  const T& top() const {
    assert(m_last >= 0);
    return m_buf[m_last];
  }

  bool contains(criteria fx) const {
    for (const auto &item : m_buf) {
      if (fx(item))
        return true;
    }
    return false;
  };
};
#endif // RBUFFER_H
