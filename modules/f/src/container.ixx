//
// Created by MuXin on 2025/11/26.
//

export module f:container;

import :memory;
import :rtt;
import :exception;

export namespace f {


struct string_hasher {
    using is_transparent = void;
    template<typename Str>
    requires std::convertible_to<const Str, std::string_view>
    size_t operator () (const Str& s) const noexcept {
        return std::hash<std::string_view>{}(s);
    }
};

class string_pool {
public:
    std::string_view of(const std::string_view sv) noexcept {
        auto r_lock = std::shared_lock{_mutex};
        if (const auto it = _keys.find(sv); it != _keys.end())
            return *it;
        r_lock.unlock();

        auto w_lock = std::unique_lock{_mutex};
        const auto [it, _] = _keys.emplace(sv);
        return *it;
    }

private:
    mutable std::shared_mutex
        _mutex;
    unordered_set<std::string, string_hasher, std::equal_to<>>
        _keys;
};

class any {
    using maker_fun = void(std::byte*);
    using collector_fun = void(std::byte*);
    using copy_fun = void(std::byte*, const std::byte*);
    using move_fun = void(std::byte*, std::byte*);
public:
    template<typename T, typename U>
    auto&& as(this U&& self)  {
        if (_idx != type_index{typeid(T)})
            throw std::bad_any_cast{};

        if constexpr (std::is_const_v<std::remove_reference_t<U>>)
            return *reinterpret_cast<const T*>(self._buffer.data());
        else
            return *reinterpret_cast<T*>(self._buffer.data());
    }

    template<typename T, typename ...Args>
    T& emplace(Args&& ... args) {
        clear();
        _buffer.resize(sizeof(T));
        const auto p = new (_buffer.data()) T {std::forward<Args>(args)...};

        if constexpr (std::constructible_from<T>)
            _maker = [](std::byte* ptr) {
                new (ptr) T {};
            };
        _collector = [](std::byte* ptr) {
            ptr->~T();
        };
        if constexpr (std::copy_constructible<T>)
            _copier = [](std::byte* dst, const std::byte* src) {
                new (dst) T { reinterpret_cast<const T&>(*src)};
            };
        if constexpr (std::move_constructible<T>)
            _mover = [](std::byte* dst, std::byte* src) {
                new (dst) T { std::move(reinterpret_cast<T&>(*src)) };
            };
        return *p;
    }

    void clear() {
        // ReSharper disable CppDFAConstantConditions
        // ReSharper disable CppDFAUnreachableCode
        if (_collector) {
            _collector(_buffer.data());
            _buffer.clear();
            _collector = nullptr;
        }
        _maker = nullptr;
        _copier = nullptr;
        _mover = nullptr;
        _idx = type_index{typeid(void)};
        // ReSharper restore CppDFAConstantConditions
        // ReSharper restore CppDFAUnreachableCode
    }


    template<typename T, typename ...Args>
    explicit
    any(std::in_place_type_t<T>, Args&& ... args):
        _idx{typeid(T)}{
        emplace<T>(std::forward<Args>(args)...);
    }

    any() noexcept: _idx{typeid(void)} {}
    // ReSharper disable once CppSpecialFunctionWithoutNoexceptSpecification
    any(any&& other): // NOLINT(*-noexcept-move-constructor)
        _buffer{other._buffer.size()},
        _idx{other._idx},
        _collector{other._collector},
        _copier{other._copier},
        _mover{other._mover}{
        if (!other._mover)
            throw exception("type not movable! (holds {})", _idx.name());
        _mover(_buffer.data(), other._buffer.data());
    }
    any(const any& other) :
        _buffer{other._buffer.size()},
        _idx{other._idx},
        _collector{other._collector},
        _copier{other._copier},
        _mover{other._mover}{
        if (!other._copier)
            throw exception("type not copyable! (holds {})", _idx.name());
        _copier(_buffer.data(), other._buffer.data());
    }

    ~any() {
        clear();
    }


private:
    vector<std::byte>
        _buffer{};
    std::type_index
        _idx;
    maker_fun*
        _maker{};
    collector_fun*
        _collector{};
    copy_fun*
        _copier{};
    move_fun*
        _mover{};
};

template<typename T>
using stack = std::stack<T, deque<T>>;

}
