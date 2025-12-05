//
// Created by MuXin on 2025/12/6.
//
module;
#include <Windows.h>
export module f.win32:thread;
import std;
import f;

export namespace f::win32 {

class process {
public:
    void join(const DWORD timeout=INFINITE) const {
        WaitForSingleObject(_process_info.hProcess, timeout);
    }
    void interrupt() const {
        GenerateConsoleCtrlEvent(CTRL_C_EVENT, GetProcessId(_process_info.hProcess));
    }
    void terminate(const UINT code=0) const {
        TerminateProcess(_process_info.hProcess, code);
    }

    explicit
    process(std::wstring_view cmd):
        process{L"", std::move(cmd)}
    {}
    process(const std::wstring_view app, const std::wstring_view arg){
        auto si = STARTUPINFOW{
            .cb = sizeof(STARTUPINFOW)
        };

        if (!CreateProcessW(
                app.empty() ? nullptr : app.data(),
                const_cast<wchar_t*>(arg.data()),
                nullptr,
                nullptr,
                FALSE,
                0,
                nullptr,
                nullptr,
                &si,
                &_process_info))
            throw std::runtime_error("创建进程失败");

    }
    ~process() {
        join();
        CloseHandle(_process_info.hProcess);
    }

private:
    PROCESS_INFORMATION
        _process_info;
};

}