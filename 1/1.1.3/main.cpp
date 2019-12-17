#include "Counter.h"
#include <iostream>
#include <thread>

int main()
{
   std::cout << "Example in 1.1.3 of the book:" << std::endl;

   Counter counter;

   std::thread t1(&Counter::getAndIncrease, &counter);
   std::thread t2(&Counter::getAndIncrease, &counter);
   std::thread t3(&Counter::value, &counter);
   std::thread t4(&Counter::getAndIncrease, &counter);
   std::thread t5(&Counter::getAndIncrease, &counter);
   std::thread t6(&Counter::getAndIncrease, &counter);
   std::thread t7(&Counter::getAndIncrease, &counter);
   std::thread t8(&Counter::value, &counter);

   t1.join();
   t2.join();
   t3.join();
   t4.join();
   t5.join();
   t6.join();
   t7.join();
   t8.join();


   return 0;
}
