#pragma once

#include <mutex>

class Counter
{
public:
   Counter() : value_(0) {}

   int64_t value() const;
   int64_t getAndIncrease();

private:
   Counter(const Counter&) = delete;
   Counter& operator=(const Counter&) = delete;

private:
   int64_t value_;
   // mutable mutex_ can be used in const member functions, and it should be mutable because
   // "lock_guard" requires a non-const mutex.
   mutable std::mutex mutex_;
};
