/**
 * @file ptr.hpp
 * @author long.cao (long.cao@whatever.com)
 * @brief
 * @version 0.1
 * @date 2024-03-27
 *
 * @copyright Copyright (c) 2024
 *
 * 智能指针，强引用计数
 * 1. 继承 ref_policy ，然后使用 ptr 引用对象
 * 2. 继承 interface_wrapper<Interface> 封装接口，使用 ptr 和 interface_ptr<I>
 * 3. 非线程安全版本 unsafe_ref_policy、unsafe_interface_wrapper, unsafe_interface_ptr
 */
#pragma once

#include <cstddef>
#include <atomic>
#include <type_traits>
#include <concepts>
#include <functional>

#ifdef DSG_SP_SUPPORT_UNIQUE_PTR
#include <memory>
#endif

#include <cassert>
#define DSG_SP_PTR_ASSERT(e) assert(e)

namespace dsg::sp {
namespace detail {

using val_t = std::size_t;
using mo_t = std::memory_order;

struct ref_counter_unsafe {
  using type = val_t;

  static auto store(type &c, val_t v, mo_t mo) -> void { c = v; }

  static auto load(const type &c, mo_t mo) noexcept -> val_t { return c; }

  static auto inc(type &c, mo_t mo) noexcept -> val_t { return ++c; }

  static auto dec(type &c, mo_t mo) noexcept -> val_t { return --c; }
};

struct ref_counter_safe {
  using type = std::atomic<val_t>;

  static auto store(type &c, val_t v, mo_t mo) -> void { c.store(v, mo); }

  static auto load(const type &c, mo_t mo) -> val_t { return c.load(mo); }

  static auto inc(type &c, mo_t mo) -> val_t { return c.fetch_add(1, mo) + 1; }

  static auto dec(type &c, mo_t mo) -> val_t { return c.fetch_sub(1, mo) - 1; }
};
}  // namespace detail

namespace traits {
template <typename T, typename U = typename T::type>
concept CCounter = requires(T t, U u, detail::val_t v, detail::mo_t mo) {
  typename T::type;
  requires std::same_as<typename T::type, U>;

  T::store(u, v, mo);
  T::load(u, mo);
  T::inc(u, mo);
  T::dec(u, mo);
};
}  // namespace traits

template <traits::CCounter T>
class ref_counter {
 private:
  mutable typename T::type _counter;

  using mo_t = detail::mo_t;
  using val_t = detail::val_t;

 public:
  ref_counter() noexcept : _counter{0} {}

  auto store(val_t v, mo_t mo = std::memory_order_relaxed) -> void { T::store(_counter, v, mo); }

  auto load(mo_t mo = std::memory_order_acquire) -> val_t { return T::load(_counter, mo); }

  auto inc(mo_t mo = std::memory_order_acq_rel) -> val_t { return T::inc(_counter, mo); }

  auto dec(mo_t mo = std::memory_order_acq_rel) -> val_t { return T::dec(_counter, mo); }

  ref_counter(const ref_counter &r) { store(r.load()); }

  ref_counter &operator=(const ref_counter &r) {
    if (this != &r) store(r.load());
    return *this;
  }

  ref_counter(ref_counter &&r) noexcept { store(r.load()); }

  ref_counter &operator=(ref_counter &&r) noexcept {
    if (this != &r) store(r.load());
    return *this;
  }
};

template <traits::CCounter Counter>
class ptr_policy;

namespace traits {
template <typename T>
concept CPtrT = std::derived_from<T, ptr_policy<detail::ref_counter_safe>> || std::derived_from<T, ptr_policy<detail::ref_counter_unsafe>>;

struct RawPtrConstructNoRef {};
struct RawPtrConstructRef {};

}  // namespace traits

template <traits::CCounter Counter>
class ptr_policy {
 private:
  /// @brief object ref
  mutable ref_counter<Counter> _ref_counter;

  template <traits::CPtrT U>
  friend class ptr;

 protected:
  ~ptr_policy() noexcept = default;
  constexpr ptr_policy() noexcept = default;
  ptr_policy(const ptr_policy &) noexcept = default;
  ptr_policy &operator=(const ptr_policy &) noexcept = default;
  ptr_policy(ptr_policy &&) noexcept = default;
  ptr_policy &operator=(ptr_policy &&) noexcept = default;

  auto use_count() -> decltype(_ref_counter.load()) { return _ref_counter.load(); }
};

///////////////////////////////////////////////////////////
/// 直接继承
using ref_policy = ptr_policy<detail::ref_counter_safe>;
using unsafe_ref_policy = ptr_policy<detail::ref_counter_unsafe>;
///////////////////////////////////////////////////////////

namespace traits {
template <typename T>
concept CPrtPolicyT = std::is_same_v<T, ref_policy> || std::is_same_v<T, unsafe_ref_policy>;
}

namespace detail {
template <typename Interface, traits::CPrtPolicyT PtrPolicyT>
struct __interface_wrapper : public Interface, public PtrPolicyT {
  using interface_type = Interface;
  using ptr_policy_type = PtrPolicyT;
};
}  // namespace detail

///////////////////////////////////////////////////////////
/// 处理接口
template <typename Interface>
using interface_wrapper = detail::__interface_wrapper<Interface, ref_policy>;

template <typename Interface>
using unsafe_interface_wrapper = detail::__interface_wrapper<Interface, unsafe_ref_policy>;
///////////////////////////////////////////////////////////

/// @brief ptr
template <traits::CPtrT T>
class ptr final {
 public:
  using element_type = T;
  using ptr_policy_type = std::conditional_t<std::derived_from<T, ref_policy>, ref_policy, unsafe_ref_policy>;

  ptr() noexcept : ptr{nullptr, traits::RawPtrConstructNoRef{}} {}
  ptr(std::nullptr_t) noexcept : ptr{nullptr, traits::RawPtrConstructNoRef{}} {}
  explicit ptr(T *t, traits::RawPtrConstructNoRef) noexcept : _t{t} {}
  ptr(T *t, traits::RawPtrConstructRef) noexcept : _t{t} { add_ref(); }

  /// copy
  template <traits::CPtrT U>
  requires std::is_convertible_v<U *, T *>
  ptr(const ptr<U> &rp) : _t{rp._t} {
    add_ref();
  }

  ptr(const ptr &rp) : _t{rp._t} { add_ref(); }

  template <traits::CPtrT U>
  requires std::is_convertible_v<U *, T *>
  ptr &operator=(const ptr<U> &rp) {
    ptr(rp).swap(*this);
    return *this;
  }

  ptr &operator=(const ptr &rp) { return operator= <T>(rp); }

  ptr &operator=(T *p) {
    ptr(p).swap(*this);
    return *this;
  }

  /// move
  template <traits::CPtrT U>
  requires std::is_convertible_v<U *, T *>
  ptr(ptr<U> &&rp) noexcept : _t{rp._t} {
    rp._t = nullptr;
  }

  ptr(ptr &&rp) noexcept : _t{rp._t} { rp._t = nullptr; }

  template <traits::CPtrT U>
  requires std::is_convertible_v<U *, T *>
  ptr &operator=(ptr<U> &&rp) noexcept {
    ptr(std::move(rp)).swap(*this);
    return *this;
  }

  ptr &operator=(ptr &&rp) noexcept { return operator= <T>(std::move(rp)); }

  ~ptr() { dec_ref(); }

#ifdef DSG_SP_SUPPORT_UNIQUE_PTR
  ptr(std::unique_ptr<T> ut) noexcept : ptr{ut.release()} {}
#endif

  auto swap(ptr &rp) noexcept -> void { std::swap(_t, rp._t); }

  auto reset() -> void { ptr().swap(*this); }

  auto reset(T *p) -> void { ptr{p, traits::RawPtrConstructRef{}}.swap(*this); }

  auto detach() -> T * {
    T *p = _t;
    _t = nullptr;
    return p;
  }

  auto get() const noexcept -> T * { return _t; }

  auto operator*() const noexcept -> T & { return *_t; }

  auto operator->() const noexcept -> T * { return _t; }

  operator bool() const noexcept { return _t != nullptr; }

  auto use_count() const -> detail::val_t { return _t ? _t->_ref_counter.load() : 0; }

  auto unique() const -> bool { return use_count() == 1; }

  template <typename... Args>
  static auto make_ptr(Args &&...args) -> ptr {
    return ptr(new T(std::forward<Args>(args)...));
  }

 private:
  T *_t;

  template <traits::CPtrT U>
  friend class ptr;

  /// @brief for make_ptr
  /// @param t
  explicit ptr(T *t) noexcept : ptr{t, traits::RawPtrConstructNoRef{}} {
    DSG_SP_PTR_ASSERT(!_t || (_t->_ref_counter.load() == 0));
    if (_t) {
      _t->_ref_counter.store(1);
    }
  }

  auto add_ref() -> void {
    if (_t) {
      auto new_ref_count = _t->_ref_counter.inc();
      DSG_SP_PTR_ASSERT(new_ref_count != 1);
    }
  }

  auto dec_ref() -> void {
    if (_t && _t->_ref_counter.dec() == 0) {
      delete _t;
      _t = nullptr;
    }
  }
};

template <traits::CPtrT T, typename... Args>
inline ptr<T> make_ptr(Args &&...args) {
  return ptr<T>::make_ptr(std::forward<Args>(args)...);
}

template <traits::CPtrT T>
inline auto swap(ptr<T> &lp, ptr<T> &rp) noexcept -> void {
  lp.swap(rp);
}

template <traits::CPtrT T1, traits::CPtrT T2>
inline auto operator<(const ptr<T1> &lp, const ptr<T2> &rp) noexcept -> bool {
  return lp.get() < rp.get();
}

template <traits::CPtrT T1, traits::CPtrT T2>
inline auto operator==(const ptr<T1> &lp, const ptr<T2> &rp) noexcept -> bool {
  return lp.get() == rp.get();
}

template <traits::CPtrT T>
inline auto operator==(const ptr<T> &lp, std::nullptr_t) noexcept -> bool {
  return lp.get() == nullptr;
}

template <traits::CPtrT T>
inline auto operator==(std::nullptr_t, const ptr<T> &rp) noexcept -> bool {
  return nullptr == rp.get();
}

template <traits::CPtrT T>
inline auto operator!=(const ptr<T> &lp, std::nullptr_t) noexcept -> bool {
  return !operator==(lp, nullptr);
}

template <traits::CPtrT T>
inline auto operator!=(std::nullptr_t, const ptr<T> &rp) noexcept -> bool {
  return !operator==(nullptr, rp);
}

template <traits::CPtrT T1, traits::CPtrT T2>
inline auto operator!=(const ptr<T1> &lp, const ptr<T2> &rp) noexcept -> bool {
  return !operator==(lp, rp);
}

template <traits::CPtrT T, traits::CPtrT U>
ptr<T> static_pointer_cast(const ptr<U> &p) {
  return {static_cast<T *>(p.get()), traits::RawPtrConstructRef{}};
}

template <traits::CPtrT T, traits::CPtrT U>
requires std::is_convertible_v<std::remove_cv_t<T *>, std::remove_cv_t<U *>>
ptr<T> const_pointer_cast(const ptr<U> &p) {
  return {const_cast<T *>(p.get()), traits::RawPtrConstructRef{}};
}

template <traits::CPtrT T, traits::CPtrT U>
requires(std::is_base_of_v<U, T> && std::is_polymorphic_v<U>)
ptr<T> dynamic_pointer_cast(const ptr<U> &p) {
  // return {dynamic_cast<T *>(p.get()), traits::RawPtrConstructRef{}};
  return {static_cast<T *>(p.get()), traits::RawPtrConstructRef{}};
}

template <traits::CPtrT T, traits::CPtrT U>
ptr<T> reinterpret_pointer_cast(const ptr<U> &p) {
  return {reinterpret_cast<T *>(p.get()), traits::RawPtrConstructRef{}};
}

template <traits::CPtrT T, traits::CPtrT U>
ptr<T> static_pointer_cast(ptr<U> &&p) noexcept {
  return ptr<T>(static_cast<T *>(p.detach()), traits::RawPtrConstructNoRef{});
}

template <traits::CPtrT T, traits::CPtrT U>
requires std::is_convertible_v<std::remove_cv_t<T *>, std::remove_cv_t<U *>>
ptr<T> const_pointer_cast(ptr<U> &&p) noexcept {
  return ptr<T>(const_cast<T *>(p.detach()), traits::RawPtrConstructNoRef{});
}

template <traits::CPtrT T, traits::CPtrT U>
requires(std::is_base_of_v<U, T> && std::is_polymorphic_v<U>)
ptr<T> dynamic_pointer_cast(ptr<U> &&p) noexcept {
  // T *p2 = dynamic_cast<T *>(p.get());
  T *p2 = static_cast<T *>(p.get());
  ptr<T> r(p2, traits::RawPtrConstructNoRef{});
  if (p2) p.detach();
  return r;
}

template <traits::CPtrT T, traits::CPtrT U>
ptr<T> reinterpret_pointer_cast(ptr<U> &&p) noexcept {
  T *p2 = reinterpret_cast<T *>(p.get());
  ptr<T> r(p2, traits::RawPtrConstructNoRef{});
  if (p2) p.detach();
  return r;
}

template <traits::CPtrT T>
class scope_ptr {
 public:
  using func = std::function<void(T *)>;

  scope_ptr(ptr<T> p, func f) : _owner{p}, _func{f} {}

  auto apply() -> void {
    if (_owner && _func) _func(_owner.get());
  }

 private:
  ptr<T> _owner;
  func _func;
};

namespace detail {
template <typename Interface, traits::CPrtPolicyT PtrPolicyT = ref_policy>
class interface_ptr final {
 public:
  interface_ptr() = default;
  ~interface_ptr() = default;
  interface_ptr(const interface_ptr &) noexcept = default;
  interface_ptr &operator=(const interface_ptr &) noexcept = default;
  interface_ptr(interface_ptr &&) noexcept = default;
  interface_ptr &operator=(interface_ptr &&) noexcept = default;

  template <traits::CPtrT T>
  requires std::is_same_v<typename ptr<T>::ptr_policy_type, PtrPolicyT>
  interface_ptr(const ptr<T> &p) : _p{p} {}

  template <traits::CPtrT T>
  requires std::is_same_v<typename ptr<T>::ptr_policy_type, PtrPolicyT>
  interface_ptr &operator=(const ptr<T> &p) {
    _p = p;
    return *this;
  }

  template <traits::CPtrT T>
  requires std::is_same_v<typename ptr<T>::ptr_policy_type, PtrPolicyT>
  interface_ptr(ptr<T> &&p) noexcept : _p{std::move(p)} {
    p.reset();
  }

  template <traits::CPtrT T>
  requires std::is_same_v<typename ptr<T>::ptr_policy_type, PtrPolicyT>
  interface_ptr &operator=(ptr<T> &&p) noexcept {
    _p = std::move(p);
    p.reset();
    return *this;
  }

  auto get() const noexcept -> Interface * { return static_cast<Interface *>(_p.get()); }

  auto operator*() const noexcept -> Interface & { return *static_cast<Interface *>(_p.get()); }

  auto operator->() const noexcept -> Interface * { return static_cast<Interface *>(_p.get()); }

  operator bool() const noexcept { return _p; }

  /// @brief for test
  /// @return
  auto use_count() -> detail::val_t { return _p.use_count(); }

 private:
  ptr<detail::__interface_wrapper<Interface, PtrPolicyT>> _p;
};
}  // namespace detail

template <typename Interface, traits::CPtrT T>
requires std::is_base_of_v<detail::__interface_wrapper<Interface, typename ptr<T>::ptr_policy_type>, T>
detail::interface_ptr<Interface, typename ptr<T>::ptr_policy_type> interface_cast(const ptr<T> &p) {
  return p;
}

template <typename Interface, traits::CPtrT T>
requires std::is_base_of_v<detail::__interface_wrapper<Interface, typename ptr<T>::ptr_policy_type>, T>
detail::interface_ptr<Interface, typename ptr<T>::ptr_policy_type> interface_cast(ptr<T> &&p) {
  return std::move(p);
}

template <typename Interface>
using interface_ptr = detail::interface_ptr<Interface, ref_policy>;

template <typename Interface>
using unsafe_interface_ptr = detail::interface_ptr<Interface, unsafe_ref_policy>;

}  // namespace dsg::sp

namespace std {
template <dsg::sp::traits::CPtrT T>
class hash<dsg::sp::ptr<T>> {
 public:
  size_t operator()(const dsg::sp::ptr<T> &p) const { return std::hash<T *>()(p.get()); }
};

template <typename Interface>
class hash<dsg::sp::interface_ptr<Interface>> {
 public:
  size_t operator()(const dsg::sp::interface_ptr<Interface> &p) const { return std::hash<Interface *>()(p.get()); }
};

template <typename Interface>
class hash<dsg::sp::unsafe_interface_ptr<Interface>> {
 public:
  size_t operator()(const dsg::sp::unsafe_interface_ptr<Interface> &p) const { return std::hash<Interface *>()(p.get()); }
};

}  // namespace std

namespace dsg {
using namespace sp;
}  // namespace dsg

// namespace h {
// template <typename Interface>
// class interface_ptr final {
//  public:
//   interface_ptr() = default;
//   ~interface_ptr() = default;
//   interface_ptr(const interface_ptr &) noexcept = default;
//   interface_ptr &operator=(const interface_ptr &) noexcept = default;
//   interface_ptr(interface_ptr &&) noexcept = default;
//   interface_ptr &operator=(interface_ptr &&) noexcept = default;

//   template <traits::CPtrT T>
//   requires std::is_same_v<typename ptr<T>::ptr_policy_type, ref_policy>
//   interface_ptr(const ptr<T> &p, ref_policy *= nullptr) : _p._safe{p} {}

//   template <traits::CPtrT T>
//   requires std::is_same_v<typename ptr<T>::ptr_policy_type,
//   unsafe_ref_policy> interface_ptr(const ptr<T> &p, unsafe_ref_policy *=
//   nullptr)
//       : _p._unsafe{p} {}

//   template <traits::CPtrT T>
//   requires std::is_same_v<typename ptr<T>::ptr_policy_type, ref_policy>
//   interface_ptr &operator=(const ptr<T> &p, ref_policy *= nullptr) {
//     _p._safe = p;
//     return *this;
//   }

//   template <traits::CPtrT T>
//   requires std::is_same_v<typename ptr<T>::ptr_policy_type,
//   unsafe_ref_policy> interface_ptr &operator=(const ptr<T> &p,
//   unsafe_ref_policy *= nullptr) {
//     _p._unsafe = p;
//     return *this;
//   }

//   template <traits::CPtrT T>
//   requires std::is_same_v<typename ptr<T>::ptr_policy_type, ref_policy>
//   interface_ptr(ptr<T> &&p, ref_policy *= nullptr)) noexcept :
//   _p._safe{std::move(p)} {
//     p.reset();
//   }

//   template <traits::CPtrT T>
//   requires std::is_same_v<typename ptr<T>::ptr_policy_type,
//   unsafe_ref_policy> interface_ptr(ptr<T> &&p, unsafe_ref_policy *= nullptr)
//   noexcept
//       : _p._unsafe{std::move(p)} {
//     p.reset();
//   }

//   template <traits::CPtrT T>
//   requires std::is_same_v<typename ptr<T>::ptr_policy_type, ref_policy>
//   interface_ptr &operator=(ptr<T> &&p, ref_policy *= nullptr) noexcept {
//     _p._safe = std::move(p);
//     p.reset();
//     return *this;
//   }

//   template <traits::CPtrT T>
//   requires std::is_same_v<typename ptr<T>::ptr_policy_type,
//   unsafe_ref_policy> interface_ptr &operator=(ptr<T> &&p, unsafe_ref_policy
//   *= nullptr) noexcept {
//     _p._unsafe = std::move(p);
//     p.reset();
//     return *this;
//   }

//   auto get() const noexcept -> Interface * {
//     return static_cast<Interface *>(_p.get());
//   }

//   auto operator*() const noexcept -> Interface & {
//     return *static_cast<Interface *>(_p.get());
//   }

//   auto operator->() const noexcept -> Interface * {
//     return static_cast<Interface *>(_p.get());
//   }

//   operator bool() const noexcept { return _p; }

//  private:
//   union {
//     ptr<interface_wrapper<Interface>> _safe;
//     ptr<unsafe_interface_wrapper<Interface>> _unsafe;
//   } _p;
// };

// template <typename Interface, traits::CPtrT T>
// requires std::is_base_of_v<
//     detail::__interface_wrapper<Interface, typename ptr<T>::ptr_policy_type>,
//     T>
// interface_ptr<Interface> interface_cast(const ptr<T> &p) {
//   return p;
// }

// template <typename Interface, traits::CPtrT T>
// requires std::is_base_of_v<
//     detail::__interface_wrapper<Interface, typename ptr<T>::ptr_policy_type>,
//     T>
// interface_ptr<Interface> interface_cast(ptr<T> &&p) {
//   return std::move(p);
// }
// }  // namespace h