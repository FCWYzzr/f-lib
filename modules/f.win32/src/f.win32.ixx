//
// Created by MuXin on 2025/11/29.
//
module;
#include <winerror.h>
export module f.win32;
export import f;
export import :window;
export import :com;
export import :thread;

export namespace f::win32 {

const auto s_ok_checker = equal_or_throw<HRESULT>{S_OK};

}