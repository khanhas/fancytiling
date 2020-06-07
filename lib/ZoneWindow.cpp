#include "pch.h"


#include "ZoneWindow.h"
#include "util.h"

#include <ShellScalingApi.h>
#include <mutex>

#include <gdiplus.h>

namespace ZoneWindowUtils
{
    const std::wstring& GetActiveZoneSetTmpPath()
    {
        static std::wstring activeZoneSetTmpFileName;
        static std::once_flag flag;

        std::call_once(flag, []() {
            wchar_t fileName[L_tmpnam_s];

            if (_wtmpnam_s(fileName, L_tmpnam_s) != 0)
                abort();

            activeZoneSetTmpFileName = std::wstring{ fileName };
        });

        return activeZoneSetTmpFileName;
    }

    const std::wstring& GetAppliedZoneSetTmpPath()
    {
        static std::wstring appliedZoneSetTmpFileName;
        static std::once_flag flag;

        std::call_once(flag, []() {
            wchar_t fileName[L_tmpnam_s];

            if (_wtmpnam_s(fileName, L_tmpnam_s) != 0)
                abort();

            appliedZoneSetTmpFileName = std::wstring{ fileName };
        });

        return appliedZoneSetTmpFileName;
    }

    const std::wstring& GetCustomZoneSetsTmpPath()
    {
        static std::wstring customZoneSetsTmpFileName;
        static std::once_flag flag;

        std::call_once(flag, []() {
            wchar_t fileName[L_tmpnam_s];

            if (_wtmpnam_s(fileName, L_tmpnam_s) != 0)
                abort();

            customZoneSetsTmpFileName = std::wstring{ fileName };
        });

        return customZoneSetsTmpFileName;
    }

    std::wstring GenerateUniqueId(HMONITOR monitor, PCWSTR deviceId, PCWSTR virtualDesktopId)
    {
        wchar_t uniqueId[256]{}; // Parsed deviceId + resolution + virtualDesktopId

        MONITORINFOEXW mi;
        mi.cbSize = sizeof(mi);
        if (virtualDesktopId && GetMonitorInfo(monitor, &mi))
        {
            wchar_t parsedId[256]{};
            ParseDeviceId(deviceId, parsedId, 256);

            Rect const monitorRect(mi.rcMonitor);
            StringCchPrintf(uniqueId, ARRAYSIZE(uniqueId), L"%s_%d_%d_%s", parsedId, monitorRect.width(), monitorRect.height(), virtualDesktopId);
        }
        return std::wstring{ uniqueId };
    }
}

struct ZoneWindow : public winrt::implements<ZoneWindow, IZoneWindow>
{
public:
    ZoneWindow(HINSTANCE hinstance);
    ~ZoneWindow();

    bool Init(IZoneWindowHost* host, HINSTANCE hinstance, HMONITOR monitor, const std::wstring& uniqueId, bool flashZones, bool newWorkArea);

    IFACEMETHODIMP_(void)
    MoveWindowIntoZoneByIndex(HWND window, int index) noexcept;
    IFACEMETHODIMP_(void)
    MoveWindowIntoZoneByIndexSet(HWND window, const std::vector<int>& indexSet) noexcept;
    IFACEMETHODIMP_(bool)
    MoveWindowIntoZoneByDirection(HWND window, DWORD vkCode, bool cycle) noexcept;
    IFACEMETHODIMP_(std::wstring)
    UniqueId() noexcept { return { m_uniqueId }; }
    IFACEMETHODIMP_(std::wstring)
    WorkAreaKey() noexcept { return { m_workArea }; }
    IFACEMETHODIMP_(void)
    SaveWindowProcessToZoneIndex(HWND window) noexcept;
    IFACEMETHODIMP_(IZoneSet*)
    ActiveZoneSet() noexcept { return m_activeZoneSet.get(); }
    IFACEMETHODIMP_(void)
    UpdateActiveZoneSet() noexcept;

protected:
    static LRESULT CALLBACK s_WndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam) noexcept;

private:
    void LoadSettings() noexcept;
    void InitializeZoneSets(bool newWorkArea) noexcept;
    void CalculateZoneSet() noexcept;
    std::vector<int> ZonesFromPoint(POINT pt) noexcept;
    LRESULT ZoneWindow::WndProc(UINT message, WPARAM wparam, LPARAM lparam) noexcept;

    winrt::com_ptr<IZoneWindowHost> m_host;
    HMONITOR m_monitor{};
    std::wstring m_uniqueId; // Parsed deviceId + resolution + virtualDesktopId
    wchar_t m_workArea[256]{};
    wil::unique_hwnd m_window{}; // Hidden tool window used to represent current monitor desktop work area.
    winrt::com_ptr<IZoneSet> m_activeZoneSet;
    std::vector<winrt::com_ptr<IZoneSet>> m_zoneSets;
    WPARAM m_keyLast{};
    size_t m_keyCycle{};

    ULONG_PTR gdiplusToken;
};

ZoneWindow::ZoneWindow(HINSTANCE hinstance)
{
    WNDCLASSEXW wcex{};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.lpfnWndProc = s_WndProc;
    wcex.hInstance = hinstance;
    wcex.lpszClassName = L"FancyTiling_ZoneWindow";
    wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    RegisterClassExW(&wcex);

    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}

ZoneWindow::~ZoneWindow()
{
    Gdiplus::GdiplusShutdown(gdiplusToken);
}

bool ZoneWindow::Init(IZoneWindowHost* host, HINSTANCE hinstance, HMONITOR monitor, const std::wstring& uniqueId, bool flashZones, bool newWorkArea)
{
    m_host.copy_from(host);

    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(monitor, &mi))
    {
        return false;
    }

    m_monitor = monitor;
    const UINT dpi = GetDpiForMonitor(m_monitor);
    const Rect monitorRect(mi.rcMonitor);
    const Rect workAreaRect(mi.rcWork, dpi);
    StringCchPrintf(m_workArea, ARRAYSIZE(m_workArea), L"%d_%d", monitorRect.width(), monitorRect.height());

    m_uniqueId = uniqueId;
    LoadSettings();
    InitializeZoneSets(newWorkArea);

    m_window = wil::unique_hwnd{
        CreateWindowExW(WS_EX_TOOLWINDOW, L"FancyTiling_ZoneWindow", L"", WS_POPUP, workAreaRect.left(), workAreaRect.top(), workAreaRect.width(), workAreaRect.height(), nullptr, nullptr, hinstance, this)
    };

    if (!m_window)
    {
        return false;
    }

    return true;
}

IFACEMETHODIMP_(void)
ZoneWindow::MoveWindowIntoZoneByIndex(HWND window, int index) noexcept
{
    MoveWindowIntoZoneByIndexSet(window, { index });
}

IFACEMETHODIMP_(void)
ZoneWindow::MoveWindowIntoZoneByIndexSet(HWND window, const std::vector<int>& indexSet) noexcept
{
    if (m_activeZoneSet)
    {
        m_activeZoneSet->MoveWindowIntoZoneByIndexSet(window, m_window.get(), indexSet, false);
    }
}

IFACEMETHODIMP_(bool)
ZoneWindow::MoveWindowIntoZoneByDirection(HWND window, DWORD vkCode, bool cycle) noexcept
{
    if (m_activeZoneSet)
    {
        if (m_activeZoneSet->MoveWindowIntoZoneByDirection(window, m_window.get(), vkCode, cycle))
        {
            SaveWindowProcessToZoneIndex(window);

            return true;
        }
    }
    return false;
}

IFACEMETHODIMP_(void)
ZoneWindow::SaveWindowProcessToZoneIndex(HWND window) noexcept
{
    if (m_activeZoneSet)
    {
        auto zoneIndexSet = m_activeZoneSet->GetZoneIndexSetFromWindow(window);
        if (zoneIndexSet.size())
        {
            OLECHAR* guidString;
            if (StringFromCLSID(m_activeZoneSet->Id(), &guidString) == S_OK)
            {
                JSONHelpers::FancyZonesDataInstance().SetAppLastZones(window, m_uniqueId, guidString, zoneIndexSet);
            }

            CoTaskMemFree(guidString);
        }
    }
}

IFACEMETHODIMP_(void)
ZoneWindow::UpdateActiveZoneSet() noexcept
{
    CalculateZoneSet();
}

#pragma region private

void ZoneWindow::LoadSettings() noexcept
{
    JSONHelpers::FancyZonesDataInstance().AddDevice(m_uniqueId);
}

void ZoneWindow::InitializeZoneSets(bool newWorkArea) noexcept
{
    auto parent = m_host->GetParentZoneWindow(m_monitor);
    if (newWorkArea && parent)
    {
        // Update device info with device info from parent virtual desktop (if empty).
        JSONHelpers::FancyZonesDataInstance().CloneDeviceInfo(parent->UniqueId(), m_uniqueId);
    }
    CalculateZoneSet();
}

void ZoneWindow::CalculateZoneSet() noexcept
{
    const auto& fancyZonesData = JSONHelpers::FancyZonesDataInstance();
    const auto deviceInfoData = fancyZonesData.FindDeviceInfo(m_uniqueId);
    const auto& activeDeviceId = fancyZonesData.GetActiveDeviceId();

    if (!activeDeviceId.empty() && activeDeviceId != m_uniqueId)
    {
        return;
    }

    if (!deviceInfoData.has_value())
    {
        return;
    }

    const auto& activeZoneSet = deviceInfoData->activeZoneSet;

    if (activeZoneSet.uuid.empty() || activeZoneSet.type == JSONHelpers::ZoneSetLayoutType::Blank)
    {
        return;
    }

    GUID zoneSetId;
    if (SUCCEEDED_LOG(CLSIDFromString(activeZoneSet.uuid.c_str(), &zoneSetId)))
    {
        auto zoneSet = MakeZoneSet(ZoneSetConfig(
            zoneSetId,
            activeZoneSet.type,
            m_monitor,
            m_workArea));
        MONITORINFO monitorInfo{};
        monitorInfo.cbSize = sizeof(monitorInfo);
        if (GetMonitorInfoW(m_monitor, &monitorInfo))
        {
            bool showSpacing = deviceInfoData->showSpacing;
            int spacing = showSpacing ? deviceInfoData->spacing : 0;
            int zoneCount = deviceInfoData->zoneCount;
            zoneSet->CalculateZones(monitorInfo, zoneCount, spacing);
            m_activeZoneSet.copy_from(zoneSet.get());
        }
    }
}

std::vector<int> ZoneWindow::ZonesFromPoint(POINT pt) noexcept
{
    if (m_activeZoneSet)
    {
        return m_activeZoneSet->ZonesFromPoint(pt);
    }
    return {};
}

LRESULT ZoneWindow::WndProc(UINT message, WPARAM wparam, LPARAM lparam) noexcept
{
    switch (message)
    {
    case WM_NCDESTROY:
    {
        ::DefWindowProc(m_window.get(), message, wparam, lparam);
        SetWindowLongPtr(m_window.get(), GWLP_USERDATA, 0);
    }
    break;

    case WM_ERASEBKGND:
        return 1;

    case WM_PRINTCLIENT:
    case WM_PAINT:
        break;

    default:
    {
        return DefWindowProc(m_window.get(), message, wparam, lparam);
    }
    }
    return 0;
}

#pragma endregion
LRESULT CALLBACK ZoneWindow::s_WndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam) noexcept
{
    auto thisRef = reinterpret_cast<ZoneWindow*>(GetWindowLongPtr(window, GWLP_USERDATA));
    if ((thisRef == nullptr) && (message == WM_CREATE))
    {
        auto createStruct = reinterpret_cast<LPCREATESTRUCT>(lparam);
        thisRef = reinterpret_cast<ZoneWindow*>(createStruct->lpCreateParams);
        SetWindowLongPtr(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(thisRef));
    }

    return (thisRef != nullptr) ? thisRef->WndProc(message, wparam, lparam) :
                                  DefWindowProc(window, message, wparam, lparam);
}

winrt::com_ptr<IZoneWindow> MakeZoneWindow(IZoneWindowHost* host, HINSTANCE hinstance, HMONITOR monitor, const std::wstring& uniqueId, bool flashZones, bool newWorkArea) noexcept
{
    auto self = winrt::make_self<ZoneWindow>(hinstance);
    if (self->Init(host, hinstance, monitor, uniqueId, flashZones, newWorkArea))
    {
        return self;
    }

    return nullptr;
}
