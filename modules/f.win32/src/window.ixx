module;
#include <Windows.h>
#include <Shellapi.h>

export module f.win32:window;
import std;
import f;

export namespace f::win32 {


class window {
    friend class window_class;
public:
    auto show() noexcept -> void;
    auto hide() noexcept -> void;
    auto destroy() noexcept -> void;

    template<typename Fn>
    void on(const UINT event, Fn&& fn) {
        _handlers[event] = std::forward<Fn>(fn);
    }
    
    auto handle() const noexcept -> HWND;
    auto title() const noexcept -> string;
    auto title(std::string_view new_title) noexcept -> string;
    
    auto consume_message() noexcept -> bool;
    auto try_consume_message() noexcept -> bool;

    auto ex_style() const noexcept -> DWORD;
    auto ex_style(DWORD new_style) noexcept -> DWORD;

    explicit window(HINSTANCE instance, std::string_view title = {}) noexcept(false);
    ~window() noexcept;
    window(const window&) noexcept = delete;
    window(window&& other) noexcept;
    window& operator=(const window&) noexcept = delete;
    window& operator=(window&& other) noexcept;

private:
    static auto wnd_proc(HWND, UINT, WPARAM, LPARAM) noexcept -> LRESULT;


    HINSTANCE
        _instance;
    
    HWND
        _handle;

    unordered_map<UINT, std::function<auto (window* wnd, WPARAM, LPARAM) -> HRESULT>>
        _handlers;


};

class tray {
public:
    tray(std::string_view tool_tip, UINT event, HICON icon=nullptr, UINT uid=0, UINT flag=NIF_ICON | NIF_MESSAGE | NIF_TIP, const window* master=nullptr);

    ~tray();

private:
    NOTIFYICONDATAW
        _tray_icon_data{};

};


}