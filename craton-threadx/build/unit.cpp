/* Copyright (C) 2014 Autotalks Ltd. */
#include <iostream>
#include <vector>
#include <string>
#include <atomic>

#include "unit.h"

class Animal
{
public:
  explicit Animal(const char *name): name_(name) { }

  void greet(void)
  {
    std::cout << name_ << ": " << sound() << std::endl;
  }

  virtual std::string sound(void) = 0;

  static void run_test(void);

protected:
  const char *name_;
};

class Dog: public Animal
{
public:
  explicit Dog(const char *name): Animal(name) { }

  virtual std::string sound(void)
  {
    return std::string("How-how!");
  }
};

class Parrot: public Animal
{
public:
  explicit Parrot(const char *name): Animal(name) { }

  virtual std::string sound(void)
  {
    auto str = std::string("Hello, my name is ");
    str.append(name_);
    return str;
  }
};

/* Dummy global variable used to test atomic<>::load.
   We need a global variable to avoid "unused variable" warning.
 */
uint64_t dummy_uint64;

void cxx_unit_test(void)
{
  /* Using C++ streams */
  std::cout << "*****" << std::endl;

#ifdef _DEFINED_ONLY_FOR_ARC1
  std::cout << "This is ARC1!" << std::endl;
#else
  std::cout << "Hello world!" << std::endl;
#endif

  Dog fido("Fido");
  Parrot polly("Polly");

  /* Using STL vector */
  std::vector<Animal *> animals;
  animals.push_back(&fido);
  animals.push_back(&polly);

  /* Using C++11 iteration syntax */
  for (auto &animal: animals) {
    animal->greet();
  }

  /* Using C++ atomics */
  std::atomic<uint64_t> x __attribute__((aligned(8)));
  std::atomic_fetch_add(&x, 37ULL);
  dummy_uint64 = x.load();

  std::cout << "*****" << std::endl;
}
