#include "Counter.h"
#include <iostream>

int64_t Counter::value() const
{
   const std::lock_guard<std::mutex> lock(mutex_);
   std::cout << "value: " << value_ << std::endl;
   return value_;
}

int64_t Counter::getAndIncrease()
{
   const std::lock_guard<std::mutex> lock(mutex_);
   value_++;
   std::cout << "Increase value to: " << value_ << std::endl;
   return value_;
}
