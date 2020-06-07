#include "pch.h"
#include "WindowMoveHandler.h"

#include <common/notifications.h>
#include <common/notifications/fancyzones_notifications.h>
#include <common/window_helpers.h>

#include "lib/Settings.h"
#include "lib/ZoneWindow.h"
#include "lib/util.h"
#include "VirtualDesktopUtils.h"

extern "C" IMAGE_DOS_HEADER __ImageBase;

namespace WindowMoveHandlerUtils
{
    bool IsCursorTypeIndicatingSizeEvent()
    {
        CURSORINFO cursorInfo = { 0 };
        cursorInfo.cbSize = sizeof(cursorInfo);

        if (::GetCursorInfo(&cursorInfo))
        {
            if (::LoadCursor(NULL, IDC_SIZENS) == cursorInfo.hCursor)
            {
                return true;
            }
            if (::LoadCursor(NULL, IDC_SIZEWE) == cursorInfo.hCursor)
            {
                return true;
            }
            if (::LoadCursor(NULL, IDC_SIZENESW) == cursorInfo.hCursor)
            {
                return true;
            }
            if (::LoadCursor(NULL, IDC_SIZENWSE) == cursorInfo.hCursor)
            {
                return true;
            }
        }
        return false;
    }
}


class WindowMoveHandlerPrivate
{
public:
    WindowMoveHandlerPrivate(const winrt::com_ptr<IFancyZonesSettings>& settings) :
        m_settings(settings)
    {};

    void MoveWindowIntoZoneByIndexSet(HWND window, HMONITOR monitor, const std::vector<int>& indexSet, const std::map<HMONITOR, winrt::com_ptr<IZoneWindow>>& zoneWindowMap) noexcept;
    bool MoveWindowIntoZoneByDirection(HMONITOR monitor, HWND window, DWORD vkCode, bool cycle, const std::map<HMONITOR, winrt::com_ptr<IZoneWindow>>& zoneWindowMap);


private:
    winrt::com_ptr<IFancyZonesSettings> m_settings{};

    HWND m_windowMoveSize{}; // The window that is being moved/sized
    bool m_inMoveSize{}; // Whether or not a move/size operation is currently active
    winrt::com_ptr<IZoneWindow> m_zoneWindowMoveSize; // "Active" ZoneWindow, where the move/size is happening. Will update as drag moves between monitors.
    bool m_dragEnabled{}; // True if we should be showing zone hints while dragging
};


WindowMoveHandler::WindowMoveHandler(const winrt::com_ptr<IFancyZonesSettings>& settings) :
    pimpl(new WindowMoveHandlerPrivate(settings))
{
}

WindowMoveHandler::~WindowMoveHandler()
{
    delete pimpl;
}

void WindowMoveHandler::MoveWindowIntoZoneByIndexSet(HWND window, HMONITOR monitor, const std::vector<int>& indexSet, const std::map<HMONITOR, winrt::com_ptr<IZoneWindow>>& zoneWindowMap) noexcept
{
    pimpl->MoveWindowIntoZoneByIndexSet(window, monitor, indexSet, zoneWindowMap);
}

bool WindowMoveHandler::MoveWindowIntoZoneByDirection(HMONITOR monitor, HWND window, DWORD vkCode, bool cycle, const std::map<HMONITOR, winrt::com_ptr<IZoneWindow>>& zoneWindowMap)
{
    return pimpl->MoveWindowIntoZoneByDirection(monitor, window, vkCode, cycle, zoneWindowMap);
}

void WindowMoveHandlerPrivate::MoveWindowIntoZoneByIndexSet(HWND window, HMONITOR monitor, const std::vector<int>& indexSet, const std::map<HMONITOR, winrt::com_ptr<IZoneWindow>>& zoneWindowMap) noexcept
{
    if (window != m_windowMoveSize)
    {
        const HMONITOR hm = (monitor != nullptr) ? monitor : MonitorFromWindow(window, MONITOR_DEFAULTTONULL);
        if (hm)
        {
            auto zoneWindow = zoneWindowMap.find(hm);
            if (zoneWindow != zoneWindowMap.end())
            {
                const auto& zoneWindowPtr = zoneWindow->second;
                // Only process windows located on currently active work area.
                GUID windowDesktopId{};
                GUID zoneWindowDesktopId{};
                if (VirtualDesktopUtils::GetWindowDesktopId(window, &windowDesktopId) &&
                    VirtualDesktopUtils::GetZoneWindowDesktopId(zoneWindowPtr.get(), &zoneWindowDesktopId) &&
                    (windowDesktopId != zoneWindowDesktopId))
                {
                    return;
                }
                zoneWindowPtr->MoveWindowIntoZoneByIndexSet(window, indexSet);
            }
        }
    }
}

bool WindowMoveHandlerPrivate::MoveWindowIntoZoneByDirection(HMONITOR monitor, HWND window, DWORD vkCode, bool cycle, const std::map<HMONITOR, winrt::com_ptr<IZoneWindow>>& zoneWindowMap)
{
    auto iter = zoneWindowMap.find(monitor);
    if (iter != std::end(zoneWindowMap))
    {
        const auto& zoneWindowPtr = iter->second;
        return zoneWindowPtr->MoveWindowIntoZoneByDirection(window, vkCode, cycle);
    }
    return false;
}
