module;
#include <genex/detail/macros.hpp>

export module genex.detail.iterator_ops;
import std;

export namespace genex::detail {
  /**
   * Generates an iterator's operators from the primitives it declares, so each operator exists only if its primitive
   * does. @c deref and @c next are required, and any of the others can be left out:
   *
   *  - @c deref() @c const gives @c *.
   *  - @c next() gives @c ++.
   *  - @c prev() gives @c --.
   *  - @c advance_by(n) gives @c +=, @c -=, @c +, @c - and @c [].
   *  - @c distance_to(that) @c const gives @c a-b and the ordering operators, where it returns @c that-this.
   *
   * @c Derived is only there to give each iterator its own base. An iterator's first member is often another iterator,
   * and two subobjects of one empty type can't share an address, which would add padding to every nested iterator.
   */
  template <typename Derived>
  struct iterator_ops {
    template <typename Self>
    GENEX_INLINE constexpr auto operator*(this const Self &self) -> decltype(auto) {
      return self.deref();
    }

    template <typename Self>
    GENEX_INLINE constexpr auto operator++(this Self &self) -> Self& {
      self.next();
      return self;
    }

    template <typename Self>
    GENEX_INLINE constexpr auto operator++(this Self &self, int) -> Self {
      auto temp = self;
      self.next();
      return temp;
    }

    template <typename Self>
      requires requires(Self &s) { s.prev(); }
    GENEX_INLINE constexpr auto operator--(this Self &self) -> Self& {
      self.prev();
      return self;
    }

    template <typename Self>
      requires requires(Self &s) { s.prev(); }
    GENEX_INLINE constexpr auto operator--(this Self &self, int) -> Self {
      auto temp = self;
      self.prev();
      return temp;
    }

    template <typename Self>
      requires requires(Self &s, Self::difference_type n) { s.advance_by(n); }
    GENEX_INLINE constexpr auto operator+=(this Self &self, Self::difference_type n) -> Self& {
      self.advance_by(n);
      return self;
    }

    template <typename Self>
      requires requires(Self &s, Self::difference_type n) { s.advance_by(n); }
    GENEX_INLINE constexpr auto operator-=(this Self &self, Self::difference_type n) -> Self& {
      self.advance_by(-n);
      return self;
    }

    template <typename Self>
      requires requires(Self &s, Self::difference_type n) { s.advance_by(n); }
    GENEX_INLINE constexpr auto operator[](this const Self &self, Self::difference_type n) -> decltype(auto) {
      auto temp = self;
      temp.advance_by(n);
      return temp.deref();
    }

    // The friends deduce their type, as Derived is still incomplete while this class is being instantiated.
    template <std::same_as<Derived> Self>
      requires requires(Self &s, Self::difference_type n) { s.advance_by(n); }
    GENEX_INLINE friend constexpr auto operator+(const Self &self, Self::difference_type n) -> Self {
      auto temp = self;
      temp.advance_by(n);
      return temp;
    }

    template <std::same_as<Derived> Self>
      requires requires(Self &s, Self::difference_type n) { s.advance_by(n); }
    GENEX_INLINE friend constexpr auto operator+(Self::difference_type n, const Self &self) -> Self {
      return self + n;
    }

    template <std::same_as<Derived> Self>
      requires requires(Self &s, Self::difference_type n) { s.advance_by(n); }
    GENEX_INLINE friend constexpr auto operator-(const Self &self, Self::difference_type n) -> Self {
      auto temp = self;
      temp.advance_by(-n);
      return temp;
    }

    template <std::same_as<Derived> Self>
      requires requires(const Self &a, const Self &b) { a.distance_to(b); }
    GENEX_INLINE friend constexpr auto operator-(const Self &self, const Self &that) -> Self::difference_type {
      return that.distance_to(self);
    }

    template <std::same_as<Derived> Self>
      requires requires(const Self &a, const Self &b) { a.distance_to(b); }
    GENEX_INLINE friend constexpr auto operator<=>(const Self &self, const Self &that) {
      return 0 <=> self.distance_to(that);
    }
  };

  /**
   * Primitives forwarded to a member @c it, for an iterator with one position per position in @c it. A class replaces
   * any of them by declaring its own, which hides the one here. An iterator that skips, groups or reverses elements
   * must use @c iterator_ops directly, as these would count its positions wrongly.
   */
  template <typename Derived>
  struct adaptor_ops : iterator_ops<Derived> {
    template <typename Self>
    GENEX_INLINE constexpr auto deref(this const Self &self) -> decltype(auto) {
      return *self.it;
    }

    template <typename Self>
    GENEX_INLINE constexpr auto next(this Self &self) -> void {
      ++self.it;
    }

    template <typename Self>
      requires std::bidirectional_iterator<decltype(Self::it)>
    GENEX_INLINE constexpr auto prev(this Self &self) -> void {
      --self.it;
    }

    template <typename Self>
      requires std::random_access_iterator<decltype(Self::it)>
    GENEX_INLINE constexpr auto advance_by(this Self &self, Self::difference_type n) -> void {
      self.it += n;
    }

    template <typename Self>
      requires std::random_access_iterator<decltype(Self::it)>
    GENEX_INLINE constexpr auto distance_to(this const Self &self, const Self &that) -> Self::difference_type {
      return that.it - self.it;
    }

    template <std::same_as<Derived> Self>
    GENEX_INLINE friend constexpr auto operator==(const Self &self, const Self &that) -> bool {
      return self.it == that.it;
    }
  };
}
