#include <iostream>
#include <memory>
#include <map>
#include <unordered_map>
#include <ptr.hpp>

using namespace dsg;

struct I {
  virtual ~I() = default;
  virtual void test() { std::cout << "I::test \n"; };
};

struct Base : public safe_ref<I> {
  Base() { std::cout << "Base() " << use_count() << "\n"; }
  virtual ~Base() { std::cout << "~Base() " << use_count() << "\n"; }
  virtual void print() { std::cout << "Base::print " << use_count() << "\n"; }
  virtual void test() override {
    std::cout << "Base::test " << use_count() << "\n";
  }
};

struct DBase : public Base {
  DBase() : Base() { std::cout << "DBase() " << use_count() << "\n"; }
  ~DBase() { std::cout << "~DBase() " << use_count() << "\n"; }
  void print() override { std::cout << "DBase::print " << use_count() << "\n"; }
  void test() override { std::cout << "DBase::test " << use_count() << "\n"; }
};

struct Base1 : public unsafe_ptr_policy {
  Base1() { std::cout << "Base1() " << use_count() << "\n"; }
  ~Base1() { std::cout << "~Base1() " << use_count() << "\n"; }
  void print() { std::cout << "Base1::print " << use_count() << "\n"; }
};

struct DBase1 : public Base1 {
  DBase1() : Base1() { std::cout << "DBase1() " << use_count() << "\n"; }
  ~DBase1() { std::cout << "~DBase1() " << use_count() << "\n"; }
  void print() { std::cout << "DBase1::print " << use_count() << "\n"; }
};

void test_ptr_policy() {
  auto p = make_ptr<Base1>();
  p->print();
  auto p1 = make_ptr<DBase1>();
  p1->print();
  p = p1;
  p->print();

  ptr<DBase1> p2;
  p2 = static_pointer_cast<DBase1>(p);
  // p2 = dynamic_pointer_cast<DBase1>(p);
  p2->print();

  ptr<Base1> p3 = make_ptr<DBase1>();
  p3->print();

  ptr<const DBase1> p4 = make_ptr<const DBase1>();
  auto p5 = const_pointer_cast<DBase1>(p4);
  p5->print();
}

void test_safe_ref() {
  auto p = make_ptr<Base>();
  p->print();
  auto p1 = make_ptr<DBase>();
  p1->print();
  p = p1;
  p->print();

  ptr<DBase> p2;
  // p2 = static_pointer_cast<DBase>(p);
  p2 = dynamic_pointer_cast<DBase>(p);
  p2->print();

  ptr<Base> p3 = make_ptr<DBase>();
  p3->print();

  ptr<const DBase> p4 = make_ptr<const DBase>();
  auto p5 = const_pointer_cast<DBase>(p4);
  p5->print();

  //
  std::cout << "-------\n";
  auto p6 = make_ptr<DBase>();
  auto p7 = static_pointer_cast<Base>(p6);
  auto p8 = static_pointer_cast<ref<I>>(p7);
  p8->test();
  auto p9 = interface_cast<I>(p6);
  p9->test();
  auto p10 = interface_cast<I>(p7);
  p10->test();
  auto p11 = interface_cast<I>(p8);
  p11->test();

  iptr<I, Base> ip12;
  ip12 = p7;
  ip12->test();
  iptr<I, DBase> ip13;
  ip13 = p6;
  ip13->test();

  // ip12 = interface_cast<I>(ip13);

  
  std::cout << "-------\n";
}

int main(int argc, char** argv) {
  // test_ptr_policy();
  test_safe_ref();

  return 0;
}

// int main(int argc, char** argv) {
// auto ri = make_ptr<Base>();
// auto rri = dynamic_pointer_cast<ref<I>>(ri);
// rri->test();

// iptr<I, Base> iiptr(ri);
// iiptr->test();

// auto iiiptr = interface_cast<I>(ri);
// iiiptr->test();

// ptr<Base> p = make_ptr<Base>();
// p->print();
// p->test();

// auto p1 = p;
// p1->print();

// auto p2 = std::move(p1);
// p2->print();

// (*p2).print();

// ptr<Base> p3;
// p3 = p2;
// p3->print();

// ptr<Base> p4;
// p4 = std::move(p3);
// p4->print();
// // p3->print();

// if (!p3) {
//   std::cout << "p3 null\n";
// }

// p4.get()->print();
// if (!p4.unique()) {
//   std::cout << "p4 unique false\n";
// }

// std::cout << "cur ptr " << p4.use_count() << "\n";

// if (p3 != p4) {
//   std::cout << "p3 != p4\n";
// }

// ptr<Base1> pp = make_ptr<Base1>();
// pp->print();

// if (p2 == p4) {
//   std::cout << "p2 == p4\n";
// }

// scope_ptr<Base> sp{p4, [](Base* b) {
//                      std::cout << "scope_ptr\n";
//                      b->print();
//                    }};
// sp.apply();

// ptr<DBase> p5 = make_ptr<DBase>();
// p5->print();
// p5->test();

// std::cout << "-------\n";
// ptr<Base> p6 = dynamic_pointer_cast<Base>(p5);
// p6->print();
// ptr<Base> p7 = dynamic_pointer_cast<Base>(std::move(p5));
// p7->print();
// // p5->print();

// auto p8 = make_ptr<const DBase>();
// ptr<DBase> p9 = const_pointer_cast<DBase>(p8);
// p9->print();
// std::cout << "-------\n";
// std::map<ptr<Base>, std::string> map1;
// map1[p7] = "hello";

// std::unordered_map<ptr<Base>, std::string> map2;
// map2[p7] = "world";

// std::cout << "map:" << map1[p7] << "\n";
// std::cout << "unordered_map:" << map2[p7] << "\n";

// auto p10 = make_ptr<DBase>();
// iptr<I, DBase> p11 = p10;

// p11->test();

// std::cout << "-------\n";

//   return 0;
// }