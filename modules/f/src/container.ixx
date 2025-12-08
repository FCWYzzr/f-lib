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
public:
    template<typename T, typename U>
    auto&& as(this U&& self)  {
        if (self._type == type_t::of<void>()) 
            throw std::bad_any_cast{};
        if (self._type != type_t::of<T>())
            throw std::bad_any_cast{};

        if constexpr (std::is_const_v<std::remove_reference_t<U>>)
            return *reinterpret_cast<const T*>(self._buffer.data());
        else
            return *reinterpret_cast<T*>(self._buffer.data());
    }

    template<typename T, typename ...Args>
    T& emplace(Args&& ... args) {
        clear();
        _type = type_t::of<T>();
        _buffer.resize(_type.size());
        const auto p = new (_buffer.data()) T {std::forward<Args>(args)...};
        return *p;
    }

    void clear() {
        _type.destroy(_buffer.data());
        _buffer.clear();
    }


    any& operator = (const any& other) {
        if (_type == other._type)
            _type.copy(_buffer.data(), other._buffer.data());
        else {
            clear();
            _type = other._type;
            _buffer.resize(_type.size());
            _type.make(_buffer.data(), other._buffer.data());
        }
        return *this;
    }

    any& operator = (any&& other) noexcept {
        _type.destroy(_buffer.data());
        _type = std::move(other._type);
        _buffer = std::move(other._buffer);
        return *this;
    }


    template<typename T, typename ...Args>
    explicit
    any(std::in_place_type_t<T>, Args&& ... args):
        _type{type_t::of<T>()}{
        emplace<T>(std::forward<Args>(args)...);
    }
    any() = delete;
    any(any&& other) noexcept:
        _buffer{std::move(other._buffer)},
        _type{std::move(other._type)}{}
    any(const any& other) :
        _buffer{other._buffer.size()},
        _type{other._type}{
        _type.make(_buffer.data(), other._buffer.data());
    }
    ~any() {
        clear();
    }



private:
    vector<std::byte>
        _buffer{};
    type_t
        _type;
};

template<typename T>
using stack = std::stack<T, deque<T>>;

}
