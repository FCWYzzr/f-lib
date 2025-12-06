// ReSharper disable CppMemberFunctionMayBeConst
module;
#include <Windows.h>

module f.win32;

namespace f::win32 {


auto window::show() noexcept -> void {
    if (_handle != nullptr) {
        ShowWindow(_handle, SW_SHOW);
        UpdateWindow(_handle);
    }
}

auto window::hide() noexcept -> void {
    if (_handle != nullptr) {
        ShowWindow(_handle, SW_HIDE);
    }
}

auto window::destroy() noexcept -> void {
    if (_handle != nullptr) {
        DestroyWindow(_handle);
        _handle = nullptr;
    }
}

auto window::handle() const noexcept -> HWND {
    return _handle;
}

auto window::title() const noexcept -> string {
    const auto length = GetWindowTextLengthW(_handle);
    if (length == 0) {
        return {};
    }

    // 分配缓冲区并获取标题
    auto wide_buffer = std::wstring(length, L'\0');
    GetWindowTextW(_handle, wide_buffer.data(), static_cast<int>(wide_buffer.size() + 1));

    // 转换为窄字符，使用提供的f::cvt实现
    auto buffer = string{};
    cvt(wide_buffer, buffer);

    return buffer;
}

auto window::title(
    const std::string_view new_title) noexcept -> string {
    auto old_title = title();

    if (_handle != nullptr) {
        // 转换标题为宽字符，使用提供的f::cvt实现
        auto wide_title = std::wstring{};
        cvt(new_title, wide_title);
        SetWindowTextW(_handle, wide_title.c_str());
    }

    return old_title;
}

auto window::consume_message() noexcept -> bool {
    MSG msg;
    auto has_message = false;

    // 只处理本窗口的消息
    while(const auto ret = GetMessageW(&msg, handle(), 0, 0)){
        if (ret == -1)
            break;

        TranslateMessage(&msg);
        DispatchMessageW(&msg);
        has_message = true;
    }

    return has_message;
}
auto window::try_consume_message() noexcept -> bool {
    MSG msg;

    // 只处理本窗口的消息
    while (PeekMessageW(&msg, handle(), 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);

        if (msg.message == WM_QUIT) {
            return false;
        }
    }

    return true;
}

// 添加ex_style方法的实现
auto window::ex_style() const noexcept -> DWORD {
    return GetWindowLongPtrW(_handle, GWL_EXSTYLE);
}

auto window::ex_style(
    const DWORD new_style) noexcept -> DWORD {
    return SetWindowLongPtrW(_handle, GWL_EXSTYLE, new_style);
}


window::window(
    const HINSTANCE instance,
    const std::string_view title) noexcept(false) :
    _instance{instance},
    _handle{nullptr}
{
    const auto cls_name = f::format("{}_class", title);
    auto w_cls_name = wstring{};
    cvt(std::string_view{cls_name}, w_cls_name);

    const auto window_class = WNDCLASSEXW{
        .cbSize = sizeof(WNDCLASSEXW),
        .style = CS_HREDRAW | CS_VREDRAW,
        .lpfnWndProc = wnd_proc,
        .cbClsExtra = 0,
        .cbWndExtra = 0,
        .hInstance = _instance,
        .hIcon = nullptr,
        .hCursor = nullptr,
        .hbrBackground = nullptr,
        .lpszMenuName = nullptr,
        .lpszClassName = w_cls_name.c_str(),
        .hIconSm = nullptr
    };

    if (!RegisterClassExW(&window_class))
        throw exception("fail to register wnd class");




    // 转换标题为宽字符，使用提供的f::cvt实现
    auto wide_title = std::wstring{};
    cvt(title, wide_title);

    // 创建窗口
    _handle = CreateWindowExW(
        0,                              // 扩展样式
        w_cls_name.c_str(),              // 类名
        wide_title.c_str(),             // 窗口标题
        WS_OVERLAPPEDWINDOW,            // 窗口样式
        CW_USEDEFAULT, CW_USEDEFAULT,   // 位置
        CW_USEDEFAULT, CW_USEDEFAULT,   // 大小
        nullptr,                        // 父窗口
        nullptr,                        // 菜单
        _instance,                      // 实例句柄
        nullptr                         // 用户数据
        );

    if (_handle == nullptr) {
        throw exception("Failed to create window");
    }

    // 设置用户数据
    SetWindowLongPtrW(_handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
}

window::~window() noexcept {
    if (_handle != nullptr) {
        DestroyWindow(_handle);
        _handle = nullptr;
    }
}

window::window(window&& other) noexcept :
    _instance{other._instance},
    _handle{other._handle}
{
    other._handle = nullptr;

    if (_handle != nullptr) {
        SetWindowLongPtrW(_handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    }
}

window& window::operator=(window&& other) noexcept {
    if (this != &other) {
        // 清理当前窗口
        if (_handle != nullptr) {
            DestroyWindow(_handle);
            _handle = nullptr;
        }

        // 移动资源
        _instance = other._instance;
        _handle = other._handle;
        other._handle = nullptr;

        // 更新用户数据
        if (_handle != nullptr) {
            SetWindowLongPtrW(_handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
        }
    }

    return *this;
}


LRESULT CALLBACK window::wnd_proc(
    const HWND handle,
    const UINT msg,
    const WPARAM wp,
    const LPARAM lp) noexcept {
    const auto window_ptr = reinterpret_cast<window*>(GetWindowLongPtrW(handle, GWLP_USERDATA));

    if (window_ptr == nullptr)
        return DefWindowProcW(handle, msg, wp, lp);


    if (auto& table = window_ptr->_handlers;
        table.contains(msg))
        return table[msg](window_ptr, wp, lp);

    return DefWindowProcW(handle, msg, wp, lp);
}

tray::tray(
        const std::string_view tool_tip,
        const UINT event, const HICON icon,
        const UINT uid, const UINT flag,
        const window* master):
    _tray_icon_data{
        .cbSize = sizeof(NOTIFYICONDATAW),
        .hWnd = nullptr,
        .uID = uid,
        .uFlags = flag,
        .uCallbackMessage = event,
        .hIcon = icon
    } {

    if (master)
        _tray_icon_data.hWnd = master->handle();

    if (!icon)
        _tray_icon_data.hIcon = LoadIconA(nullptr, IDI_APPLICATION);

    auto state = std::mbstate_t{};
    auto src = tool_tip.data();
    // ReSharper disable once CppDeprecatedEntity
    std::mbsrtowcs(_tray_icon_data.szTip, &src, 128, &state);

    if (!Shell_NotifyIconW(NIM_ADD, &_tray_icon_data))
        throw exception("fail to add tray");
}

tray::~tray() {
    Shell_NotifyIconW(NIM_DELETE, &_tray_icon_data);
}
}
