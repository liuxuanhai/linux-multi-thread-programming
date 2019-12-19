#include <mutex>
#include <iostream>

class NonRecursiveMutexRequest
{
public:
   void process()
   {
      std::lock_guard<std::mutex> lock(mutex_);

      std::cout << "NonRecursiveMutexRequest::process()" << std::endl;

      print();
   }

   void print()
   {
      std::lock_guard<std::mutex> lock(mutex_);

      std::cout << "NonRecursiveMutexRequest::print()" << std::endl;
   }

private:
   mutable std::mutex mutex_;
};

class RecursiveMutextRequest
{
public:
   void process()
   {
      std::lock_guard<std::recursive_mutex> lock(mutex_);

      std::cout << "RecursiveMutextRequest::process()" << std::endl;

      print();
   }

   void print()
   {
      std::lock_guard<std::recursive_mutex> lock(mutex_);

      std::cout << "RecursiveMutextRequest::print()" << std::endl;
   }

private:
   mutable std::recursive_mutex mutex_;
};

int main()
{
   RecursiveMutextRequest nonDeadLockReq;
   nonDeadLockReq.process();

   NonRecursiveMutexRequest deadLockReq;
   deadLockReq.process();
}