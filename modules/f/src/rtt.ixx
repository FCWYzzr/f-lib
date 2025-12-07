//
// Created by MuXin on 2025/11/23.
//

export module f:rtt;
export import std;

export namespace f {

struct method_not_implemented: std::runtime_error {
    explicit
    method_not_implemented(const std::string_view name):
        runtime_error{std::format("method not implemented: {}", name)},
        method_name{name}
    {}

    std::string
        method_name;
};

/// 存储一个类型在运行时所有*常见*的行为（如果有）
/// 包括：
/// - 默认构造
/// - 复制构造
/// - 移动构造
/// - 析构
/// - 复制赋值
/// - 移动赋值
class type_t {
    using default_constructor_t = auto (std::byte*) -> void;
    using destructor_t          = auto (std::byte*) -> void;
    using copier_t              = auto (std::byte*, const std::byte*) -> void;
    using mover_t               = auto (std::byte*, std::byte*) -> void;

public:
    void make(std::byte* obj) const {
        if (!_default_constructor)
            throw method_not_implemented{"Default Constructor"};
        _default_constructor(obj);
    }
    void make(std::byte* obj, const std::byte* prototype) const {
        if (!_copy_constructor)
            throw method_not_implemented{"Copy Constructor"};
        _copy_constructor(obj, prototype);
    }
    void make(std::byte* obj, std::byte* expired) const noexcept {
        if (!_move_constructor)
            throw method_not_implemented{"Move Constructor"};
        _move_constructor(obj, expired);
    }

    void destroy(std::byte* obj) const {
        if (!_destructor)
            throw method_not_implemented{"Destructor"};
        _destructor(obj);
    }

    void copy(std::byte* obj, const std::byte* prototype) const {
        if (!_copier)
            throw method_not_implemented{"Copy Assignment"};
        _copier(obj, prototype);
    }
    void move(std::byte* obj, std::byte* expired) const noexcept {
        if (!_mover)
            throw method_not_implemented{"Move Assignment"};
        _mover(obj, expired);
    }

    auto operator == (const type_t& other) const noexcept {
        return _info == other._info;
    }


    template<typename T>
    static
    auto of() noexcept {
        return type_t{std::in_place_type<T>};
    }

    type_t() = delete;
    type_t(type_t&&) noexcept = default;
    type_t(const type_t&) noexcept = default;
    template<typename T>
    explicit
    type_t(std::in_place_type_t<T>):
            _info{typeid(T)},
            _destructor{[](auto&& ptr){ ptr -> ~T(); }}{
        if constexpr (std::is_default_constructible_v<T>)
            _default_constructor = [](auto&& ptr) {
                new (ptr) T {};
            };
        if constexpr (std::is_copy_constructible_v<T>)
            _copy_constructor = [](auto&& dst, auto&& src) {
                new (dst) T {reinterpret_cast<const T&>(*src)};
            };
        if constexpr (std::is_move_constructible_v<T>)
            _move_constructor = [](auto&& dst, auto&& src) {
                new (dst) T {std::move(reinterpret_cast<T&>(*src))};
            };
        if constexpr (std::is_copy_assignable_v<T>)
            _copier = [](auto&& dst, auto&& src) {
                reinterpret_cast<T&>(*dst) =
                    reinterpret_cast<const T&>(*src);
            };
        if constexpr (std::is_move_assignable_v<T>)
            _mover = [](auto&& dst, auto&& src) {
                reinterpret_cast<T&>(*dst) =
                    std::move(reinterpret_cast<T&>(*src));
            };
    }

private:
    std::type_index
        _info;
    destructor_t*
        _destructor;

    default_constructor_t*
        _default_constructor;
    copier_t*
        _copy_constructor;
    mover_t*
        _move_constructor;
    copier_t*
        _copier;
    mover_t*
        _mover;
};

template<typename T, typename U>
requires std::is_class_v<U>
bool is_instance([[maybe_unused]] U& obj) noexcept {
    return typeid(obj) == typeid(T);
}

template<typename D, typename T>
requires std::is_class_v<T>
auto& as(T& obj) noexcept {
    if constexpr (std::is_const_v<T>) {
        using B = std::remove_const_t<T>;
        static_assert(std::derived_from<D, B>);
        return dynamic_cast<const D&>(obj);
    }
    else {
        using B = T;
        static_assert(std::derived_from<D, B>);
        return dynamic_cast<D&>(obj);
    }
}

template<typename D, typename T>
requires std::is_class_v<T>
auto as(T* obj) noexcept {
    if constexpr (std::is_const_v<std::remove_pointer_t<T>>) {
        using B = std::remove_const_t<std::remove_pointer_t<T>>;
        static_assert(std::derived_from<D, B>);
        return dynamic_cast<const D*>(obj);
    }
    else {
        using B = std::remove_pointer_t<T>;
        static_assert(std::derived_from<D, B>);
        return dynamic_cast<D*>(obj);
    }
}


}
