#ifndef SEMAPHORE_HPP
#define SEMAPHORE_HPP

#include <condition_variable>
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>

namespace di378 {

struct semaphore
{
  semaphore() : semaphore(0) {}
  explicit semaphore(std::size_t initial_count)
    : m_count(initial_count) {}

  auto wait()
  {
    std::unique_lock<std::mutex> lk(m_mutex);
    if (m_count > 0) {
      --m_count;
    } else {
      m_cv.wait(lk, [this] { return m_count > 0; });
      --m_count;
    }
  }

  auto wait(std::size_t count)
  {
    for (std::size_t i(0); i < count; ++i) {
      wait();
    }
  }

  auto signal()
  {
    std::unique_lock<std::mutex> lk(m_mutex);
    ++m_count;
    lk.unlock();
    m_cv.notify_one();
  }

  auto get_count() const
  {
    std::unique_lock<std::mutex> lk(m_mutex);
    return m_count;
  }

private:
  std::size_t m_count;
  mutable std::mutex m_mutex;
  std::condition_variable m_cv;
};

}

#endif
