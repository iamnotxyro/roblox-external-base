#pragma once
#include <windows.h>
#include <TlHelp32.h>
#include <cstdint>
#include <psapi.h>

#include <string>

namespace memory
{
    inline int32_t pid = 0;
    inline HANDLE process = nullptr;
    inline uintptr_t base_address = 0;

    __forceinline int32_t get_process(LPCTSTR processName) noexcept
    {
        PROCESSENTRY32 pe{};
        pe.dwSize = sizeof(pe);

        HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snap == INVALID_HANDLE_VALUE)
            return 0;

        if (Process32First(snap, &pe))
        {
            do
            {
                if (!lstrcmpi(pe.szExeFile, processName))
                {
                    CloseHandle(snap);
                    return pe.th32ProcessID;
                }
            } while (Process32Next(snap, &pe));
        }

        CloseHandle(snap);
        return 0;
    }

    __forceinline uintptr_t get_base() noexcept
    {
        if (!pid) return 0;

        HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
        if (!hProc) return 0;

        HMODULE mod = nullptr;
        DWORD needed = 0;
        uintptr_t base = 0;

        if (EnumProcessModules(hProc, &mod, sizeof(mod), &needed))
            base = reinterpret_cast<uintptr_t>(mod);

        CloseHandle(hProc);
        return base;
    }

    __forceinline bool EnsureSyscallInit() noexcept
    {
        if (process || !pid) return process != nullptr;

        process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
        return process != nullptr;
    }

    template <typename T>
    __forceinline T read(uint64_t address) noexcept
    {
        T buffer{};
        SIZE_T bytesRead = 0;

        if (process)
        {
            ReadProcessMemory(process, reinterpret_cast<LPCVOID>(address), &buffer, sizeof(T), &bytesRead);
        }

        return buffer;
    }

    __forceinline std::string read_string(uint64_t address, size_t max_length = 256) noexcept
    {
        if (!address) return {};

        std::string result(max_length, '\0');
        SIZE_T bytesRead = 0;

        if (process)
        {
            ReadProcessMemory(process, reinterpret_cast<LPCVOID>(address), result.data(), max_length, &bytesRead);
        }

        result.resize(strnlen(result.c_str(), max_length));
        return result;
    }
}