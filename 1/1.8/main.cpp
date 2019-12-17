#include "Observable.h"
#include "Foo.h"
#include <iostream>
#include <memory>


int main()
{
   std::cout << "Observer Thread Safe: " << std::endl;

   Observable subject;

   {
      auto foo = std::make_shared<Foo>();
      subject.regiser(foo);
      subject.notifyObservers();
   }

   subject.notifyObservers();

   return 0;
}
