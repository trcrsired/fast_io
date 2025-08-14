#define NOMINMAX 1
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#include <wlanapi.h>
#include <fast_io.h>
#include <fast_io_dsal/string_view.h>
#include <cstdint>
#pragma comment(lib, "wlanapi.lib")

class win32_wlan_file {
public:
    explicit constexpr win32_wlan_file() noexcept = default;

    explicit win32_wlan_file(::std::uint_least32_t version) {
        ::std::uint_least32_t negotiated_version{};
        if (WlanOpenHandle(version, nullptr, reinterpret_cast<PDWORD>(::std::addressof(negotiated_version)), reinterpret_cast<void**>(&handle)) != ERROR_SUCCESS) {
            ::fast_io::io::panic("WlanOpenHandle failed\n");
        }
    }

    ~win32_wlan_file() {
        if (handle) WlanCloseHandle(reinterpret_cast<void*>(handle), nullptr);
    }

    void* handle = nullptr;

    win32_wlan_file(const win32_wlan_file&) = delete;
    win32_wlan_file& operator=(const win32_wlan_file&) = delete;

    constexpr win32_wlan_file(win32_wlan_file&& other) noexcept : handle(other.handle) {
        other.handle = nullptr;
    }

    win32_wlan_file& operator=(win32_wlan_file&& other) noexcept {
        if (this != &other) {
            if (handle) WlanCloseHandle(reinterpret_cast<HANDLE>(handle), nullptr);
            handle = other.handle;
            other.handle = nullptr;
        }
        return *this;
    }
};

class win32_wlan_memory_file {
public:
    explicit constexpr win32_wlan_memory_file() noexcept  = default;

    ~win32_wlan_memory_file() {
        if (hd) WlanFreeMemory(hd);
    }

    void* hd = nullptr;

    win32_wlan_memory_file(const win32_wlan_memory_file&) = delete;
    win32_wlan_memory_file& operator=(const win32_wlan_memory_file&) = delete;

    constexpr win32_wlan_memory_file(win32_wlan_memory_file&& other) noexcept : hd(other.hd) {
        other.hd = nullptr;
    }

    win32_wlan_memory_file& operator=(win32_wlan_memory_file&& other) noexcept {
        if (this != &other) {
            if (hd) WlanFreeMemory(hd);
            hd = other.hd;
            other.hd = nullptr;
        }
        return *this;
    }
};

int main() {
    win32_wlan_file wlan(2);

    win32_wlan_memory_file if_mem;
    if (WlanEnumInterfaces(wlan.handle, nullptr,
        reinterpret_cast<WLAN_INTERFACE_INFO_LIST**>(::std::addressof(if_mem.hd))) != ERROR_SUCCESS) {
        ::fast_io::io::panic("WlanEnumInterfaces failed\n");
    }

    auto* if_list = reinterpret_cast<WLAN_INTERFACE_INFO_LIST*>(if_mem.hd);
    for (::std::uint_least32_t i{}; i != if_list->dwNumberOfItems; ++i) {
        GUID guid = if_list->InterfaceInfo[i].InterfaceGuid;

        win32_wlan_memory_file bss_mem;
        if (WlanGetNetworkBssList(wlan.handle, ::std::addressof(guid), nullptr,
            dot11_BSS_type_any, FALSE, nullptr,
            reinterpret_cast<WLAN_BSS_LIST**>(::std::addressof(bss_mem.hd))) != ERROR_SUCCESS) {
            ::fast_io::io::perrln("WlanGetNetworkBssList failed:", i );
            continue;
        }

        auto* bss_list{reinterpret_cast<WLAN_BSS_LIST*>(bss_mem.hd)};
        for (::std::uint_least32_t j{}; j != bss_list->dwNumberOfItems; ++j) {
            const WLAN_BSS_ENTRY& entry = bss_list->wlanBssEntries[j];
            ::fast_io::string_view ssid(reinterpret_cast<char const*>(entry.dot11Ssid.ucSSID), entry.dot11Ssid.uSSIDLength);
            if (ssid.is_empty())
            {
                ssid=::fast_io::string_view("[Hidden]");
            }
            ::fast_io::io::print("SSID:", ssid, "\t",
                "Signal:", entry.lRssi, " dBm\t"
                "Frequency:",entry.ulChCenterFrequency,"kHz\n");
        }
    }
}
