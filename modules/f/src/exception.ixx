//
// Created by MuXin on 2025/11/26.
//

export module f:exception;
import std;

export namespace f {

template<typename ...Ts>
std::runtime_error
    exception(std::format_string<Ts...>&& fmt, Ts&&... vs) noexcept {
    return std::runtime_error{std::format<Ts...>(fmt, std::forward<Ts>(vs)...)};
}

}

