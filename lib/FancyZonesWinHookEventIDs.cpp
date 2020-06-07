#include "pch.h"

#include <mutex>

#include "FancyZonesWinHookEventIDs.h"

UINT WM_PRIV_LOCATIONCHANGE;
UINT WM_PRIV_NAMECHANGE;
UINT WM_PRIV_WINDOWCREATED;

std::once_flag init_flag;

void InitializeWinhookEventIds()
{
    std::call_once(init_flag, [&] {
        WM_PRIV_LOCATIONCHANGE = RegisterWindowMessage(L"{d56c5ee7-58e5-481c-8c4f-8844cf4d0347}");
        WM_PRIV_NAMECHANGE = RegisterWindowMessage(L"{b7b30c61-bfa0-4d95-bcde-fc4f2cbf6d76}");
        WM_PRIV_WINDOWCREATED = RegisterWindowMessage(L"{bdb10669-75da-480a-9ec4-eeebf09a02d7}");
    });
}
