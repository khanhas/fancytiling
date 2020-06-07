#pragma once

interface IFancyZonesSettings;
interface IZoneWindow;

class WindowMoveHandler
{
public:
    WindowMoveHandler(const winrt::com_ptr<IFancyZonesSettings>& settings);
    ~WindowMoveHandler();

    void MoveWindowIntoZoneByIndexSet(HWND window, HMONITOR monitor, const std::vector<int>& indexSet, const std::map<HMONITOR, winrt::com_ptr<IZoneWindow>>& zoneWindowMap) noexcept;
    bool MoveWindowIntoZoneByDirection(HMONITOR monitor, HWND window, DWORD vkCode, bool cycle, const std::map<HMONITOR, winrt::com_ptr<IZoneWindow>>& zoneWindowMap);

private:
    class WindowMoveHandlerPrivate* pimpl;
};