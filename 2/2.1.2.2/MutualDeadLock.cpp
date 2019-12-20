#include <mutex>
#include <thread>
#include <cstdio>
#include <chrono>
#include <set>
#include <iostream>

class Foo;

class FooContainer
{
public:
   void add(Foo* foo)
   {
      std::lock_guard<std::mutex> lock(mutex_);
      foo_.insert(foo);
   }

   void remove(Foo* foo)
   {
      std::lock_guard<std::mutex> lock(mutex_);
      foo_.erase(foo);
   }

   void printAll() const;

private:
   std::set<Foo*> foo_;
   mutable std::mutex mutex_;
};

FooContainer g_fooContainer;

class Foo
{
public:
   void process()
   {
      std::lock_guard<std::mutex> lock(mutex_);
      g_fooContainer.add(this);
   }

   void print() const
   {
      std::lock_guard<std::mutex> lock(mutex_);
      printf("Foo::print() unlocked\n");
   }

   ~Foo()
   {
      // Lock the two mutex in one function.
      std::lock_guard<std::mutex> lock(mutex_);
      std::this_thread::sleep_for(std::chrono::seconds(1));
      g_fooContainer.remove(this);
   }

private:
   mutable std::mutex mutex_;
};

void FooContainer::printAll() const
{
   // Lock the two mutex in one function.
   std::lock_guard<std::mutex> lock(mutex_);
   std::this_thread::sleep_for(std::chrono::seconds(1));

   for (const auto& foo : foo_)
   {
      foo->print();
   }

   printf("Foo::printAll() unlocked\n");
}

void threadFunc()
{
   Foo* foo = new Foo();
   foo->process();
   delete foo;
}

int main()
{
   std::cout << "Mutual Dead Lock" << std::endl;

   std::thread thread(threadFunc);

   std::this_thread::sleep_for(std::chrono::milliseconds(500));

   g_fooContainer.printAll();

   thread.join();

   return 0;
}