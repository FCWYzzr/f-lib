//
// Created by MuXin on 2025/11/23.
//

export module f:rtt;
export import std;

export namespace f {

using std::type_index;

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


auto type_of([[maybe_unused]] auto& obj) noexcept {
    return type_index{typeid(obj)};
}

}
