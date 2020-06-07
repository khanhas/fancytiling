#pragma once

#include "ZoneWindow.h"
#include <inspectable.h>

namespace VirtualDesktopUtils
{
    // Ignore following API's:
    #define IAsyncCallback UINT
    #define IImmersiveMonitor UINT
    #define APPLICATION_VIEW_COMPATIBILITY_POLICY UINT
    #define IShellPositionerPriority UINT
    #define IApplicationViewOperation UINT
    #define APPLICATION_VIEW_CLOAK_TYPE UINT
    #define IApplicationViewPosition UINT

    // Computer\HKEY_LOCAL_MACHINE\SOFTWARE\Classes\Interface\{372E1D3B-38D3-42E4-A15B-8AB2B178F513}
    // Found with searching "IApplicationView"
    DECLARE_INTERFACE_IID_(IApplicationView, IInspectable, "372E1D3B-38D3-42E4-A15B-8AB2B178F513")
    {
        STDMETHOD(QueryInterface)
        (THIS_ REFIID riid, LPVOID FAR* ppvObject) PURE;
        STDMETHOD_(ULONG, AddRef)
        (THIS) PURE;
        STDMETHOD_(ULONG, Release)
        (THIS) PURE;

        /*** IInspectable methods ***/
        STDMETHOD(GetIids)
        (__RPC__out ULONG* iidCount, __RPC__deref_out_ecount_full_opt(*iidCount) IID** iids) PURE;
        STDMETHOD(GetRuntimeClassName)
        (__RPC__deref_out_opt HSTRING* className) PURE;
        STDMETHOD(GetTrustLevel)
        (__RPC__out TrustLevel* trustLevel) PURE;

        /*** IApplicationView methods ***/
        STDMETHOD(SetFocus)
        (THIS) PURE;
        STDMETHOD(SwitchTo)
        (THIS) PURE;
        STDMETHOD(TryInvokeBack)
        (THIS_ IAsyncCallback*)PURE; // Proc8
        STDMETHOD(GetThumbnailWindow)
        (THIS_ HWND*)PURE; // Proc9
        STDMETHOD(GetMonitor)
        (THIS_ IImmersiveMonitor**)PURE; // Proc10
        STDMETHOD(GetVisibility)
        (THIS_ int*)PURE; // Proc11
        STDMETHOD(SetCloak)
        (THIS_ APPLICATION_VIEW_CLOAK_TYPE, int)PURE; // Proc12
        STDMETHOD(GetPosition)
        (THIS_ REFIID, void**)PURE; // Proc13
        STDMETHOD(SetPosition)
        (THIS_ IApplicationViewPosition*)PURE; // Proc14
        STDMETHOD(InsertAfterWindow)
        (THIS_ HWND) PURE; // Proc15
        STDMETHOD(GetExtendedFramePosition)
        (THIS_ RECT*)PURE; // Proc16
        STDMETHOD(GetAppUserModelId)
        (THIS_ PWSTR*)PURE; // Proc17
        STDMETHOD(SetAppUserModelId)
        (THIS_ PCWSTR) PURE; // Proc18
        STDMETHOD(IsEqualByAppUserModelId)
        (THIS_ PCWSTR, int*)PURE; // Proc19
        STDMETHOD(GetViewState)
        (THIS_ UINT*)PURE; // Proc20
        STDMETHOD(SetViewState)
        (THIS_ UINT) PURE; // Proc21
        STDMETHOD(GetNeediness)
        (THIS_ int*)PURE; // Proc22
        STDMETHOD(GetLastActivationTimestamp)
        (THIS_ ULONGLONG*)PURE;
        STDMETHOD(SetLastActivationTimestamp)
        (THIS_ ULONGLONG) PURE;
        STDMETHOD(GetVirtualDesktopId)
        (THIS_ GUID*)PURE;
        STDMETHOD(SetVirtualDesktopId)
        (THIS_ REFGUID) PURE;
        STDMETHOD(GetShowInSwitchers)
        (THIS_ int*)PURE;
        STDMETHOD(SetShowInSwitchers)
        (THIS_ int)PURE;
        STDMETHOD(GetScaleFactor)
        (THIS_ int*)PURE;
        STDMETHOD(CanReceiveInput)
        (THIS_ BOOL*)PURE;
        STDMETHOD(GetCompatibilityPolicyType)
        (THIS_ APPLICATION_VIEW_COMPATIBILITY_POLICY*)PURE;
        STDMETHOD(SetCompatibilityPolicyType)
        (THIS_ APPLICATION_VIEW_COMPATIBILITY_POLICY) PURE;
        STDMETHOD(GetSizeConstraints)
        (THIS_ IImmersiveMonitor*, SIZE*, SIZE*)PURE;
        STDMETHOD(GetSizeConstraintsForDpi)
        (THIS_ UINT, SIZE*, SIZE*)PURE;
        STDMETHOD(SetSizeConstraintsForDpi)
        (THIS_ const UINT*, const SIZE*, const SIZE*)PURE;
        STDMETHOD(OnMinSizePreferencesUpdated)
        (THIS_ HWND) PURE;
        STDMETHOD(ApplyOperation)
        (THIS_ IApplicationViewOperation*)PURE;
        STDMETHOD(IsTray)
        (THIS_ BOOL*)PURE;
        STDMETHOD(IsInHighZOrderBand)
        (THIS_ BOOL*)PURE;
        STDMETHOD(IsSplashScreenPresented)
        (THIS_ BOOL*)PURE;
        STDMETHOD(Flash)
        (THIS) PURE;
        STDMETHOD(GetRootSwitchableOwner)
        (THIS_ IApplicationView**)PURE; // proc45
        STDMETHOD(EnumerateOwnershipTree)
        (THIS_ IObjectArray**)PURE; // proc46

        STDMETHOD(GetEnterpriseId)
        (THIS_ PWSTR*)PURE; // proc47
        STDMETHOD(IsMirrored)
        (THIS_ BOOL*)PURE; //

        STDMETHOD(Unknown1)
        (THIS_ int*)PURE;
        STDMETHOD(Unknown2)
        (THIS_ int*)PURE;
        STDMETHOD(Unknown3)
        (THIS_ int*)PURE;
        STDMETHOD(Unknown4)
        (THIS_ int)PURE;
        STDMETHOD(Unknown5)
        (THIS_ int*)PURE;
        STDMETHOD(Unknown6)
        (THIS_ int)PURE;
        STDMETHOD(Unknown7)
        (THIS) PURE;
        STDMETHOD(Unknown8)
        (THIS_ int*)PURE;
        STDMETHOD(Unknown9)
        (THIS_ int)PURE;
        STDMETHOD(Unknown10)
        (THIS_ int, int)PURE;
        STDMETHOD(Unknown11)
        (THIS_ int)PURE;
        STDMETHOD(Unknown12)
        (THIS_ SIZE*)PURE;
    };

    MIDL_INTERFACE("1841C6D7-4F9D-42C0-AF41-8747538F10E5")
    IApplicationViewCollection : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetViews(IObjectArray * *pArray) = 0;
        virtual HRESULT STDMETHODCALLTYPE GetViewsByZOrder(IObjectArray * *pArray) = 0;
        virtual HRESULT STDMETHODCALLTYPE GetViewsByAppUserModelId(BSTR pId, IObjectArray * *pArray) = 0;
        virtual HRESULT STDMETHODCALLTYPE GetViewForHwnd(HWND pHwnd, IApplicationView * *pView) = 0;
        virtual HRESULT STDMETHODCALLTYPE GetViewForApplication(void* pApplication, IApplicationView** pView) = 0;
        virtual HRESULT STDMETHODCALLTYPE GetViewForAppUserModelId(BSTR pId, IApplicationView * *pView) = 0;
        virtual HRESULT STDMETHODCALLTYPE GetViewInFocus(int* pView) = 0;
        virtual HRESULT STDMETHODCALLTYPE RefreshCollection() = 0;
        virtual HRESULT STDMETHODCALLTYPE RegisterForApplicationViewChanges(void* pListener, int* pCookie) = 0;
        virtual HRESULT STDMETHODCALLTYPE RegisterForApplicationViewPositionChanges(void* pListener, int* pCookie) = 0;
        virtual HRESULT STDMETHODCALLTYPE UnregisterForApplicationViewChanges(int pCookie) = 0;
    };

    MIDL_INTERFACE("FF72FFDD-BE7E-43FC-9C03-AD81681E88E4")
    IVirtualDesktop : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE IsViewVisible(
            IApplicationView * pView,
            int* pfVisible) = 0;

        virtual HRESULT STDMETHODCALLTYPE GetID(
            GUID * pGuid) = 0;
    };

    enum AdjacentDesktop
    {
        // Neighboring desktop on the left
        LeftDirection = 3,
        // Neighboring desktop on the right
        RightDirection = 4
    };

    MIDL_INTERFACE("F31574D6-B682-4CDC-BD56-1827860ABEC6")
    IVirtualDesktopManagerInternal : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetCount(UINT * pCount) = 0;
        virtual HRESULT STDMETHODCALLTYPE MoveViewToDesktop(IApplicationView * pView, IVirtualDesktop * pDesktop) = 0;
        virtual HRESULT STDMETHODCALLTYPE CanViewMoveDesktops(IApplicationView * pView, int* pfCanViewMoveDesktops) = 0;
        virtual HRESULT STDMETHODCALLTYPE GetCurrentDesktop(IVirtualDesktop * *desktop) = 0;
        virtual HRESULT STDMETHODCALLTYPE GetDesktops(IObjectArray * *ppDesktops) = 0;
        // Obtain a neighboring desktop relative to the specified, taking into account the direction of the
        virtual HRESULT STDMETHODCALLTYPE GetAdjacentDesktop(IVirtualDesktop * pDesktopReference, AdjacentDesktop uDirection, IVirtualDesktop * *ppAdjacentDesktop) = 0;
        virtual HRESULT STDMETHODCALLTYPE SwitchDesktop(IVirtualDesktop * pDesktop) = 0;
        virtual HRESULT STDMETHODCALLTYPE CreateDesktopW(IVirtualDesktop * *ppNewDesktop) = 0;
        // pFallbackDesktop - the desktop to which the transition will be made after removing the specified
        virtual HRESULT STDMETHODCALLTYPE RemoveDesktop(IVirtualDesktop * pRemove, IVirtualDesktop * pFallbackDesktop) = 0;
        virtual HRESULT STDMETHODCALLTYPE FindDesktop(GUID * desktopId, IVirtualDesktop * *ppDesktop) = 0;
    };

    bool GetWindowDesktopId(HWND topLevelWindow, GUID* desktopId);
    bool GetZoneWindowDesktopId(IZoneWindow* zoneWindow, GUID* desktopId);
    bool GetCurrentVirtualDesktopId(GUID* desktopId);
    bool GetVirtualDesktopIds(std::vector<GUID>& ids);
    bool GetVirtualDesktopIds(std::vector<std::wstring>& ids);
    HKEY GetVirtualDesktopsRegKey();
    void HandleVirtualDesktopUpdates(HWND window, UINT message, HANDLE terminateEvent);
    bool SwitchToVirtualDesktop(UINT index);
    bool MoveWindowToVirtualDesktop(HWND window, UINT index);
    IApplicationViewCollection* GetApplicationViewCollection(void);
    IVirtualDesktopManager* GetVirtualDesktopManager(void);
}
