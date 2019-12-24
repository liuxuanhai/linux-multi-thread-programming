#include "Observable.h"
#include <algorithm>
#include <iostream>

void Observable::regiser(std::weak_ptr<Observer> observer)
{
   observers_.push_back(observer);
}

void Observable::notifyObservers()
{
   std::lock_guard<std::mutex> lock(mutex_);
   auto it = observers_.begin();
   while (it != observers_.end())
   {
      auto observer = it->lock();
      if (observer)
      {
         observer->update();
         ++it;
      }
      else
      {
         std::cout << "notifyObservers() erase" << std::endl;
         it = observers_.erase(it);
      }
   }
}
