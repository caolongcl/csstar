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

struct ref_counter_unsafe_policy {
  using type = std::size_t;

  auto store(type &counter, std::size_t value, std::memory_order mo) noexcept
      -> void {
    counter = value;
  }

  auto load(const type &counter, std::memory_order mo) noexcept -> std::size_t {
    return counter;
  }

  auto inc(type &counter, std::memory_order mo) noexcept -> std::size_t {
    return ++counter;
  }

  auto dec(type &counter, std::memory_order mo) noexcept -> std::size_t {
    return --counter;
  }
};

struct ref_counter_safe_policy {
  using type = std::atomic<std::size_t>;

  auto store(type &counter, std::size_t value, std::memory_order mo) noexcept
      -> void {
    counter.store(value, mo);
  }

  auto load(const type &counter, std::memory_order mo) noexcept -> std::size_t {
    return counter.load(mo);
  }

  auto inc(type &counter, std::memory_order mo) noexcept -> std::size_t {
    return counter.fetch_add(1, mo) + 1;
  }

  auto dec(type &counter, std::memory_order mo) noexcept -> std::size_t {
    return counter.fetch_sub(1, mo) - 1;
  }
};
}  // namespace detail

namespace traits {
template <typename T, typename U = typename T::type>
concept CCounterPolicy =
    requires(T t, U u, std::size_t count, std::memory_order mo) {
      typename T::type;
      std::same_as<typename T::type, U>;

      t.store(u, count, mo);
      t.load(u, mo);
      t.inc(u, mo);
      t.dec(u, mo);
    };
}  // namespace traits

template <traits::CCounterPolicy T>
class ref_counter {
 private:
  mutable typename T::type _counter;

 public:
  ref_counter() noexcept : _counter{0} {}

  auto store(std::size_t value,
             std::memory_order mo = std::memory_order_relaxed) noexcept
      -> void {
    T{}.store(_counter, value, mo);
  }

  auto load(std::memory_order mo = std::memory_order_acquire) noexcept
      -> std::size_t {
    return T{}.load(_counter, mo);
  }

  auto inc(std::memory_order mo = std::memory_order_acq_rel) noexcept
      -> std::size_t {
    return T{}.inc(_counter, mo);
  }

  auto dec(std::memory_order mo = std::memory_order_acq_rel) noexcept
      -> std::size_t {
    return T{}.dec(_counter, mo);
  }

  ref_counter(const ref_counter &r) noexcept { store(r.load()); }

  ref_counter &operator=(const ref_counter &r) noexcept {
    if (this != &r) store(r.load());
    return *this;
  }

  ref_counter(ref_counter &&r) noexcept { store(r.load()); }

  ref_counter &operator=(ref_counter &&r) noexcept {
    if (this != &r) store(r.load());
    return *this;
  }
};

template <typename CounterPolicy>
class ptr_policy;

namespace traits {
template <typename T>
concept CPtrPolicy =
    std::derived_from<T, ptr_policy<detail::ref_counter_unsafe_policy>> ||
    std::derived_from<T, ptr_policy<detail::ref_counter_safe_policy>>;

struct RawPtrConstructNoRef {};
struct RawPtrConstructRef {};

}  // namespace traits

/// @brief 子类继承
template <typename CounterPolicy>
class ptr_policy {
 private:
  /// @brief object ref
  mutable ref_counter<CounterPolicy> _ref_counter;

  template <traits::CPtrPolicy U>
  friend class ptr;

 protected:
  ~ptr_policy() noexcept = default;
  constexpr ptr_policy() noexcept = default;
  ptr_policy(const ptr_policy &) noexcept = default;
  ptr_policy &operator=(const ptr_policy &) noexcept = default;
  ptr_policy(ptr_policy &&) noexcept = default;
  ptr_policy &operator=(ptr_policy &&) noexcept = default;

  auto use_count() -> std::size_t { return _ref_counter.load(); }
};

using PtrPolicy = ptr_policy<detail::ref_counter_safe_policy>;
using LocalPtrPolicy = ptr_policy<detail::ref_counter_unsafe_policy>;

template <traits::CPtrPolicy T>
class ptr final {
 public:
  using element_type = T;

  ptr() noexcept : ptr{nullptr, traits::RawPtrConstructNoRef{}} {}
  ptr(std::nullptr_t) noexcept : ptr{nullptr, traits::RawPtrConstructNoRef{}} {}
  explicit ptr(T *t, traits::RawPtrConstructNoRef) noexcept : _t{t} {}
  ptr(T *t, traits::RawPtrConstructRef) noexcept : _t{t} { add_ref(); }

  /// copy
  template <typename U>
    requires(traits::CPtrPolicy<U> && std::is_constructible_v<U *, T *>)
  ptr(const ptr<U> &rp) : _t{rp._t} {
    add_ref();
  }

  ptr(const ptr &rp) : _t{rp._t} { add_ref(); }

  template <typename U>
    requires(traits::CPtrPolicy<U> && std::is_constructible_v<U *, T *>)
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
  template <typename U>
    requires(traits::CPtrPolicy<U> && std::is_constructible_v<U *, T *>)
  ptr(ptr<U> &&rp) noexcept : _t{rp._t} {
    rp._t = nullptr;
  }

  ptr(ptr &&rp) noexcept : _t{rp._t} { rp._t = nullptr; }

  template <typename U>
    requires(traits::CPtrPolicy<U> && std::is_constructible_v<U *, T *>)
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

  auto reset(T *p) -> void { ptr(p).swap(*this); }

  auto detach() -> T * {
    T *p = _t;
    _t = nullptr;
    return p;
  }

  auto get() const noexcept -> T * { return _t; }

  auto operator*() const noexcept -> T & { return *_t; }

  auto operator->() const noexcept -> T * { return _t; }

  operator bool() const noexcept { return _t != nullptr; }

  auto use_count() const -> std::size_t {
    return _t ? _t->_ref_counter.load() : 0;
  }

  auto unique() const -> bool { return use_count() == 1; }

  template <typename... Args>
  static auto make_ptr(Args &&...args) -> ptr {
    return ptr(new T(std::forward<Args>(args)...));
  }

 private:
  T *_t;

  template <traits::CPtrPolicy U>
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

template <traits::CPtrPolicy T, typename... Args>
inline ptr<T> make_ptr(Args &&...args) {
  return ptr<T>::make_ptr(std::forward<Args>(args)...);
}

template <traits::CPtrPolicy T>
inline auto swap(ptr<T> &lp, ptr<T> &rp) noexcept -> void {
  lp.swap(rp);
}

template <traits::CPtrPolicy T1, traits::CPtrPolicy T2>
inline auto operator<(const ptr<T1> &lp, const ptr<T2> &rp) noexcept -> bool {
  return lp.get() < rp.get();
}

template <traits::CPtrPolicy T1, traits::CPtrPolicy T2>
inline auto operator==(const ptr<T1> &lp, const ptr<T2> &rp) noexcept -> bool {
  return lp.get() == rp.get();
}

template <traits::CPtrPolicy T>
inline auto operator==(const ptr<T> &lp, std::nullptr_t) noexcept -> bool {
  return lp.get() == nullptr;
}

template <traits::CPtrPolicy T>
inline auto operator==(std::nullptr_t, const ptr<T> &rp) noexcept -> bool {
  return nullptr == rp.get();
}

template <traits::CPtrPolicy T>
inline auto operator!=(const ptr<T> &lp, std::nullptr_t) noexcept -> bool {
  return !operator==(lp, nullptr);
}

template <traits::CPtrPolicy T>
inline auto operator!=(std::nullptr_t, const ptr<T> &rp) noexcept -> bool {
  return !operator==(nullptr, rp);
}

template <traits::CPtrPolicy T1, traits::CPtrPolicy T2>
inline auto operator!=(const ptr<T1> &lp, const ptr<T2> &rp) noexcept -> bool {
  return !operator==(lp, rp);
}

template <traits::CPtrPolicy T, traits::CPtrPolicy U>
ptr<T> static_pointer_cast(const ptr<U> &p) {
  return {static_cast<T *>(p.get()), traits::RawPtrConstructRef{}};
}

template <traits::CPtrPolicy T, traits::CPtrPolicy U>
  requires(std::is_convertible_v<std::remove_cv_t<U> *, std::remove_cv_t<T> *>)
ptr<T> const_pointer_cast(const ptr<U> &p) {
  return {const_cast<T *>(p.get()), traits::RawPtrConstructRef{}};
}

template <traits::CPtrPolicy T, traits::CPtrPolicy U>
  requires(std::is_convertible_v<U *, T *>)
ptr<T> dynamic_pointer_cast(const ptr<U> &p) {
  return {dynamic_cast<T *>(p.get()), traits::RawPtrConstructRef{}};
}

template <traits::CPtrPolicy T, traits::CPtrPolicy U>
ptr<T> reinterpret_pointer_cast(const ptr<U> &p) {
  return {reinterpret_cast<T *>(p.get()), traits::RawPtrConstructRef{}};
}

template <traits::CPtrPolicy T, traits::CPtrPolicy U>
ptr<T> static_pointer_cast(ptr<U> &&p) noexcept {
  return ptr<T>(static_cast<T *>(p.detach()), traits::RawPtrConstructNoRef{});
}

template <traits::CPtrPolicy T, traits::CPtrPolicy U>
  requires(std::is_convertible_v<U *, T *>)
ptr<T> const_pointer_cast(ptr<U> &&p) noexcept {
  return ptr<T>(const_cast<T *>(p.detach()), traits::RawPtrConstructNoRef{});
}

template <traits::CPtrPolicy T, traits::CPtrPolicy U>
  requires(std::is_convertible_v<U *, T *>)
ptr<T> dynamic_pointer_cast(ptr<U> &&p) noexcept {
  T *p2 = dynamic_cast<T *>(p.get());
  ptr<T> r(p2, traits::RawPtrConstructNoRef{});
  if (p2) p.detach();
  return r;
}

template <traits::CPtrPolicy T, traits::CPtrPolicy U>
ptr<T> reinterpret_pointer_cast(ptr<U> &&p) noexcept {
  T *p2 = reinterpret_cast<T *>(p.get());
  ptr<T> r(p2, traits::RawPtrConstructNoRef{});
  if (p2) p.detach();
  return r;
}

template <traits::CPtrPolicy T>
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

}  // namespace dsg::sp

namespace std {
template <dsg::sp::traits::CPtrPolicy T>
class hash<dsg::sp::ptr<T>> {
 public:
  size_t operator()(const dsg::sp::ptr<T> &p) const {
    return std::hash<T *>()(p.get());
  }
};

}  // namespace std

namespace dsg {
using namespace sp;
}  // namespace dsg
