#include <iostream>
#include <memory>
#include <map>
#include <unordered_map>
#include <ptr.hpp>

struct Base : public dsg::PtrPolicy {
  Base() { std::cout << "Base() " << use_count() << "\n"; }
  ~Base() { std::cout << "~Base() " << use_count() << "\n"; }
  virtual void print() { std::cout << "Base::print " << use_count() << "\n"; }
};

struct DBase : public Base {
  DBase() : Base() { std::cout << "DBase() " << use_count() << "\n"; }
  ~DBase() { std::cout << "~DBase() " << use_count() << "\n"; }
  void print() override { std::cout << "DBase::print " << use_count() << "\n"; }
};

struct Base1 : public dsg::LocalPtrPolicy {
  Base1() { std::cout << "Base1() " << use_count() << "\n"; }
  ~Base1() { std::cout << "~Base1() " << use_count() << "\n"; }
  void print() { std::cout << "Base1::print " << use_count() << "\n"; }
};

int main(int argc, char** argv) {
  // auto p0 = dsg::ptr<Base>();
  // p0->print();

  dsg::ptr<Base> p = dsg::make_ptr<Base>();
  p->print();

  auto p1 = p;
  p1->print();

  auto p2 = std::move(p1);
  p2->print();

  (*p2).print();

  dsg::ptr<Base> p3;
  p3 = p2;
  p3->print();

  dsg::ptr<Base> p4;
  p4 = std::move(p3);
  p4->print();
  // p3->print();

  if (!p3) {
    std::cout << "p3 null\n";
  }

  p4.get()->print();
  if (!p4.unique()) {
    std::cout << "p4 unique false\n";
  }

  std::cout << "cur ptr " << p4.use_count() << "\n";

  if (p3 != p4) {
    std::cout << "p3 != p4\n";
  }

  dsg::ptr<Base1> pp = dsg::make_ptr<Base1>();
  pp->print();

  if (p2 == p4) {
    std::cout << "p2 == p4\n";
  }

  dsg::scope_ptr<Base> sp{p4, [](Base* b) {
                            std::cout << "scope_ptr\n";
                            b->print();
                          }};
  sp.apply();

  dsg::ptr<DBase> p5 = dsg::make_ptr<DBase>();
  p5->print();

  std::cout << "-------\n";
  dsg::ptr<Base> p6 = dsg::dynamic_pointer_cast<Base>(p5);
  p6->print();
  dsg::ptr<Base> p7 = dsg::dynamic_pointer_cast<Base>(std::move(p5));
  p7->print();
  // p5->print();

  auto p8 = dsg::make_ptr<const DBase>();
  dsg::ptr<DBase> p9 = dsg::const_pointer_cast<DBase>(p8);
  p9->print();
  std::cout << "-------\n";
  std::map<dsg::ptr<Base>, std::string> map1;
  map1[p7] = "hello";

  std::unordered_map<dsg::ptr<Base>, std::string> map2;
  map2[p7] = "world";

  std::cout << "map:" << map1[p7] << "\n";
  std::cout << "unordered_map:" << map2[p7] << "\n";

  std::cout << "-------\n";

  return 0;
}