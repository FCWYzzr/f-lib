//
// Created by MuXin on 2025/12/17.
//

export module f:utilities;
import std;
import :exception;

export namespace f {

template<typename T, typename E=void>
class equal_or_throw {
public:
    equal_or_throw(T&& expected, E&& on_unexpected) noexcept:
        expected{std::move(expected)},
        on_unexpected{std::move(on_unexpected)}
    {}

    auto operator >> (const T& check) const noexcept(false) -> const equal_or_throw& {
        if (expected == check)
            return *this;
        throw on_unexpected;
    }
private:
    T expected;
    E on_unexpected;
};

template<typename T, typename E=void>
class not_equal_or_throw {
public:
    not_equal_or_throw(T&& unexpected, E&& on_unexpected) noexcept:
        unexpected{std::move(unexpected)},
        on_unexpected{std::move(on_unexpected)}
    {}

    auto operator >> (const T& check) const noexcept(false) -> const not_equal_or_throw& {
        if (unexpected != check)
            return *this;
        throw on_unexpected;
    }
private:
    T unexpected;
    E on_unexpected;
};

template<typename T>
class equal_or_throw<T, void> {
public:
    explicit
    equal_or_throw(T&& expected) noexcept:
        expected{std::move(expected)}
    {}

    auto operator >> (const T& check) const noexcept(false) -> const equal_or_throw& {
        if (expected == check)
            return *this;
        throw exception("expect {}, but get {}", expected, check);
    }
private:
    T expected;
};

template<typename T>
class not_equal_or_throw<T, void> {
public:
    explicit
    not_equal_or_throw(T&& unexpected) noexcept:
        unexpected{std::move(unexpected)}
    {}

    auto operator >> (const T& check) const noexcept(false) -> const not_equal_or_throw& {
        if (unexpected != check)
            return *this;
        throw exception("get unexpected {}", unexpected);
    }
private:
    T unexpected;
};

}