// Copyright [year] <CopyrightOwner>

#include "DXUT.h"
#include "./dxut/Optional/DXUTgui.h"
#include "./dxut/Core/DXUTmisc.h"
#include "./dxut/Optional/DXUTSettingsDlg.h"
#include "./dxut/Optional/SDKmisc.h"
#include "./dxut/Optional/SDKmesh.h"
#include "./maze.h"
#include "./resource.h"

// Global variables.
CDXUTDialogResourceManager  g_DialogResourceManager;
CDXUTTextHelper*            g_pTxtHelper = NULL;
CDXUTDialog                 g_Start;

// Direct3D 9 resources.
ID3DXFont*                  g_pFont9 = NULL;
ID3DXSprite*                g_pSprite9 = NULL;
D3DXHANDLE                  g_hmWorldViewProjection;
D3DXHANDLE                  g_hmWorld;
D3DXHANDLE                  g_hfTime;

Maze*                       maze;

// Forward declarations.
LRESULT CALLBACK MsgProc               (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext);  // NOLINT
void    CALLBACK OnKeyboard            (UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext);  // NOLINT
void    CALLBACK OnFrameMove           (double fTime, float fElapsedTime, void* pUserContext);  // NOLINT
bool    CALLBACK ModifyDeviceSettings  (DXUTDeviceSettings* pDeviceSettings, void* pUserContext);  // NOLINT
bool    CALLBACK IsD3D9DeviceAcceptable(D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext);  // NOLINT
HRESULT CALLBACK OnD3D9CreateDevice    (IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);  // NOLINT
HRESULT CALLBACK OnD3D9ResetDevice     (IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);  // NOLINT
void    CALLBACK OnD3D9FrameRender     (IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext);  // NOLINT
void    CALLBACK OnD3D9LostDevice      (void* pUserContext);  // NOLINT
void    CALLBACK OnD3D9DestroyDevice   (void* pUserContext);  // NOLINT

void InitApp();
void RenderText();

// Entry point to the program. Initializes everything and goes into a message
// processing loop. Idle time is used to render the scene.
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPWSTR lpCmdLine, int nCmdShow) {
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    // DXUT will create and use the best device (either D3D9 or D3D10) that is
    // available on the system depending on which D3D callbacks are set below

    // Set DXUT callbacks
    DXUTSetCallbackMsgProc(MsgProc);
    DXUTSetCallbackKeyboard(OnKeyboard);
    DXUTSetCallbackFrameMove(OnFrameMove);
    DXUTSetCallbackDeviceChanging(ModifyDeviceSettings);

    DXUTSetCallbackD3D9DeviceAcceptable(IsD3D9DeviceAcceptable);
    DXUTSetCallbackD3D9DeviceCreated(OnD3D9CreateDevice);
    DXUTSetCallbackD3D9DeviceReset(OnD3D9ResetDevice);
    DXUTSetCallbackD3D9DeviceLost(OnD3D9LostDevice);
    DXUTSetCallbackD3D9DeviceDestroyed(OnD3D9DestroyDevice);
    DXUTSetCallbackD3D9FrameRender(OnD3D9FrameRender);

    maze = new Maze();
    InitApp();
    DXUTInit(true, true, NULL);
    DXUTSetCursorSettings(true, true);
    DXUTCreateWindow(L"SimpleSample");
    DXUTCreateDevice(true, 640, 480);

    DXUTMainLoop();  // Enter into the DXUT render loop
    return DXUTGetExitCode();
}


// Initialize the app.
void InitApp() {
    g_Start.Init(&g_DialogResourceManager);
}


// Render the help and statistics text. This function uses the ID3DXFont
// interface for efficient text rendering.
void RenderText() {
    g_pTxtHelper->Begin();
    g_pTxtHelper->SetInsertionPos(5, 5);
    g_pTxtHelper->SetForegroundColor(D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));
    g_pTxtHelper->DrawTextLine(DXUTGetFrameStats(DXUTIsVsyncEnabled()));
    g_pTxtHelper->DrawTextLine(DXUTGetDeviceStats());

    const D3DXVECTOR3 * pVec3 = maze->GetCameraEyePt();
    WCHAR s[256];
    swprintf_s(s, 64, L"Eye At: %.2f, %.2f, %.2f", pVec3->x, pVec3->y, pVec3->z);
    g_pTxtHelper->DrawTextLine(s);
    pVec3 = maze->GetCameraLookAtPt();
    swprintf_s(s, 64, L"Look at: %.2f, %.2f, %.2f", pVec3->x, pVec3->y, pVec3->z);
    g_pTxtHelper->DrawTextLine(s);
    g_pTxtHelper->End();
}


// Rejects any D3D9 devices that aren't acceptable to the app by returning false
bool CALLBACK IsD3D9DeviceAcceptable(D3DCAPS9* pCaps, D3DFORMAT AdapterFormat,
                                     D3DFORMAT BackBufferFormat, bool bWindowed,
                                     void* pUserContext ) {
    // Skip backbuffer formats that don't support alpha blending
    IDirect3D9* pD3D = DXUTGetD3D9Object();
    if (FAILED(pD3D->CheckDeviceFormat(pCaps->AdapterOrdinal, pCaps->DeviceType,
                                       AdapterFormat,
                                       D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING,
                                       D3DRTYPE_TEXTURE, BackBufferFormat)))
        return false;

    // No fallback defined by this app, so reject any device that
    // doesn't support at least ps2.0
    if (pCaps->PixelShaderVersion < D3DPS_VERSION(2, 0))
        return false;

    return true;
}


// Called right before creating a D3D9 or D3D10 device, allowing the app to
// modify the device settings as needed.
bool CALLBACK ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings,
                                   void* pUserContext) {
    if (pDeviceSettings->ver == DXUT_D3D9_DEVICE) {
        IDirect3D9* pD3D = DXUTGetD3D9Object();
        D3DCAPS9 Caps;
        pD3D->GetDeviceCaps(pDeviceSettings->d3d9.AdapterOrdinal,
            pDeviceSettings->d3d9.DeviceType, &Caps);

        // If device doesn't support HW T&L or doesn't support 1.1 vertex
        // shaders in HW then switch to SWVP.
        if ((Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ) == 0 ||
            Caps.VertexShaderVersion < D3DVS_VERSION(1, 1)) {
            pDeviceSettings->d3d9.BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;  // NOLINT
        }

        // Debugging vertex shaders requires either REF or software vertex
        // processing and debugging pixel shaders requires REF.
    }

    // For the first device created if its a REF device, optionally display a
    // warning dialog box.
    static bool s_bFirstTime = true;
    if (s_bFirstTime) {
        s_bFirstTime = false;
        if ((DXUT_D3D9_DEVICE == pDeviceSettings->ver &&
            pDeviceSettings->d3d9.DeviceType == D3DDEVTYPE_REF) ||
            (DXUT_D3D10_DEVICE == pDeviceSettings->ver &&
              pDeviceSettings->d3d10.DriverType == D3D10_DRIVER_TYPE_REFERENCE))
            DXUTDisplaySwitchingToREFWarning(pDeviceSettings->ver);
    }

    return true;
}


// Create any D3D9 resources that will live through a device reset
// (D3DPOOL_MANAGED) and aren't tied to the back buffer size.
HRESULT CALLBACK OnD3D9CreateDevice(IDirect3DDevice9* pd3dDevice,
    const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext) {

    HRESULT hr;

    V_RETURN(g_DialogResourceManager.OnD3D9CreateDevice(pd3dDevice));

    V_RETURN(D3DXCreateFont(pd3dDevice, 15, 0, FW_BOLD, 1, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, L"Arial", &g_pFont9));

    DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE;

#ifdef DEBUG_VS
    dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
#endif
#ifdef DEBUG_PS
    dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
#endif
#ifdef D3DXFX_LARGEADDRESS_HANDLE
    dwShaderFlags |= D3DXFX_LARGEADDRESSAWARE;
#endif

    return S_OK;
}


// Create any D3D9 resources that won't live through a device reset
// (D3DPOOL_DEFAULT) or that are tied to the back buffer size.
HRESULT CALLBACK OnD3D9ResetDevice(IDirect3DDevice9* pd3dDevice,
                                   const D3DSURFACE_DESC* pBackBufferSurfaceDesc, // NOLINT
                                   void* pUserContext) {
    HRESULT hr;

    V_RETURN(g_DialogResourceManager.OnD3D9ResetDevice());
    if (g_pFont9) V_RETURN(g_pFont9->OnResetDevice());
    V_RETURN(D3DXCreateSprite(pd3dDevice, &g_pSprite9));
    g_pTxtHelper = new CDXUTTextHelper(g_pFont9, g_pSprite9, NULL, NULL, 15);
    g_Start.SetLocation(0, 0);
    g_Start.SetSize(640, 480);

    maze->OnResetDevice(pd3dDevice, pBackBufferSurfaceDesc, pUserContext);
    return S_OK;
}


// Handle updates to the scene. This is called regardless of which D3D API is
// used.
void CALLBACK OnFrameMove(double fTime, float fElapsedTime,
                          void* pUserContext) {
    maze->OnFrameMove(fTime, fElapsedTime, pUserContext);
}


// Render the scene using the D3D9 device.
void CALLBACK OnD3D9FrameRender(IDirect3DDevice9* pd3dDevice, double fTime,
                                float fElapsedTime, void* pUserContext) {
    HRESULT hr;
    // Clear the render target and the zbuffer
    V(pd3dDevice->Clear(0,
        NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 45, 50, 170),
        1.0f, 0));

    // Render the scene
    if (SUCCEEDED(pd3dDevice->BeginScene())) {
        // These events are to help PIX identify what the code is doing
        DXUT_BeginPerfEvent(DXUT_PERFEVENTCOLOR, L"HUD / Stats");
        RenderText();
        maze->Render(pd3dDevice);
        DXUT_EndPerfEvent();
        V(pd3dDevice->EndScene());
    }
}


// Handle messages to the application
LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                         bool* pbNoFurtherProcessing, void* pUserContext) {
    // Pass messages to dialog resource manager calls so GUI state is updated
    // correctly
    *pbNoFurtherProcessing = g_DialogResourceManager.MsgProc(hWnd, uMsg, wParam, lParam);  // NOLINT

    if (*pbNoFurtherProcessing)
        return 0;

    maze->MsgProc(hWnd, uMsg, wParam, lParam);
    return 0;
}


// Handle key presses.
void CALLBACK OnKeyboard(UINT nChar, bool bKeyDown, bool bAltDown,
                         void* pUserContext) {
}


// Release D3D9 resources created in the OnD3D9ResetDevice callback.
void CALLBACK OnD3D9LostDevice(void* pUserContext) {
    g_DialogResourceManager.OnD3D9LostDevice();
    if (g_pFont9) g_pFont9->OnLostDevice();
    SAFE_RELEASE(g_pSprite9);
    SAFE_DELETE(g_pTxtHelper);
    maze->OnD3D9LostDevice();
}


// Release D3D9 resources created in the OnD3D9CreateDevice callback.
void CALLBACK OnD3D9DestroyDevice(void* pUserContext) {
    g_DialogResourceManager.OnD3D9DestroyDevice();
    SAFE_RELEASE(g_pFont9);
    SAFE_DELETE(maze);
}
