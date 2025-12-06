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
    process(std::wstring_view cmd, std::wstring_view title){
        auto si = STARTUPINFOW{
            .cb = sizeof(STARTUPINFOW),
            .lpTitle = const_cast<wchar_t*>(title.data())
        };

        if (!CreateProcessW(
                nullptr,
                const_cast<wchar_t*>(cmd.data()),
                nullptr,
                nullptr,
                FALSE,
                CREATE_NO_WINDOW,
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