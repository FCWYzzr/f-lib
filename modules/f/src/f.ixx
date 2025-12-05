//
// Created by MuXin on 2025/11/22.
//

export module f;
export import std;

export import :memory;
export import :rtt;
export import :exception;
export import :container;

export namespace f {
using namespace std::views;


template<typename... Args>
string format(std::format_string<Args...>&& fmt, Args&& ...args) noexcept {
    auto buffer = string{};
    std::format_to(std::back_inserter(buffer), std::move(fmt), std::forward<Args>(args)...);
    return buffer;
}

void assert(const bool condition) {
    if (!condition)
        throw exception("bad assert");
}

template<typename Alloc>
auto& cvt(
    const std::string_view mbs,
    std::basic_string<wchar_t, std::char_traits<wchar_t>, Alloc>& buf) {

    buf.resize(mbs.size());
    auto state = std::mbstate_t{};
    auto p = mbs.data();
    // ReSharper disable once CppDeprecatedEntity
    std::mbsrtowcs(buf.data(), &p, buf.length()+1, &state);
    return buf;
}

template<typename Alloc>
auto& cvt(
    const std::wstring_view wcs,
    std::basic_string<char, std::char_traits<char>, Alloc>& buf) {

    buf.resize(wcs.size() * sizeof(wchar_t));
    auto state = std::mbstate_t{};
    auto p = wcs.data();
    // ReSharper disable once CppDeprecatedEntity
    std::wcsrtombs(buf.data(), &p, buf.length()+1, &state);
    return buf;
}

}
