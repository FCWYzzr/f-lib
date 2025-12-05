//
// Created by MuXin on 2025/11/29.
//
module;
#include <Windows.h>

export module f.win32:com;
import std;
import f;

export namespace f::win32 {

struct com_init_error: std::runtime_error {
    enum class fail_reason: long {
        ALREADY_INIT = S_FALSE,
        MODE_CHANGE = RPC_E_CHANGED_MODE
    };
    constexpr static const char* fail_reason_string[] = {
        "ALREADY_INIT", "MODE_CHANGE"
    };

    fail_reason reason;

    explicit
    com_init_error(fail_reason&& reason) noexcept:
        std::runtime_error{fail_reason_string[(static_cast<HRESULT>(reason) == S_FALSE) ? 0 : 1]},
        reason{std::move(reason)}
    {}
};

struct com_context {
    [[deprecated("新应用程序应当调用CoInitializeEx(带参数构造com_context)")]]
    com_context() {
        if (const auto ret = CoInitialize(nullptr); ret != S_OK)
            throw com_init_error{static_cast<com_init_error::fail_reason>(ret)};
    }

    template<typename ...Ts>
    requires (std::same_as<COINIT, Ts> && ...)
    explicit
    com_context(Ts... vs) {
        if (const auto ret = CoInitializeEx(nullptr, (vs |...)); ret != S_OK)
            throw com_init_error{static_cast<com_init_error::fail_reason>(ret)};
    }
    ~com_context() noexcept {
        CoUninitialize();
    }

    com_context(com_context&&)=delete;
    com_context(const com_context&)=delete;
};

template<typename T>
requires std::derived_from<T, IUnknown>
struct com_deleter {
    void operator () (T* ptr) const {
        if (ptr != nullptr)
            ptr->Release();
    }
};

template<typename T>
using unique_com = std::unique_ptr<T, com_deleter<T>>;

}
