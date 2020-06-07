#include "pch.h"

#include "VirtualDesktopUtils.h"
#include <objbase.h>
#include <ObjectArray.h>
#include <comip.h>

namespace VirtualDesktopUtils
{
    const CLSID CLSID_ImmersiveShell = { 0xC2F03A33, 0x21F5, 0x47FA, 0xB4, 0xBB, 0x15, 0x63, 0x62, 0xA2, 0xF2, 0x39 };
    const CLSID CLSID_VirtualDesktopAPI_Unknown = { 0xC5E0CDCA, 0x7B6E, 0x41B2, 0x9F, 0xC4, 0xD9, 0x39, 0x75, 0xCC, 0x46, 0x7B };

    EXTERN_C const IID IID_IApplicatonView;
    EXTERN_C const IID IID_IApplicatonViewCollection;
    EXTERN_C const IID IID_IVirtualDesktop;
    EXTERN_C const IID IID_IVirtualDesktopManagerInternal;
    const IID IID_IApplicatonView = { 0x9AC0B5C8, 0x1484, 0x4C5B, 0x95, 0x33, 0x41, 0x34, 0xA0, 0xF9, 0x7C, 0xEA };
    const IID IID_IApplicatonViewCollection = { 0x1841C6D7, 0x4F9D, 0x42C0, 0xAF, 0x41, 0x87, 0x47, 0x53, 0x8F, 0x10, 0xE5 };

    const wchar_t GUID_EmptyGUID[] = L"{00000000-0000-0000-0000-000000000000}";

    const wchar_t RegCurrentVirtualDesktop[] = L"CurrentVirtualDesktop";
    const wchar_t RegVirtualDesktopIds[] = L"VirtualDesktopIDs";
    const wchar_t RegKeyVirtualDesktops[] = L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\VirtualDesktops";

    IServiceProvider* GetServiceProvider()
    {
        IServiceProvider* provider{ nullptr };
        if (FAILED(CoCreateInstance(CLSID_ImmersiveShell, nullptr, CLSCTX_LOCAL_SERVER, __uuidof(provider), (PVOID*)&provider)))
        {
            return nullptr;
        }
        return provider;
    }

    IVirtualDesktopManager* GetVirtualDesktopManager(void)
    {
        IVirtualDesktopManager* manager{ nullptr };
        IServiceProvider* serviceProvider = GetServiceProvider();
        if (serviceProvider == nullptr || FAILED(serviceProvider->QueryService(__uuidof(manager), &manager)))
        {
            return nullptr;
        }
        return manager;
    }

    bool GetWindowDesktopId(HWND topLevelWindow, GUID* desktopId)
    {
        static IVirtualDesktopManager* virtualDesktopManager = GetVirtualDesktopManager();
        return (virtualDesktopManager != nullptr) &&
               SUCCEEDED(virtualDesktopManager->GetWindowDesktopId(topLevelWindow, desktopId));
    }

    bool GetZoneWindowDesktopId(IZoneWindow* zoneWindow, GUID* desktopId)
    {
        // Format: <device-id>_<resolution>_<virtual-desktop-id>
        std::wstring uniqueId = zoneWindow->UniqueId();
        std::wstring virtualDesktopId = uniqueId.substr(uniqueId.rfind('_') + 1);
        if (virtualDesktopId == GUID_EmptyGUID)
        {
            return false;
        }
        return SUCCEEDED(CLSIDFromString(virtualDesktopId.c_str(), desktopId));
    }

    bool GetDesktopIdFromCurrentSession(GUID* desktopId)
    {
        DWORD sessionId;
        ProcessIdToSessionId(GetCurrentProcessId(), &sessionId);

        wchar_t sessionKeyPath[256]{};
        RETURN_IF_FAILED(
            StringCchPrintfW(
                sessionKeyPath,
                ARRAYSIZE(sessionKeyPath),
                L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\SessionInfo\\%d\\VirtualDesktops",
                sessionId));

        wil::unique_hkey key{};
        GUID value{};
        if (RegOpenKeyExW(HKEY_CURRENT_USER, sessionKeyPath, 0, KEY_ALL_ACCESS, &key) == ERROR_SUCCESS)
        {
            DWORD size = sizeof(GUID);
            if (RegQueryValueExW(key.get(), RegCurrentVirtualDesktop, 0, nullptr, reinterpret_cast<BYTE*>(&value), &size) == ERROR_SUCCESS)
            {
                *desktopId = value;
                return true;
            }
        }
        return false;
    }

    bool GetCurrentVirtualDesktopId(GUID* desktopId)
    {
        if (!GetDesktopIdFromCurrentSession(desktopId))
        {
            // Explorer persists current virtual desktop identifier to registry on a per session basis,
            // but only after first virtual desktop switch happens. If the user hasn't switched virtual
            // desktops (only primary desktop) in this session value in registry will be empty.
            // If this value is empty take first element from array of virtual desktops (not kept per session).
            std::vector<GUID> ids{};
            if (!GetVirtualDesktopIds(ids) || ids.empty())
            {
                return false;
            }
            *desktopId = ids[0];
        }
        return true;
    }

    bool GetVirtualDesktopIds(HKEY hKey, std::vector<GUID>& ids)
    {
        if (!hKey)
        {
            return false;
        }
        DWORD bufferCapacity;
        // request regkey binary buffer capacity only
        if (RegQueryValueExW(hKey, RegVirtualDesktopIds, 0, nullptr, nullptr, &bufferCapacity) != ERROR_SUCCESS)
        {
            return false;
        }
        std::unique_ptr<BYTE[]> buffer = std::make_unique<BYTE[]>(bufferCapacity);
        // request regkey binary content
        if (RegQueryValueExW(hKey, RegVirtualDesktopIds, 0, nullptr, buffer.get(), &bufferCapacity) != ERROR_SUCCESS)
        {
            return false;
        }
        const size_t guidSize = sizeof(GUID);
        std::vector<GUID> temp;
        temp.reserve(bufferCapacity / guidSize);
        for (size_t i = 0; i < bufferCapacity; i += guidSize)
        {
            GUID* guid = reinterpret_cast<GUID*>(buffer.get() + i);
            temp.push_back(*guid);
        }
        ids = std::move(temp);
        return true;
    }

    bool GetVirtualDesktopIds(std::vector<GUID>& ids)
    {
        return GetVirtualDesktopIds(GetVirtualDesktopsRegKey(), ids);
    }

    bool GetVirtualDesktopIds(std::vector<std::wstring>& ids)
    {
        std::vector<GUID> guids{};
        if (GetVirtualDesktopIds(guids))
        {
            for (auto& guid : guids)
            {
                wil::unique_cotaskmem_string guidString;
                if (SUCCEEDED(StringFromCLSID(guid, &guidString)))
                {
                    ids.push_back(guidString.get());
                }
            }
            return true;
        }
        return false;
    }

    HKEY OpenVirtualDesktopsRegKey()
    {
        HKEY hKey{ nullptr };
        if (RegOpenKeyEx(HKEY_CURRENT_USER, RegKeyVirtualDesktops, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
        {
            return hKey;
        }
        return nullptr;
    }

    HKEY GetVirtualDesktopsRegKey()
    {
        static wil::unique_hkey virtualDesktopsKey{ OpenVirtualDesktopsRegKey() };
        return virtualDesktopsKey.get();
    }

    void HandleVirtualDesktopUpdates(HWND window, UINT message, HANDLE terminateEvent)
    {
        HKEY virtualDesktopsRegKey = GetVirtualDesktopsRegKey();
        if (!virtualDesktopsRegKey)
        {
            return;
        }
        HANDLE regKeyEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        HANDLE events[2] = { regKeyEvent, terminateEvent };
        while (1)
        {
            if (RegNotifyChangeKeyValue(virtualDesktopsRegKey, TRUE, REG_NOTIFY_CHANGE_LAST_SET, regKeyEvent, TRUE) != ERROR_SUCCESS)
            {
                return;
            }
            if (WaitForMultipleObjects(2, events, FALSE, INFINITE) != (WAIT_OBJECT_0 + 0))
            {
                // if terminateEvent is signalized or WaitForMultipleObjects failed, terminate thread execution
                return;
            }
            PostMessage(window, message, 0, 0);
        }
    }

    IVirtualDesktopManagerInternal* GetVirtualDesktopInternal()
    {
        IVirtualDesktopManagerInternal* manager{ nullptr };
        IServiceProvider* serviceProvider = GetServiceProvider();
        if (FAILED(serviceProvider->QueryService(CLSID_VirtualDesktopAPI_Unknown, &manager)))
        {
            return nullptr;
        }
        return manager;
    }

    IVirtualDesktop* GetVirtualDesktopFromIndex(UINT index)
    {
        static IVirtualDesktopManagerInternal* manager = GetVirtualDesktopInternal();

        if (!manager)
        {
            return nullptr;
        }

        IObjectArray* desktops = nullptr;
        if (FAILED(manager->GetDesktops(&desktops)))
        {
            return nullptr;
        }

        IVirtualDesktop* desktop = nullptr;
        if (FAILED(desktops->GetAt(index, IID_PPV_ARGS(&desktop))) || desktop == nullptr)
        {
            return nullptr;
        }

        return desktop;
    }

    bool SwitchToVirtualDesktop(UINT index)
    {
        static IVirtualDesktopManagerInternal* manager = GetVirtualDesktopInternal();
        IVirtualDesktop* desktop = GetVirtualDesktopFromIndex(index);
        if (desktop != nullptr)
        {
            manager->SwitchDesktop(desktop);
            return true;
        }
        return false;
    }

    IApplicationViewCollection* GetApplicationViewCollection(void)
    {
        IApplicationViewCollection* collection;
        GetServiceProvider()->QueryService(IID_IApplicatonViewCollection, &collection);
        return collection;
    }

    bool MoveWindowToVirtualDesktop(HWND window, UINT index)
    {
        static IVirtualDesktopManagerInternal* manager = GetVirtualDesktopInternal();
        IVirtualDesktop* desktop = GetVirtualDesktopFromIndex(index);
        if (desktop == nullptr)
        {
            return false;
        }
        IApplicationViewCollection* applications = GetApplicationViewCollection();
        IApplicationView* applicationView;
        applications->GetViewForHwnd(window, &applicationView);
        manager->MoveViewToDesktop(applicationView, desktop);
        return true;
    }
}
