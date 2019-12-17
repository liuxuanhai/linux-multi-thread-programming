#pragma once

#include "Observer.h"
#include <memory>
#include <mutex>
#include <vector>

class Observable
{
public:
   void regiser(std::weak_ptr<Observer> observer);
   void notifyObservers();
private:
   mutable std::mutex mutex_;
   std::vector<std::weak_ptr<Observer>> observers_;
};
