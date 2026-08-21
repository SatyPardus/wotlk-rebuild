#include "gx/d3d/CGxDeviceD3d.hpp"
#include "gx/Blit.hpp"
#include "gx/CGxBatch.hpp"
#include "gx/texture/CGxTex.hpp"
#include "math/Utils.hpp"
#include <algorithm>
#include <DirectXMath.h>

int32_t CGxDeviceD3d::s_clientAdjustWidth;
int32_t CGxDeviceD3d::s_clientAdjustHeight;
float CGxDeviceD3d::s_normalizeNormals;
D3DLIGHT9 CGxDeviceD3d::s_d3dLight;
D3DMATERIAL9 CGxDeviceD3d::s_d3dMaterial;

D3DCMPFUNC CGxDeviceD3d::s_cmpFunc[] = {
    D3DCMP_LESSEQUAL,
    D3DCMP_EQUAL,
    D3DCMP_GREATEREQUAL,
    D3DCMP_LESS,
};

D3DCULL CGxDeviceD3d::s_cullMode[] = {
    D3DCULL_NONE,
    D3DCULL_CW,
    D3DCULL_CCW,
};

D3DBLEND CGxDeviceD3d::s_dstBlend[] = {
    D3DBLEND_ZERO,              // GxBlend_Opaque
    D3DBLEND_ZERO,              // GxBlend_AlphaKey
    D3DBLEND_INVSRCALPHA,       // GxBlend_Alpha
    D3DBLEND_ONE,               // GxBlend_Add
    D3DBLEND_ZERO,              // GxBlend_Mod
    D3DBLEND_SRCCOLOR,          // GxBlend_Mod2x
    D3DBLEND_ONE,               // GxBlend_ModAdd
    D3DBLEND_ONE,               // GxBlend_InvSrcAlphaAdd
    D3DBLEND_ZERO,              // GxBlend_InvSrcAlphaOpaque
    D3DBLEND_ZERO,              // GxBlend_SrcAlphaOpaque
    D3DBLEND_ONE,               // GxBlend_NoAlphaAdd
    D3DBLEND_INVBLENDFACTOR,    // GxBlend_ConstantAlpha
};

D3DCUBEMAP_FACES CGxDeviceD3d::s_faceTypes[] = {
    D3DCUBEMAP_FACE_POSITIVE_X,
    D3DCUBEMAP_FACE_NEGATIVE_X,
    D3DCUBEMAP_FACE_POSITIVE_Y,
    D3DCUBEMAP_FACE_NEGATIVE_Y,
    D3DCUBEMAP_FACE_POSITIVE_Z,
    D3DCUBEMAP_FACE_NEGATIVE_Z,
};

D3DTEXTUREFILTERTYPE CGxDeviceD3d::s_filterModes[GxTexFilters_Last][3] = {
    // Min, Mag, Mip
    { D3DTEXF_POINT,    D3DTEXF_POINT,  D3DTEXF_NONE    },  // GxTex_Nearest
    { D3DTEXF_LINEAR,   D3DTEXF_LINEAR, D3DTEXF_NONE    },  // GxTex_Linear
    { D3DTEXF_POINT,    D3DTEXF_POINT,  D3DTEXF_POINT   },  // GxTex_NearestMipNearest
    { D3DTEXF_LINEAR,   D3DTEXF_LINEAR, D3DTEXF_POINT   },  // GxTex_LinearMipNearest
    { D3DTEXF_LINEAR,   D3DTEXF_LINEAR, D3DTEXF_LINEAR  },  // GxTex_LinearMipLinear
    { D3DTEXF_LINEAR,   D3DTEXF_LINEAR, D3DTEXF_LINEAR  },  // GxTex_Anisotropic
};

uint32_t CGxDeviceD3d::s_gxAttribToD3dAttribSize[] = {
    4,                      // type 0
    4,                      // type 1
    4,                      // type 2
    8,                      // type 3
    12,                     // type 4
    4,                      // type 5
    4,                      // type 6
};

D3DDECLTYPE CGxDeviceD3d::s_gxAttribToD3dAttribType[] = {
    D3DDECLTYPE_D3DCOLOR,   // type 0
    D3DDECLTYPE_UBYTE4,     // type 1
    D3DDECLTYPE_UBYTE4N,    // type 2
    D3DDECLTYPE_FLOAT2,     // type 3
    D3DDECLTYPE_FLOAT3,     // type 4
    D3DDECLTYPE_SHORT2,     // type 5
    D3DDECLTYPE_FLOAT1,     // type 6
};

D3DDECLUSAGE CGxDeviceD3d::s_gxAttribToD3dAttribUsage[] = {
    D3DDECLUSAGE_POSITION,      // GxVA_Position
    D3DDECLUSAGE_BLENDWEIGHT,   // GxVA_BlendWeight
    D3DDECLUSAGE_BLENDINDICES,  // GxVA_BlendIndices
    D3DDECLUSAGE_NORMAL,        // GxVA_Normal
    D3DDECLUSAGE_COLOR,         // GxVA_Color0
    D3DDECLUSAGE_COLOR,         // GxVA_Color1
    D3DDECLUSAGE_TEXCOORD,      // GxVA_TexCoord0
    D3DDECLUSAGE_TEXCOORD,      // GxVA_TexCoord1
    D3DDECLUSAGE_TEXCOORD,      // GxVA_TexCoord2
    D3DDECLUSAGE_TEXCOORD,      // GxVA_TexCoord3
    D3DDECLUSAGE_TEXCOORD,      // GxVA_TexCoord4
    D3DDECLUSAGE_TEXCOORD,      // GxVA_TexCoord5
    D3DDECLUSAGE_TEXCOORD,      // GxVA_TexCoord6
    D3DDECLUSAGE_TEXCOORD,      // GxVA_TexCoord7
};

uint32_t CGxDeviceD3d::s_gxAttribToD3dAttribUsageIndex[] = {
    0,                          // GxVA_Position
    0,                          // GxVA_BlendWeight
    0,                          // GxVA_BlendIndices
    0,                          // GxVA_Normal
    0,                          // GxVA_Color0
    1,                          // GxVA_Color1
    0,                          // GxVA_TexCoord0
    1,                          // GxVA_TexCoord1
    2,                          // GxVA_TexCoord2
    3,                          // GxVA_TexCoord3
    4,                          // GxVA_TexCoord4
    5,                          // GxVA_TexCoord5
    6,                          // GxVA_TexCoord6
    7,                          // GxVA_TexCoord7
};

D3DFORMAT CGxDeviceD3d::s_GxFormatToD3dFormat[] = {
    D3DFMT_R5G6B5,      // Fmt_Rgb565
    D3DFMT_X8R8G8B8,    // Fmt_ArgbX888
    D3DFMT_A8R8G8B8,    // Fmt_Argb8888
    D3DFMT_A2R10G10B10, // Fmt_Argb2101010
    D3DFMT_D16,         // Fmt_Ds160
    D3DFMT_D24X8,       // Fmt_Ds24X
    D3DFMT_D24S8,       // Fmt_Ds248
    D3DFMT_D32,         // Fmt_Ds320
};

D3DFORMAT CGxDeviceD3d::s_GxTexFmtToD3dFmt[] = {
    D3DFMT_UNKNOWN,     // GxTex_Unknown
    D3DFMT_A8B8G8R8,    // GxTex_Abgr8888
    D3DFMT_A8R8G8B8,    // GxTex_Argb8888
    D3DFMT_A4R4G4B4,    // GxTex_Argb4444
    D3DFMT_A1R5G5B5,    // GxTex_Argb1555
    D3DFMT_R5G6B5,      // GxTex_Rgb565
    D3DFMT_DXT1,        // GxTex_Dxt1
    D3DFMT_DXT3,        // GxTex_Dxt3
    D3DFMT_DXT5,        // GxTex_Dxt5
    D3DFMT_V8U8,        // GxTex_Uv88
    D3DFMT_G16R16F,     // GxTex_Gr1616F
    D3DFMT_R32F,        // GxTex_R32F
    D3DFMT_D24X8,       // GxTex_D24X8
};

EGxTexFormat CGxDeviceD3d::s_GxTexFmtToUse[] = {
    GxTex_Unknown,
    GxTex_Abgr8888,
    GxTex_Argb8888,
    GxTex_Argb4444,
    GxTex_Argb1555,
    GxTex_Rgb565,
    GxTex_Dxt1,
    GxTex_Dxt3,
    GxTex_Dxt5,
    GxTex_Uv88,
    GxTex_Gr1616F,
    GxTex_R32F,
    GxTex_D24X8,
};

D3DPRIMITIVETYPE CGxDeviceD3d::s_primitiveConversion[] = {
    D3DPT_POINTLIST,    // GxPrim_Points
    D3DPT_LINELIST,     // GxPrim_Lines
    D3DPT_LINESTRIP,    // GxPrim_LineStrip
    D3DPT_TRIANGLELIST, // GxPrim_Triangles
    D3DPT_TRIANGLESTRIP, // GxPrim_TriangleStrip
    D3DPT_TRIANGLEFAN,  // GxPrim_TriangleFan
};

D3DBLEND CGxDeviceD3d::s_srcBlend[] = {
    D3DBLEND_ONE,           // GxBlend_Opaque
    D3DBLEND_ONE,           // GxBlend_AlphaKey
    D3DBLEND_SRCALPHA,      // GxBlend_Alpha
    D3DBLEND_SRCALPHA,      // GxBlend_Add
    D3DBLEND_DESTCOLOR,     // GxBlend_Mod
    D3DBLEND_DESTCOLOR,     // GxBlend_Mod2x
    D3DBLEND_DESTCOLOR,     // GxBlend_ModAdd
    D3DBLEND_INVSRCALPHA,   // GxBlend_InvSrcAlphaAdd
    D3DBLEND_INVSRCALPHA,   // GxBlend_InvSrcAlphaOpaque
    D3DBLEND_SRCALPHA,      // GxBlend_SrcAlphaOpaque
    D3DBLEND_ONE,           // GxBlend_NoAlphaAdd
    D3DBLEND_BLENDFACTOR,   // GxBlend_ConstantAlpha
};

EGxTexFormat CGxDeviceD3d::s_tolerableTexFmtMapping[] = {
    GxTex_Unknown,      // GxTex_Unknown
    GxTex_Argb4444,     // GxTex_Abgr8888
    GxTex_Argb4444,     // GxTex_Argb8888
    GxTex_Argb4444,     // GxTex_Argb4444
    GxTex_Argb4444,     // GxTex_Argb1555
    GxTex_Argb4444,     // GxTex_Rgb565
    GxTex_Dxt1,         // GxTex_Dxt1
    GxTex_Dxt3,         // GxTex_Dxt3
    GxTex_Dxt5,         // GxTex_Dxt5
    GxTex_Uv88,         // GxTex_Uv88
    GxTex_Gr1616F,      // GxTex_Gr1616F
    GxTex_R32F,         // GxTex_R32F
    GxTex_D24X8,        // GxTex_D24X8
};

D3DTEXTUREADDRESS CGxDeviceD3d::s_wrapModes[] = {
    D3DTADDRESS_CLAMP,  // GxTex_Clamp
    D3DTADDRESS_WRAP,   // GxTex_Wrap
};

D3DTEXTUREOP CGxDeviceD3d::s_texOp[] {
    D3DTOP_MODULATE,          // 4
    D3DTOP_MODULATE2X,        // 5
    D3DTOP_ADD,               // 7
    D3DTOP_SELECTARG2,        // 3
    D3DTOP_BLENDCURRENTALPHA, // 16 (0x10)
    D3DTOP_BLENDDIFFUSEALPHA  // 12 (0x0C)
};

int32_t CGxDeviceD3d::s_texArgs[] {
    D3DTA_TEXTURE, D3DTA_CURRENT, // 2, 1
    D3DTA_TEXTURE, D3DTA_CURRENT, // 2, 1
    D3DTA_TEXTURE, D3DTA_CURRENT, // 2, 1
    D3DTA_TEXTURE, D3DTA_CURRENT, // 2, 1
    D3DTA_CURRENT, D3DTA_TEXTURE, // 1, 2
    D3DTA_TEXTURE, D3DTA_CURRENT  // 2, 1
};

// OFFSET: 0x68EB20
ATOM WindowClassCreate() {
    auto instance = GetModuleHandle(nullptr);

    WNDCLASSEX wc = { 0 };

    wc.cbSize = sizeof(wc);
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = CGxDeviceD3d::WindowProcD3d;
    wc.hInstance = instance;
    wc.lpszClassName = TEXT("GxWindowClassD3d");

    wc.hIcon = static_cast<HICON>(LoadImage(instance, TEXT("BlizzardIcon.ico"), 1u, 0, 0, 0x40));
    wc.hCursor = LoadCursor(instance, TEXT("BlizzardCursor.cur"));

    if (!wc.hCursor) {
        wc.hCursor = LoadCursor(instance, IDC_ARROW);
    }

    return RegisterClassEx(&wc);
}

// OFFSET: 0x68ED80
int32_t CGxDeviceD3d::ILoadD3dLib(HINSTANCE& d3dLib, LPDIRECT3D9& d3d) {
    d3dLib = nullptr;
    d3d = nullptr;

    d3dLib = LoadLibrary(TEXT("d3d9.dll"));

    typedef LPDIRECT3D9 (WINAPI *DIRECT3DCREATE9)(UINT SDKVersion);

    if (d3dLib) {
        auto d3dCreateProc = reinterpret_cast<DIRECT3DCREATE9>(GetProcAddress(d3dLib, "Direct3DCreate9"));

        if (d3dCreateProc) {
            d3d = d3dCreateProc(D3D_SDK_VERSION);

            if (d3d) {
                return 1;
            }

            CGxDevice::Log("CGxDeviceD3d::ILoadD3dLib(): unable to d3dCreateProc()");
        } else {
            CGxDevice::Log("CGxDeviceD3d::ILoadD3dLib(): unable to GetProcAddress()");
        }
    } else {
        CGxDevice::Log("CGxDeviceD3d::ILoadD3dLib(): unable to LoadLibrary()");
    }

    CGxDeviceD3d::IUnloadD3dLib(d3dLib, d3d);

    return 0;
}

// OFFSET: 0x68E140
void CGxDeviceD3d::IUnloadD3dLib(HINSTANCE& d3dLib, LPDIRECT3D9& d3d) {
    if (d3d) {
        d3d->Release();
    }

    if (d3dLib) {
        FreeLibrary(d3dLib);
    }
}

// OFFSET: 0x6A0360
LRESULT CALLBACK CGxDeviceD3d::WindowProcD3d(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    auto device = reinterpret_cast<CGxDeviceD3d*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    switch (uMsg) {
    case WM_CREATE: {
        auto lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(lpcs->lpCreateParams));

        return 0;
    }

    case WM_DESTROY: {
        device->DeviceWM(GxWM_Destroy, 0, 0);

        return 0;
    }

    case WM_SIZE: {
        CRect windowRect = {
            0.0f,
            0.0f,
            static_cast<float>(HIWORD(lParam)),
            static_cast<float>(LOWORD(lParam))
        };

        int32_t resizeType = 0;
        if (wParam == SIZE_MINIMIZED) {
            resizeType = 1;
        } else if (wParam == SIZE_MAXHIDE) {
            resizeType = 2;
        }

        device->DeviceWM(GxWM_Size, reinterpret_cast<uintptr_t>(&windowRect), resizeType);

        break;
    }

    case WM_ACTIVATE: {
        if (wParam == WA_INACTIVE && !device->IDevIsWindowed()) {
            CRect windowRect = { 0.0f, 0.f, 0.0f, 0.0f };
            device->DeviceWM(GxWM_Size, reinterpret_cast<uintptr_t>(&windowRect), 1);
        } else if (wParam == WA_ACTIVE && !device->IDevIsWindowed()) {
            CRect windowRect;
            device->CapsWindowSizeInScreenCoords(windowRect);
            device->DeviceWM(GxWM_Size, reinterpret_cast<uintptr_t>(&windowRect), 3);
        }

        break;
    }

    case WM_SETFOCUS: {
        device->DeviceWM(GxWM_SetFocus, 0, 0);

        return 0;
    }

    case WM_KILLFOCUS: {
        device->DeviceWM(GxWM_KillFocus, 0, 0);

        return 0;
    }

    case WM_PAINT: {
        PAINTSTRUCT paint;
        BeginPaint(hWnd, &paint);
        EndPaint(hWnd, &paint);

        return 0;
    }

    case WM_ERASEBKGND: {
        return 0;
    }

    case WM_SETCURSOR: {
        if (device) {
            if (device->m_d3dDevice && LOWORD(lParam) == HTCLIENT) {
                SetCursor(nullptr);
                BOOL show = device->m_cursorVisible && device->m_hwCursor ? TRUE : FALSE;
                device->m_d3dDevice->ShowCursor(show);
                return 1;
            }
        }
        break;
    }

    case WM_DISPLAYCHANGE: {
        if (!device->IDevIsWindowed()) {
            CRect windowRect = {
                0.0f,
                0.0f,
                static_cast<float>(HIWORD(lParam)),
                static_cast<float>(LOWORD(lParam))
            };
            device->DeviceWM(GxWM_DisplayChange, reinterpret_cast<uintptr_t>(&windowRect), 0);
        }
        break;
    }

    case WM_SYSCOMMAND: {
        break;
    }

    case WM_SIZING: {
        auto windowRect = reinterpret_cast<RECT*>(lParam);

        if (windowRect->right - windowRect->left <= 0 || windowRect->bottom - windowRect->top <= 0) {
            return 0;
        }

        SizingRect r = {
            windowRect->top,
            windowRect->left,
            windowRect->bottom,
            windowRect->right
        };

        switch (wParam) {
        case WMSZ_LEFT:
            SizingMinWidthMoveLeft(r);
            SizingAspectMoveTop(r);
            break;

        case WMSZ_RIGHT:
            SizingMinWidthMoveRight(r);
            SizingAspectMoveBottom(r);
            break;

        case WMSZ_TOP:
            SizingMinHeightMoveTop(r);
            SizingAspectMoveLeft(r);
            break;

        case WMSZ_TOPLEFT:
            SizingMinHeightMoveTop(r);
            SizingMinWidthMoveLeft(r);
            SizingAspectMoveTop(r);
            SizingAspectMoveLeft(r);
            break;

        case WMSZ_TOPRIGHT:
            SizingMinHeightMoveTop(r);
            SizingMinWidthMoveRight(r);
            SizingAspectMoveTop(r);
            SizingAspectMoveRight(r);
            break;

        case WMSZ_BOTTOM:
            SizingMinHeightMoveBottom(r);
            SizingAspectMoveRight(r);
            break;

        case WMSZ_BOTTOMLEFT:
            SizingMinHeightMoveBottom(r);
            SizingMinWidthMoveLeft(r);
            SizingAspectMoveLeft(r);
            SizingAspectMoveBottom(r);
            break;

        case WMSZ_BOTTOMRIGHT:
            SizingMinHeightMoveBottom(r);
            SizingMinWidthMoveRight(r);
            SizingAspectMoveBottom(r);
            SizingAspectMoveRight(r);
            break;

        default:
            break;
        }

        windowRect->left = r.left;
        windowRect->top = r.top;
        windowRect->right = r.right;
        windowRect->bottom = r.bottom;

        return 1;
    }

    default:
        break;
    }

    if (device && device->m_windowProc) {
        return device->m_windowProc(hWnd, uMsg, wParam, lParam);
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

CGxDeviceD3d::CGxDeviceD3d() : CGxDevice() {
    // TODO

    this->m_api = GxApi_D3d9;

    // TODO

    memset(this->m_deviceStates, 0xFF, sizeof(this->m_deviceStates));

    // TODO

    this->DeviceCreatePools();
    this->DeviceCreateStreamBufs();
}

char* CGxDeviceD3d::BufLock(CGxBuf* buf) {
    CGxDevice::BufLock(buf);
    return this->IBufLock(buf);
}

int32_t CGxDeviceD3d::BufUnlock(CGxBuf* buf, uint32_t size) {
    CGxDevice::BufUnlock(buf, size);
    this->IBufUnlock(buf);

    return 1;
}

void CGxDeviceD3d::BufData(CGxBuf* buf, const void* data, size_t size, uintptr_t offset) {
    CGxDevice::BufData(buf, data, size, offset);

    auto bufData = this->IBufLock(buf);
    memcpy(&bufData[offset], data, size);
    this->IBufUnlock(buf);
}

void CGxDeviceD3d::CapsWindowSize(CRect& dst) {
    dst = this->DeviceCurWindow();
}

void CGxDeviceD3d::CapsWindowSizeInScreenCoords(CRect& dst) {
    if (this->IDevIsWindowed()) {
        auto windowRect = this->DeviceCurWindow();

        POINT points[2];
        points[0].x = 0;
        points[0].y = 0;
        points[1].x = windowRect.maxX;
        points[1].y = windowRect.maxY;

        MapWindowPoints(this->m_hwnd, nullptr, points, 2);

        dst.minY = points[0].y;
        dst.minX = points[0].x;
        dst.maxY = points[1].y;
        dst.maxX = points[1].x;
    } else {
        dst = this->DeviceCurWindow();
    }
}

int32_t CGxDeviceD3d::CreatePoolAPI(CGxPool* pool) {
    if (pool->m_target == GxPoolTarget_Vertex) {
        pool->m_apiSpecific = this->ICreateD3dVB(pool->m_usage, pool->m_size);
    } else if (pool->m_target == GxPoolTarget_Index) {
        pool->m_apiSpecific = this->ICreateD3dIB(pool->m_usage, pool->m_size);
    }

    return 1;
}

int32_t CGxDeviceD3d::DeviceCreate(int32_t (*windowProc)(void* window, uint32_t message, uintptr_t wparam, intptr_t lparam), const CGxFormat& format) {
    this->m_ownhwnd = 1;

    // TODO gamma ramp

    this->m_hwndClass = WindowClassCreate();

    if (this->m_hwndClass) {
        if (this->ICreateD3d() && this->CGxDevice::DeviceCreate(windowProc, format)) {
            return 1;
        } else {
            // TODO
            // this->DeviceDestroy();
            return 0;
        }
    }

    // TODO CGxDevice::Log("CGxDeviceD3d::DeviceCreate(): WindowClassCreate() failed: %s", OsGetLastErrorStr());
    // TODO this->DeviceDestroy();

    return 0;
}

int32_t CGxDeviceD3d::DeviceSetFormat(const CGxFormat& format) {
    CGxDevice::Log("CGxDeviceD3d::DeviceSetFormat():");
    CGxDevice::Log(format);

    if (this->m_hwnd) {
        ShowWindow(this->m_hwnd, 0);
    }

    this->IDestroyD3dDevice();

    if (this->m_hwnd) {
        DestroyWindow(this->m_hwnd);
    }

    this->m_hwnd = nullptr;

    this->m_format = format;

    CGxFormat createFormat = format;

    if (this->ICreateWindow(createFormat) && this->ICreateD3dDevice(createFormat) && this->CGxDevice::DeviceSetFormat(format)) {
        this->ISetWindowFocus(1);
        return 1;
    } else {
        CGxDevice::Log("CGxDeviceD3d::DeviceSetFormat(): unable to set format!");
        this->IDestroyD3dDevice();
        if (this->m_hwnd) {
            DestroyWindow(this->m_hwnd);
        }
        this->m_hwnd = nullptr;
        return 0;
    }
}

void* CGxDeviceD3d::DeviceWindow() {
    return this->m_hwnd;
}

// OFFSET: 0x690230
void CGxDeviceD3d::DeviceWM(EGxWM wm, uintptr_t param1, uintptr_t param2) {
    switch (wm) {
    case GxWM_Size: {
        if (param2 == 1 || param2 == 2) {
            this->m_windowVisible = 0;
        } else {
            this->m_windowVisible = 1;

            auto& windowRect = *reinterpret_cast<CRect*>(param1);
            this->DeviceSetDefWindow(windowRect);

            if (this->m_d3dDevice && this->m_context) {
                this->IReleaseD3dResources(0);

                D3DPRESENT_PARAMETERS d3dpp;
                this->ISetPresentParms(d3dpp, this->m_format);

                if (SUCCEEDED(this->m_d3dDevice->Reset(&d3dpp))) {
                    this->IStateSetD3dDefaults();
                    this->ISetWindowFocus(true);

                    this->m_context = 1;
                    this->intF5C = 0;

                    // TODO this->ukn5();

                    this->m_needsReset = 1;

                    return;
                } else {
                    this->m_context = 0;
                }
            }

            this->m_needsReset = 1;
        }

        break;
    }
    case GxWM_DisplayChange:
        if (this->m_windowVisible) {
            auto& windowRect = *reinterpret_cast<CRect*>(param1);
            this->DeviceSetDefWindow(windowRect);
            this->m_needsReset = 1;
        }
        break;
    case GxWM_Destroy:
    case GxWM_KillFocus:
        this->ISetWindowFocus(false);
        break;
    case GxWM_SetFocus:
        this->ISetWindowFocus(true);
        break;
    default: {
    }
    }
}

void CGxDeviceD3d::Draw(CGxBatch* batch, int32_t indexed) {
    if (!this->m_context || this->intF5C) {
        return;
    }

    this->IStateSync();

    int32_t baseIndex = 0;
    if (!this->m_caps.int10) {
        baseIndex = this->m_primVertexFormatBuf[0]->m_index / this->m_primVertexFormatBuf[0]->m_itemSize;
    }

    if (indexed) {
        this->m_d3dDevice->DrawIndexedPrimitive(
            CGxDeviceD3d::s_primitiveConversion[batch->m_primType],
            baseIndex,
            batch->m_minIndex,
            batch->m_maxIndex - batch->m_minIndex + 1,
            batch->m_start + (this->m_primIndexBuf->m_index / 2),
            CGxDevice::PrimCalcCount(batch->m_primType, batch->m_count)
        );
    } else {
        this->m_d3dDevice->DrawPrimitive(
            CGxDeviceD3d::s_primitiveConversion[batch->m_primType],
            baseIndex,
            CGxDevice::PrimCalcCount(batch->m_primType, batch->m_count)
        );
    }
}

void CGxDeviceD3d::DsSet(EDeviceState state, uint32_t val) {
    if (this->m_deviceStates[state] == val) {
        return;
    }

    switch (state) {
    // TODO handle other device states

    case Ds_SrcBlend: {
        this->m_d3dDevice->SetRenderState(D3DRS_SRCBLEND, val);
        break;
    }

    case Ds_DstBlend: {
        this->m_d3dDevice->SetRenderState(D3DRS_DESTBLEND, val);
        break;
    }

    case Ds_TssMagFilter0:
    case Ds_TssMagFilter1:
    case Ds_TssMagFilter2:
    case Ds_TssMagFilter3:
    case Ds_TssMagFilter4:
    case Ds_TssMagFilter5:
    case Ds_TssMagFilter6:
    case Ds_TssMagFilter7:
    case Ds_TssMagFilter8:
    case Ds_TssMagFilter9:
    case Ds_TssMagFilter10:
    case Ds_TssMagFilter11:
    case Ds_TssMagFilter12:
    case Ds_TssMagFilter13:
    case Ds_TssMagFilter14:
    case Ds_TssMagFilter15: {
        auto tmu = state - Ds_TssMagFilter0;
        this->m_d3dDevice->SetSamplerState(tmu, D3DSAMP_MAGFILTER, val);

        break;
    }

    case Ds_TssMinFilter0:
    case Ds_TssMinFilter1:
    case Ds_TssMinFilter2:
    case Ds_TssMinFilter3:
    case Ds_TssMinFilter4:
    case Ds_TssMinFilter5:
    case Ds_TssMinFilter6:
    case Ds_TssMinFilter7:
    case Ds_TssMinFilter8:
    case Ds_TssMinFilter9:
    case Ds_TssMinFilter10:
    case Ds_TssMinFilter11:
    case Ds_TssMinFilter12:
    case Ds_TssMinFilter13:
    case Ds_TssMinFilter14:
    case Ds_TssMinFilter15: {
        auto tmu = state - Ds_TssMinFilter0;
        this->m_d3dDevice->SetSamplerState(tmu, D3DSAMP_MINFILTER, val);

        break;
    }

    case Ds_TssMipFilter0:
    case Ds_TssMipFilter1:
    case Ds_TssMipFilter2:
    case Ds_TssMipFilter3:
    case Ds_TssMipFilter4:
    case Ds_TssMipFilter5:
    case Ds_TssMipFilter6:
    case Ds_TssMipFilter7:
    case Ds_TssMipFilter8:
    case Ds_TssMipFilter9:
    case Ds_TssMipFilter10:
    case Ds_TssMipFilter11:
    case Ds_TssMipFilter12:
    case Ds_TssMipFilter13:
    case Ds_TssMipFilter14:
    case Ds_TssMipFilter15: {
        auto tmu = state - Ds_TssMipFilter0;
        this->m_d3dDevice->SetSamplerState(tmu, D3DSAMP_MIPFILTER, val);

        break;
    }

    case Ds_TssWrapU0:
    case Ds_TssWrapU1:
    case Ds_TssWrapU2:
    case Ds_TssWrapU3:
    case Ds_TssWrapU4:
    case Ds_TssWrapU5:
    case Ds_TssWrapU6:
    case Ds_TssWrapU7:
    case Ds_TssWrapU8:
    case Ds_TssWrapU9:
    case Ds_TssWrapU10:
    case Ds_TssWrapU11:
    case Ds_TssWrapU12:
    case Ds_TssWrapU13:
    case Ds_TssWrapU14:
    case Ds_TssWrapU15: {
        auto tmu = state - Ds_TssWrapU0;
        this->m_d3dDevice->SetSamplerState(tmu, D3DSAMP_ADDRESSU, val);

        break;
    }

    case Ds_TssWrapV0:
    case Ds_TssWrapV1:
    case Ds_TssWrapV2:
    case Ds_TssWrapV3:
    case Ds_TssWrapV4:
    case Ds_TssWrapV5:
    case Ds_TssWrapV6:
    case Ds_TssWrapV7:
    case Ds_TssWrapV8:
    case Ds_TssWrapV9:
    case Ds_TssWrapV10:
    case Ds_TssWrapV11:
    case Ds_TssWrapV12:
    case Ds_TssWrapV13:
    case Ds_TssWrapV14:
    case Ds_TssWrapV15: {
        auto tmu = state - Ds_TssWrapV0;
        this->m_d3dDevice->SetSamplerState(tmu, D3DSAMP_ADDRESSV, val);

        break;
    }

    case Ds_TssTTF0:
    case Ds_TssTTF1:
    case Ds_TssTTF2:
    case Ds_TssTTF3:
    case Ds_TssTTF4:
    case Ds_TssTTF5:
    case Ds_TssTTF6:
    case Ds_TssTTF7: {
        this->m_d3dDevice->SetTextureStageState(state - Ds_TssTTF0, D3DTSS_TEXTURETRANSFORMFLAGS, val);
        break;
    }

    case Ds_TssMaxAnisotropy0:
    case Ds_TssMaxAnisotropy1:
    case Ds_TssMaxAnisotropy2:
    case Ds_TssMaxAnisotropy3:
    case Ds_TssMaxAnisotropy4:
    case Ds_TssMaxAnisotropy5:
    case Ds_TssMaxAnisotropy6:
    case Ds_TssMaxAnisotropy7:
    case Ds_TssMaxAnisotropy8:
    case Ds_TssMaxAnisotropy9:
    case Ds_TssMaxAnisotropy10:
    case Ds_TssMaxAnisotropy11:
    case Ds_TssMaxAnisotropy12:
    case Ds_TssMaxAnisotropy13:
    case Ds_TssMaxAnisotropy14:
    case Ds_TssMaxAnisotropy15: {
        this->m_d3dDevice->SetSamplerState(state - Ds_TssMaxAnisotropy0, D3DSAMP_MAXANISOTROPY, val);
        break;
    }

    case Ds_TssTexCoordIndex0:
    case Ds_TssTexCoordIndex1:
    case Ds_TssTexCoordIndex2:
    case Ds_TssTexCoordIndex3:
    case Ds_TssTexCoordIndex4:
    case Ds_TssTexCoordIndex5:
    case Ds_TssTexCoordIndex6:
    case Ds_TssTexCoordIndex7: {
        this->m_d3dDevice->SetTextureStageState(state - Ds_TssTexCoordIndex0, D3DTSS_TEXCOORDINDEX, val);
        break;
    }

    case Ds_TssColorOp0:
    case Ds_TssColorOp1:
    case Ds_TssColorOp2:
    case Ds_TssColorOp3:
    case Ds_TssColorOp4:
    case Ds_TssColorOp5:
    case Ds_TssColorOp6:
    case Ds_TssColorOp7: {
        this->m_d3dDevice->SetTextureStageState(state - Ds_TssColorOp0, D3DTSS_COLOROP, val);
        break;
    }

    case Ds_TssAlphaOp0:
    case Ds_TssAlphaOp1:
    case Ds_TssAlphaOp2:
    case Ds_TssAlphaOp3:
    case Ds_TssAlphaOp4:
    case Ds_TssAlphaOp5:
    case Ds_TssAlphaOp6:
    case Ds_TssAlphaOp7: {
        this->m_d3dDevice->SetTextureStageState(state - Ds_TssAlphaOp0, D3DTSS_ALPHAOP, val);
        break;
    }

    case Ds_TssColorArg10:
    case Ds_TssColorArg11:
    case Ds_TssColorArg12:
    case Ds_TssColorArg13:
    case Ds_TssColorArg14:
    case Ds_TssColorArg15:
    case Ds_TssColorArg16:
    case Ds_TssColorArg17: {
        this->m_d3dDevice->SetTextureStageState(state - Ds_TssColorArg10, D3DTSS_COLORARG1, val);
        break;
    }

    case Ds_TssColorArg20:
    case Ds_TssColorArg21:
    case Ds_TssColorArg22:
    case Ds_TssColorArg23:
    case Ds_TssColorArg24:
    case Ds_TssColorArg25:
    case Ds_TssColorArg26:
    case Ds_TssColorArg27: {
        this->m_d3dDevice->SetTextureStageState(state - Ds_TssColorArg20, D3DTSS_COLORARG2, val);
        break;
    }

    case Ds_TssAlphaArg10:
    case Ds_TssAlphaArg11:
    case Ds_TssAlphaArg12:
    case Ds_TssAlphaArg13:
    case Ds_TssAlphaArg14:
    case Ds_TssAlphaArg15:
    case Ds_TssAlphaArg16:
    case Ds_TssAlphaArg17: {
        this->m_d3dDevice->SetTextureStageState(state - Ds_TssAlphaArg10, D3DTSS_ALPHAARG1, val);
        break;
    }

    case Ds_TssAlphaArg20:
    case Ds_TssAlphaArg21:
    case Ds_TssAlphaArg22:
    case Ds_TssAlphaArg23:
    case Ds_TssAlphaArg24:
    case Ds_TssAlphaArg25:
    case Ds_TssAlphaArg26:
    case Ds_TssAlphaArg27: {
        this->m_d3dDevice->SetTextureStageState(state - Ds_TssAlphaArg20, D3DTSS_ALPHAARG2, val);
        break;
    }

    case Ds_AlphaBlendEnable: {
        this->m_d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, val);
        break;
    }

    case Ds_AlphaTestEnable: {
        this->m_d3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, val);
        break;
    }

    case Ds_AlphaRef: {
        this->m_d3dDevice->SetRenderState(D3DRS_ALPHAREF, val);
        break;
    }

    case Ds_FogEnable: {
        this->m_d3dDevice->SetRenderState(D3DRS_FOGENABLE, val);
        break;
    }

    case Ds_ZWriteEnable: {
        this->m_d3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, val);
        break;
    }

    case Ds_ColorWriteEnable: {
        this->m_d3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, val);
        break;
    }

    case Ds_CullMode: {
        this->m_d3dDevice->SetRenderState(D3DRS_CULLMODE, val);
        break;
    }

    case Ds_ZFunc: {
        this->m_d3dDevice->SetRenderState(D3DRS_ZFUNC, val);
        break;
    }
    default: {
        SErrDisplayAppFatal("Error, unhandled EGxRenderState '%d'", state);
    }
    }

    this->m_deviceStates[state] = val;
}

char* CGxDeviceD3d::IBufLock(CGxBuf* buf) {
    if (!this->m_context) {
        // TODO
        return nullptr;
    }

    auto pool = buf->m_pool;
    uint32_t lockFlags = 0x0;

    if (pool->m_usage == GxPoolUsage_Stream) {
        auto v6 = buf->m_itemSize + pool->unk1C - 1 - (buf->m_itemSize + pool->unk1C - 1) % buf->m_itemSize;
        if (buf->m_size + v6 <= pool->m_size) {
            lockFlags = D3DLOCK_NOOVERWRITE;
            buf->m_index = v6;
            pool->unk1C = buf->m_size + v6;
        } else {
            lockFlags = D3DLOCK_DISCARD;
            pool->Discard();
            buf->m_index = 0;
            pool->unk1C = buf->m_size;
        }
    } else if (pool->m_usage == GxPoolUsage_Dynamic) {
        lockFlags = D3DLOCK_NOOVERWRITE;
    }

    if (!pool->m_apiSpecific) {
        this->CreatePoolAPI(pool);
    }

    if (!pool->m_apiSpecific) {
        // TODO
        return nullptr;
    }

    // Invalid target
    if (pool->m_target >= GxPoolTargets_Last) {
        return nullptr;
    }

    char* data = nullptr;
    HRESULT lockResult = S_OK;

    if (pool->m_target == GxPoolTarget_Vertex) {
        auto d3dBuf = static_cast<LPDIRECT3DVERTEXBUFFER9>(pool->m_apiSpecific);
        lockResult = d3dBuf->Lock(buf->m_index, buf->m_size, reinterpret_cast<void**>(&data), lockFlags);
    } else if (pool->m_target == GxPoolTarget_Index) {
        auto d3dBuf = static_cast<LPDIRECT3DINDEXBUFFER9>(pool->m_apiSpecific);
        lockResult = d3dBuf->Lock(buf->m_index, buf->m_size, reinterpret_cast<void**>(&data), lockFlags);
    }

    if (SUCCEEDED(lockResult)) {
        if (buf->m_size) {
            // TODO

            if (pool->m_usage == GxPoolUsage_Stream) {
                *data = 0;
            } else {
                *data = *data;
            }

            // TODO
        }
    } else {
        this->IBufUnlock(buf);

        // TODO
        return nullptr;
    }

    return data;
}

void CGxDeviceD3d::IBufUnlock(CGxBuf* buf) {
    // TODO

    auto pool = buf->m_pool;

    if (pool->m_target == GxPoolTarget_Vertex) {
        auto d3dBuf = static_cast<LPDIRECT3DVERTEXBUFFER9>(pool->m_apiSpecific);
        buf->unk1D = SUCCEEDED(d3dBuf->Unlock());
    } else if (pool->m_target == GxPoolTarget_Index) {
        auto d3dBuf = static_cast<LPDIRECT3DINDEXBUFFER9>(pool->m_apiSpecific);
        buf->unk1D = SUCCEEDED(d3dBuf->Unlock());
    } else {
        buf->unk1D = 1;
    }
}

int32_t CGxDeviceD3d::ICreateD3d() {
    if (CGxDeviceD3d::ILoadD3dLib(this->m_d3dLib, this->m_d3d) && SUCCEEDED(this->m_d3d->GetDeviceCaps(0, D3DDEVTYPE_HAL, &this->m_d3dCaps))) {
        if (this->m_desktopDisplayMode.Format != D3DFMT_UNKNOWN) {
            return 1;
        }

        D3DDISPLAYMODE displayMode;
        if (SUCCEEDED(this->m_d3d->GetAdapterDisplayMode(0, &displayMode))) {
            this->m_desktopDisplayMode.Width = displayMode.Width;
            this->m_desktopDisplayMode.Height = displayMode.Height;
            this->m_desktopDisplayMode.RefreshRate = displayMode.RefreshRate;
            this->m_desktopDisplayMode.Format = displayMode.Format;

            return 1;
        }
    }

    this->IDestroyD3d();

    return 0;
}

int32_t CGxDeviceD3d::ICreateD3dDevice(const CGxFormat& format) {
    // TODO stereoscopic setup

    auto hwTnL = format.hwTnL;
    if (hwTnL && (this->m_d3dCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0) {
        hwTnL = false;
    }
    this->m_d3dIsHwDevice = hwTnL;

    D3DPRESENT_PARAMETERS d3dpp;
    this->ISetPresentParms(d3dpp, format);

    uint32_t behaviorFlags = hwTnL
        ? D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE | D3DCREATE_FPU_PRESERVE
        : D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE;

    if (SUCCEEDED(this->m_d3d->CreateDevice(0, D3DDEVTYPE_HAL, this->m_hwnd, behaviorFlags, &d3dpp, &this->m_d3dDevice))) {
        // TODO

        this->m_devAdapterFormat = d3dpp.BackBufferFormat;
        this->m_context = 1;

        // TODO

        this->ISetCaps(format);

        // TODO logs

        this->IStateSetD3dDefaults();

        this->ICursorCreate(format);

        // TODO

        return 1;
    }

    this->m_d3dDevice = nullptr;
    return 0;
}

LPDIRECT3DINDEXBUFFER9 CGxDeviceD3d::ICreateD3dIB(EGxPoolUsage usage, uint32_t size) {
    uint32_t d3dUsage = this->m_d3dIsHwDevice ? D3DUSAGE_WRITEONLY : D3DUSAGE_SOFTWAREPROCESSING;
    D3DPOOL d3dPool = D3DPOOL_MANAGED;

    if (usage == GxPoolUsage_Dynamic || usage == GxPoolUsage_Stream) {
        d3dUsage |= D3DUSAGE_DYNAMIC;
        d3dPool = D3DPOOL_DEFAULT;
    }

    LPDIRECT3DINDEXBUFFER9 indexBuf = nullptr;

    if (SUCCEEDED(this->m_d3dDevice->CreateIndexBuffer(size, d3dUsage, D3DFMT_INDEX16, d3dPool, &indexBuf, nullptr))) {
        return indexBuf;
    }

    return nullptr;
}

LPDIRECT3DVERTEXBUFFER9 CGxDeviceD3d::ICreateD3dVB(EGxPoolUsage usage, uint32_t size) {
    uint32_t d3dUsage = this->m_d3dIsHwDevice ? D3DUSAGE_WRITEONLY : D3DUSAGE_SOFTWAREPROCESSING;
    D3DPOOL d3dPool = D3DPOOL_MANAGED;

    if (usage == GxPoolUsage_Dynamic || usage == GxPoolUsage_Stream) {
        d3dUsage |= D3DUSAGE_DYNAMIC;
        d3dPool = D3DPOOL_DEFAULT;
    }

    LPDIRECT3DVERTEXBUFFER9 vertexBuf = nullptr;

    if (SUCCEEDED(this->m_d3dDevice->CreateVertexBuffer(size, d3dUsage, 0, d3dPool, &vertexBuf, nullptr))) {
        return vertexBuf;
    }

    return nullptr;
}

LPDIRECT3DVERTEXDECLARATION9 CGxDeviceD3d::ICreateD3dVertexDecl(D3DVERTEXELEMENT9 elements[], uint32_t count) {
    if (this->m_primVertexFormat < GxVertexBufferFormats_Last) {
        for (int32_t i = 0; i < count; i++) {
            auto& element = elements[i];
            auto foo = 1;
        }

        if (!this->m_d3dVertexDecl[this->m_primVertexFormat]) {
            this->m_d3dDevice->CreateVertexDeclaration(elements, &this->m_d3dVertexDecl[this->m_primVertexFormat]);
        }

        return this->m_d3dVertexDecl[this->m_primVertexFormat];
    }

    // TODO new vertex buffer format

    return nullptr;
}

bool CGxDeviceD3d::ICreateWindow(CGxFormat& format) {
    auto instance = GetModuleHandle(nullptr);

    DWORD dwStyle;
    if (format.window == 0) {
        dwStyle = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU;
    } else if (format.maximize == 1) {
        dwStyle = WS_POPUP | WS_VISIBLE;
    } else if (format.maximize == 2) {
        dwStyle = WS_POPUP;
    } else {
        dwStyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_CAPTION | WS_SYSMENU | WS_SIZEBOX | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
    }

    // TODO

    RECT clientArea = {
        0,             // left
        0,             // top
        format.size.x, // right
        format.size.y  // bottom
    };
    AdjustWindowRectEx(&clientArea, dwStyle, false, 0);
    CGxDeviceD3d::s_clientAdjustWidth = clientArea.right - format.size.x - clientArea.left;
    CGxDeviceD3d::s_clientAdjustHeight = clientArea.bottom - format.size.y - clientArea.top;

    // TODO

    int32_t width = format.size.x ? format.size.x : CW_USEDEFAULT;
    int32_t height = format.size.y ? format.size.y : CW_USEDEFAULT;

    if (format.window && format.maximize != 1 && format.size.x && format.size.y) {
        width += CGxDeviceD3d::s_clientAdjustWidth;
        height += CGxDeviceD3d::s_clientAdjustHeight;
    }

    this->m_hwnd = CreateWindowEx(
        WS_EX_APPWINDOW,
        TEXT("GxWindowClassD3d"),
        TEXT("World of Warcraft"),
        dwStyle,
        format.pos.x,
        format.pos.y,
        width,
        height,
        nullptr,
        nullptr,
        instance,
        this
    );

    if (this->m_hwnd && format.maximize != 2) {
        ShowWindow(this->m_hwnd, SW_SHOWNORMAL);
    }

    return this->m_hwnd != nullptr;
}

void CGxDeviceD3d::ISetWindowFocus(bool focus) {
    this->m_windowFocus = focus;

    if (focus) {
        this->m_hwCursorNeedsUpdate = 1;

        if (this->m_format.window == 0) {
            RECT windowRect;
            GetWindowRect(this->m_hwnd, &windowRect);
            ClipCursor(&windowRect);
        }
    }
}

void CGxDeviceD3d::IDestroyD3d() {
    this->IDestroyD3dDevice();
    CGxDeviceD3d::IUnloadD3dLib(this->m_d3dLib, this->m_d3d);
}

void CGxDeviceD3d::IDestroyD3dDevice() {
    // TODO
}

void CGxDeviceD3d::IReleaseD3dPools(int32_t a2) {
    for (auto pool = this->m_poolList.Head(); pool; pool = this->m_poolList.Next(pool)) {
        if (!a2) {
            if (pool->m_usage != GxPoolUsage_Dynamic && pool->m_usage != GxPoolUsage_Stream) {
                continue;
            }
        }

        if (pool->m_usage == GxPoolUsage_Stream) {
            pool->unk1C = 0;
        }

        pool->Invalidate();

        if (pool->m_apiSpecific) {
            if (pool->m_target == GxPoolTarget_Vertex) {
                auto d3dBuf = static_cast<LPDIRECT3DVERTEXBUFFER9>(pool->m_apiSpecific);
                d3dBuf->Release();
            } else if (pool->m_target == GxPoolTarget_Index) {
                auto d3dBuf = static_cast<LPDIRECT3DINDEXBUFFER9>(pool->m_apiSpecific);
                d3dBuf->Release();
            }

            pool->m_apiSpecific = nullptr;
        }
    }
}

void CGxDeviceD3d::IReleaseD3dResources(int32_t a2) {
    // TODO

    this->IReleaseD3dPools(a2);

    memset(this->m_deviceStates, 0xFF, sizeof(this->m_deviceStates));

    if (this->m_defColorSurface) {
        this->m_defColorSurface->Release();
        this->m_defColorSurface = nullptr;
    }

    if (this->m_defDepthSurface) {
        this->m_defDepthSurface->Release();
        this->m_defDepthSurface = nullptr;
    }

    // TODO

    if (this->m_d3dDevice) {
        this->m_d3dDevice->ShowCursor(false);
    }
}

// OFFSET: 0x6A4C30
void CGxDeviceD3d::IRsSendToHw(EGxRenderState which) {
    auto state = &this->m_appRenderStates[which];

    switch (which) {
    case GxRs_PolygonOffset: {
        if (this->Caps().m_depthBias) {
            this->m_d3dDevice->SetRenderState(D3DRS_DEPTHBIAS, -static_cast<float>(state->m_value));
        }

        break;
    }

    case GxRs_MatDiffuse:
    case GxRs_MatEmissive:
    case GxRs_MatSpecular:
    case GxRs_MatSpecularExp: {
        this->ISetMaterial(
            this->m_appRenderStates[GxRs_MatDiffuse].m_value.m_data.i[0],
            this->m_appRenderStates[GxRs_MatEmissive].m_value.m_data.i[0],
            this->m_appRenderStates[GxRs_MatSpecular].m_value.m_data.i[0],
            this->m_appRenderStates[GxRs_MatSpecularExp].m_value.m_data.f[0]
        );

        this->m_appRenderStates[GxRs_MatDiffuse].m_dirty = 0;
        this->m_appRenderStates[GxRs_MatEmissive].m_dirty = 0;
        this->m_appRenderStates[GxRs_MatSpecular].m_dirty = 0;
        this->m_appRenderStates[GxRs_MatSpecularExp].m_dirty = 0;
        break;
    }

    case GxRs_NormalizeNormals: {
        auto normalizeNormals = static_cast<float>(state->m_value);

        if (CGxDeviceD3d::s_normalizeNormals != normalizeNormals) {
            this->m_d3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, normalizeNormals);
            CGxDeviceD3d::s_normalizeNormals = normalizeNormals;
        }

        break;
    }

    case GxRs_BlendingMode: {
        auto blendMode = static_cast<EGxBlend>(static_cast<int32_t>(state->m_value));

        if (blendMode < GxBlend_Alpha) {
            this->DsSet(Ds_AlphaBlendEnable, 0);
        } else {
            this->DsSet(Ds_AlphaBlendEnable, 1);
            this->DsSet(Ds_SrcBlend, CGxDeviceD3d::s_srcBlend[blendMode]);
            this->DsSet(Ds_DstBlend, CGxDeviceD3d::s_dstBlend[blendMode]);
        }

        break;
    }

    case GxRs_AlphaRef: {
        auto alphaRef = static_cast<int32_t>(state->m_value);

        if (alphaRef <= 0) {
            this->DsSet(Ds_AlphaTestEnable, 0);
        } else {
            this->DsSet(Ds_AlphaRef, alphaRef);
            this->DsSet(Ds_AlphaTestEnable, 1);
        }

        break;
    }

    case GxRs_FogStart: {
        this->m_d3dDevice->SetRenderState(D3DRS_FOGSTART, static_cast<uint32_t>(state->m_value));
        break;
    }

    case GxRs_FogEnd: {
        this->m_d3dDevice->SetRenderState(D3DRS_FOGEND, static_cast<uint32_t>(state->m_value));
        break;
    }

    case GxRs_FogColor: {
        this->m_d3dDevice->SetRenderState(D3DRS_FOGCOLOR, static_cast<uint32_t>(state->m_value));
        break;
    }

    case GxRs_Lighting: {
        int32_t enabled = 0;

        if (this->MasterEnable(GxMasterEnable_Lighting)) {
            enabled = state->m_value.m_data.i[0] != 0;
        }

        if (this->m_deviceStates[Ds_Lighting] != enabled) {
            this->m_d3dDevice->SetRenderState(D3DRS_LIGHTING, enabled);
            this->m_deviceStates[Ds_Lighting] = enabled;
        }
        break;
    }

    case GxRs_Fog: {
        auto fogWrite = static_cast<uint32_t>(state->m_value);
        if (!this->MasterEnable(GxMasterEnable_Fog)) {
            fogWrite = 0;
        }

        this->DsSet(Ds_FogEnable, fogWrite);

        break;
    }

    case GxRs_DepthTest:
    case GxRs_DepthFunc: {
        auto depthTest = static_cast<uint32_t>((&this->m_appRenderStates[GxRs_DepthTest])->m_value);
        auto depthFunc = static_cast<uint32_t>((&this->m_appRenderStates[GxRs_DepthFunc])->m_value);

        auto d3dDepthFunc = D3DCMP_ALWAYS;
        if (this->MasterEnable(GxMasterEnable_DepthTest) && depthTest) {
            d3dDepthFunc = CGxDeviceD3d::s_cmpFunc[depthFunc];
        }

        this->DsSet(Ds_ZFunc, d3dDepthFunc);

        this->m_appRenderStates[GxRs_DepthTest].m_dirty = 0;
        this->m_appRenderStates[GxRs_DepthFunc].m_dirty = 0;

        break;
    }

    case GxRs_DepthWrite: {
        auto depthWrite = static_cast<uint32_t>(state->m_value);
        if (!this->MasterEnable(GxMasterEnable_DepthWrite)) {
            depthWrite = 0;
        }

        this->DsSet(Ds_ZWriteEnable, depthWrite);

        break;
    }

    case GxRs_ColorWrite: {
        auto colorWrite = static_cast<uint32_t>(state->m_value);
        if (!this->MasterEnable(GxMasterEnable_ColorWrite)) {
            colorWrite = 0;
        }

        uint32_t finalWrite = (colorWrite & 1) != 0 ? 1 : 0;
        if ((colorWrite & 4) != 0)
            finalWrite |= 2u;
        if ((colorWrite & 2) != 0)
            finalWrite |= 4u;
        if ((colorWrite & 8) != 0)
            finalWrite |= 8u;

        this->DsSet(Ds_ColorWriteEnable, finalWrite);

        break;
    }

    case GxRs_Culling: {
        auto cullMode = static_cast<int32_t>(state->m_value);

        if (!this->MasterEnable(GxMasterEnable_Culling)) {
            cullMode = 0;
        }

        if (cullMode > 2) {
            cullMode = 2;
        }

        this->DsSet(Ds_CullMode, CGxDeviceD3d::s_cullMode[cullMode]);

        break;
    }

    case GxRs_ScissorTest: {
        auto scissorTestEnable = static_cast<uint32_t>(state->m_value) != 0;
        this->m_d3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, scissorTestEnable);

        break;
    }

    case GxRs_Texture0:
    case GxRs_Texture1:
    case GxRs_Texture2:
    case GxRs_Texture3:
    case GxRs_Texture4:
    case GxRs_Texture5:
    case GxRs_Texture6:
    case GxRs_Texture7:
    case GxRs_Texture8:
    case GxRs_Texture9:
    case GxRs_Texture10:
    case GxRs_Texture11:
    case GxRs_Texture12:
    case GxRs_Texture13:
    case GxRs_Texture14:
    case GxRs_Texture15: {
        uint32_t tmu = which - GxRs_Texture0;
        auto texture = static_cast<CGxTex*>(static_cast<void*>(state->m_value));
        this->ISetTexture(tmu, texture);

        break;
    }

    case GxRs_ColorOp0:
    case GxRs_ColorOp1:
    case GxRs_ColorOp2:
    case GxRs_ColorOp3:
    case GxRs_ColorOp4:
    case GxRs_ColorOp5:
    case GxRs_ColorOp6:
    case GxRs_ColorOp7: {
        this->ISetColorOp(which - GxRs_ColorOp0, static_cast<int32_t>(state->m_value));
        break;
    }

    case GxRs_AlphaOp0:
    case GxRs_AlphaOp1:
    case GxRs_AlphaOp2:
    case GxRs_AlphaOp3:
    case GxRs_AlphaOp4:
    case GxRs_AlphaOp5:
    case GxRs_AlphaOp6:
    case GxRs_AlphaOp7: {
        this->ISetAlphaOp(which - GxRs_AlphaOp0, static_cast<int32_t>(state->m_value));
        break;
    }

    case GxRs_TexGen0:
    case GxRs_TexGen1:
    case GxRs_TexGen2:
    case GxRs_TexGen3:
    case GxRs_TexGen4:
    case GxRs_TexGen5:
    case GxRs_TexGen6:
    case GxRs_TexGen7: {
        this->ISetTexGen(which - GxRs_TexGen0, static_cast<int32_t>(state->m_value));
        break;
    }

    case GxRs_TextureCoord0:
    case GxRs_TextureCoord1:
    case GxRs_TextureCoord2:
    case GxRs_TextureCoord3:
    case GxRs_TextureCoord4:
    case GxRs_TextureCoord5:
    case GxRs_TextureCoord6:
    case GxRs_TextureCoord7: {
        this->ISetTexCoord(which - GxRs_TextureCoord0, static_cast<int32_t>(state->m_value));
        break;
    }

    case GxRs_VertexShader: {
        auto shader = static_cast<CGxShader*>(static_cast<void*>(state->m_value));
        this->IShaderBindVertex(shader);

        break;
    }

    case GxRs_PixelShader: {
        auto shader = static_cast<CGxShader*>(static_cast<void*>(state->m_value));
        this->IShaderBindPixel(shader);

        break;
    }

    case GxRs_PointScale: {
        //v22 = v5->m_value.m_data.f[0];
        //if ((unsigned __int8)CGxStateBom__operator_ne(&m_data[80], &dword_AD8BB4)) {
        //    v17 = sub_682D70(this);
        //    v18 = v22 / ((1.0 - this->m_viewport.y.l) * *(float*)(v17 + 8) - (1.0 - this->m_viewport.y.h) * *(float*)(v17 + 8));
        //} else {
        //    v18 = v22;
        //}
        //v23 = v18;
        //((void(__stdcall*)(LPDIRECT3DDEVICE9, _D3DDEVTYPE, _DWORD))this->m_d3dDevice->v_table->v_fn_57_SetRenderState)(
        //    this->m_d3dDevice,
        //    D3DRS_POINTSIZE,
        //    LODWORD(v23))

        break;
    }

    case GxRs_PointScaleAttenuation: {
        //v19 = (unsigned __int8)CGxStateBom__operator_ne(&m_data[a2], &dword_AD8BB4) != 0;
        //if (*(_DWORD*)&this[1].m_gammaRamp.red[58] != v19) {
        //    ((void(__stdcall*)(LPDIRECT3DDEVICE9, _D3DDEVTYPE, int))this->m_d3dDevice->v_table->v_fn_57_SetRenderState)(
        //        this->m_d3dDevice,
        //        D3DRS_POINTSCALEENABLE,
        //        v19);
        //    *(_DWORD*)&this[1].m_gammaRamp.red[58] = v19;
        //}
        //if (v19) {
        //    CGxDeviceD3d::DsSet(this, 178, COERCE__DWORD_(v5->m_value.m_data.f[0]));
        //    CGxDeviceD3d::DsSet(this, 179, COERCE__DWORD_(v5->m_value.m_data.f[1]));
        //    CGxDeviceD3d::DsSet(this, 180, COERCE__DWORD_(v5->m_value.m_data.f[2]));
        //}

        break;
    }

    case GxRs_PointScaleMin: {
        m_d3dDevice->SetRenderState(D3DRS_POINTSIZE_MIN, static_cast<float>(state->m_value));

        break;
    }

    case GxRs_PointScaleMax: {
        m_d3dDevice->SetRenderState(D3DRS_POINTSIZE_MAX, static_cast<float>(state->m_value));

        break;
    }

    case GxRs_PointSprite: {
        m_d3dDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, static_cast<uint32_t>(state->m_value) != 0);

        break;
    }

    case GxRs_BlendFactor: {
        uint8_t channel = (uint8_t)(static_cast<float>(state->m_value) * 255.0f);
        D3DCOLOR factor = D3DCOLOR_RGBA(channel, channel, channel, channel);
        m_d3dDevice->SetRenderState(D3DRS_BLENDFACTOR, factor);
        break;
    }

    case GxRs_ClipPlaneMask:
    case GxRs_Multisample:
    case GxRs_TextureShader0:
    case GxRs_TextureShader1:
    case GxRs_TextureShader2:
    case GxRs_TextureShader3:
    case GxRs_TextureShader4:
    case GxRs_TextureShader5:
    case GxRs_TextureShader6:
    case GxRs_TextureShader7:
    case GxRs_ColorMaterial: {
        break; // Not handled in client
    }

    default:
        SErrDisplayAppFatal("Error, unhandled EGxRenderState '%d'", which);
        break;
    }
}

void CGxDeviceD3d::ICursorCreate(const CGxFormat& format) {
    CGxDevice::ICursorCreate(format);

    if (this->m_hwCursor && this->m_hwCursorTexture == nullptr) {
        this->m_d3dDevice->CreateTexture(
            32,
            32,
            1,
            0,
            D3DFMT_A8R8G8B8,
            D3DPOOL_MANAGED,
            &this->m_hwCursorTexture,
            nullptr);

        if (this->m_hwCursorTexture) {
            this->m_hwCursorTexture->GetSurfaceLevel(0, &this->m_hwCursorBitmap);
        }

        this->m_hwCursorNeedsUpdate = 1;
        this->ICursorDraw();
    }
}

void CGxDeviceD3d::ICursorDestroy() {
    CGxDevice::ICursorDestroy();

    if (this->m_hwCursorBitmap) {
        this->m_hwCursorBitmap->Release();
        this->m_hwCursorBitmap = nullptr;
    }

    if (this->m_hwCursorTexture) {
        this->m_hwCursorTexture->Release();
        this->m_hwCursorTexture = nullptr;
    }
}

void CGxDeviceD3d::CursorSetVisible(int32_t visible) {
    CGxDevice::CursorSetVisible(visible);

    if (this->m_hwCursor && this->m_context) {
        POINT point;
        RECT rect;
        GetCursorPos(&point);
        ScreenToClient(this->m_hwnd, &point);
        GetClientRect(this->m_hwnd, &rect);

        if (rect.left <= point.x && (point.x < rect.right && (rect.top <= point.y)) && point.y < rect.bottom) {
            this->m_d3dDevice->ShowCursor(this->m_cursorVisible);
        }
    }
}

void CGxDeviceD3d::CursorUnlock(uint32_t x, uint32_t y) {
    CGxDevice::CursorUnlock(x, y);
    this->m_hwCursorNeedsUpdate = 1;
}

void CGxDeviceD3d::ICursorDraw() {
    if (!this->m_hwCursor) {
        this->ISceneBegin();
    }

    CGxDevice::ICursorDraw();

    if (!this->m_hwCursor) {
        this->ISceneEnd();
        if (!this->m_hwCursor) {
            return;
        }
    }

    if (this->m_hwCursorNeedsUpdate && this->m_hwCursorBitmap && this->m_context) {
        D3DLOCKED_RECT lockedRect;
        if SUCCEEDED(this->m_hwCursorBitmap->LockRect(&lockedRect, nullptr, 0)) {
            // upload cursor texture data
            auto src = reinterpret_cast<uint8_t*>(this->m_cursor);

            for (int32_t i = 0; i < 32; i++) {
                auto dest = reinterpret_cast<uint8_t*>(lockedRect.pBits) + (lockedRect.Pitch * i);
                memcpy(dest, src, 128);
                src += 128;
            }

            this->m_hwCursorBitmap->UnlockRect();

            this->m_d3dDevice->SetCursorProperties(this->m_cursorHotspotX, this->m_cursorHotspotY, this->m_hwCursorBitmap);
        }

        this->m_hwCursorNeedsUpdate = 0;
    }
}

void CGxDeviceD3d::ISceneBegin() {
    if (!this->m_context) {
        if (this->m_d3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
            D3DPRESENT_PARAMETERS d3dpp;
            this->IReleaseD3dResources(0);
            this->ISetPresentParms(d3dpp, this->m_format);
            if (this->m_d3dDevice->Reset(&d3dpp) == D3D_OK) {
                this->IStateSetD3dDefaults();
                this->ISetWindowFocus(1);
                this->m_context = 1;
                // TODO
                // this->intF5C = 0;
                // this->unk3ACC = 1;

                // this->NotifyOnDeviceRestored();
            }
        }
    }

    if (this->m_context) {
        this->ShaderConstantsClear();

        if (SUCCEEDED(this->m_d3dDevice->BeginScene())) {
            this->m_inScene = 1;
        }
    }
}

void CGxDeviceD3d::ISceneEnd() {
    if (this->m_inScene) {
        this->m_d3dDevice->EndScene();
        this->m_inScene = 0;
    }
}

void CGxDeviceD3d::ISetCaps(const CGxFormat& format) {
    // Texture stages

    int32_t maxSimultaneousTextures = this->m_d3dCaps.MaxSimultaneousTextures;
    this->m_caps.m_numTmus = std::min(maxSimultaneousTextures, 8);

    // Rasterization rules

    this->m_caps.m_pixelCenterOnEdge = 0;
    this->m_caps.m_texelCenterOnEdge = 1;

    // Max texture size

    uint32_t maxTextureWidth = this->m_d3dCaps.MaxTextureWidth;
    this->m_caps.m_texMaxSize[GxTex_2d] = std::max(maxTextureWidth, 256u);
    this->m_caps.m_texMaxSize[GxTex_CubeMap] = std::max(maxTextureWidth, 256u);
    this->m_caps.m_texMaxSize[GxTex_Rectangle] = std::max(maxTextureWidth, 256u);
    this->m_caps.m_texMaxSize[GxTex_NonPow2] = std::max(maxTextureWidth, 256u);

    // Max vertex index

    this->m_caps.m_maxIndex = this->m_d3dCaps.MaxVertexIndex;

    // Trilinear filtering

    this->m_caps.m_texFilterTrilinear =
        (this->m_d3dCaps.TextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR) != 0;

    // Anisotropic filtering

    this->m_caps.m_texFilterAnisotropic =
        (this->m_d3dCaps.TextureFilterCaps & (D3DPTFILTERCAPS_MINFANISOTROPIC | D3DPTFILTERCAPS_MAGFANISOTROPIC)) != 0;

    if (this->m_d3dCaps.TextureFilterCaps & D3DPTFILTERCAPS_MINFANISOTROPIC) {
        CGxDeviceD3d::s_filterModes[GxTex_Anisotropic][0] = D3DTEXF_ANISOTROPIC;
    }

    if (this->m_d3dCaps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFANISOTROPIC) {
        CGxDeviceD3d::s_filterModes[GxTex_Anisotropic][1] = D3DTEXF_ANISOTROPIC;
    }

    this->m_caps.m_maxTexAnisotropy = this->m_d3dCaps.MaxAnisotropy;

    if (this->m_caps.m_texFilterAnisotropic && this->m_d3dCaps.MaxAnisotropy < 2) {
        this->m_caps.m_texFilterAnisotropic = 0;
    }

    // Misc capabilities

    this->m_caps.m_depthBias = (this->m_d3dCaps.RasterCaps & D3DPRASTERCAPS_DEPTHBIAS) != 0;
    this->m_caps.m_numStreams = this->m_d3dCaps.MaxStreams;
    this->m_caps.int10 = (this->m_d3dCaps.Caps2 & 1) != 0; // unknown caps flag

    // Shader targets

    auto pixelShaderVersion = this->m_d3dCaps.PixelShaderVersion;

    if (pixelShaderVersion >= D3DPS_VERSION(3, 0)) {
        this->m_caps.m_shaderTargets[GxSh_Pixel] = GxShPS_ps_3_0;
    } else if (pixelShaderVersion >= D3DPS_VERSION(2, 0)) {
        this->m_caps.m_shaderTargets[GxSh_Pixel] = GxShPS_ps_2_0;
    } else if (pixelShaderVersion >= D3DPS_VERSION(1, 4)) {
        this->m_caps.m_shaderTargets[GxSh_Pixel] = GxShPS_ps_1_4;
    } else if (pixelShaderVersion >= D3DPS_VERSION(1, 1)) {
        this->m_caps.m_shaderTargets[GxSh_Pixel] = GxShPS_ps_1_1;
    }

    if (this->m_caps.m_shaderTargets[GxSh_Pixel] != GxShPS_none) {
        auto vertexShaderVersion = this->m_d3dCaps.VertexShaderVersion;

        if (vertexShaderVersion >= D3DVS_VERSION(3, 0)) {
            this->m_caps.m_shaderTargets[GxSh_Vertex] = GxShVS_vs_3_0;
        } else if (vertexShaderVersion >= D3DVS_VERSION(2, 0)) {
            this->m_caps.m_shaderTargets[GxSh_Vertex] = GxShVS_vs_2_0;
        } else if (vertexShaderVersion == D3DVS_VERSION(1, 1)) {
            this->m_caps.m_shaderTargets[GxSh_Vertex] = GxShVS_vs_1_1;
        }

        // TODO maxVertexShaderConst
    }

    // TODO modify shader targets based on format

    // Detect hardware cursor

    this->m_caps.m_hwCursor = this->m_d3dCaps.CursorCaps & D3DCURSORCAPS_COLOR;

    // Texture formats

    for (int32_t i = 0; i < GxTexFormats_Last; i++) {
        if (i == GxTex_Unknown) {
            this->m_caps.m_texFmt[i] = 0;
        } else {
            this->m_caps.m_texFmt[i] = this->m_d3d->CheckDeviceFormat(
                0,
                D3DDEVTYPE_HAL,
                this->m_devAdapterFormat,
                0,
                D3DRTYPE_TEXTURE,
                CGxDeviceD3d::s_GxTexFmtToD3dFmt[i]
            ) == D3D_OK;
        }
    }

    this->m_caps.m_generateMipMaps = (this->m_d3dCaps.Caps2 & D3DCAPS2_CANAUTOGENMIPMAP) != 0;

    // TODO

    // Texture targets

    this->m_caps.m_texTarget[GxTex_2d] = 1;
    this->m_caps.m_texTarget[GxTex_CubeMap] = (this->m_d3dCaps.TextureCaps & D3DPTEXTURECAPS_CUBEMAP) != 0;
    this->m_caps.m_texTarget[GxTex_Rectangle] = 0;
    this->m_caps.m_texTarget[GxTex_NonPow2] =
        (this->m_d3dCaps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL) != 0 || (this->m_d3dCaps.TextureCaps & D3DPTEXTURECAPS_POW2) == 0;

    // TODO
}

void CGxDeviceD3d::ISetPresentParms(D3DPRESENT_PARAMETERS& d3dpp, const CGxFormat& format) {
    memset(&d3dpp, 0, sizeof(d3dpp));

    if (format.window) {
        D3DDISPLAYMODE currentMode;
        D3DFORMAT backBufferFormat;
        if (SUCCEEDED(this->m_d3d->GetAdapterDisplayMode(0, &currentMode))) {
            backBufferFormat = currentMode.Format;
        } else {
            backBufferFormat = this->m_desktopDisplayMode.Format;
        }

        auto& windowRect = this->DeviceCurWindow();

        d3dpp.Windowed = true;
        d3dpp.BackBufferWidth = windowRect.maxX;
        d3dpp.BackBufferHeight = windowRect.maxY;
        d3dpp.BackBufferFormat = backBufferFormat;

        if (format.vsync) {
            // TODO d3dpp.BackBufferCount = format.int1C;
            d3dpp.BackBufferCount = 1;
        } else {
            d3dpp.BackBufferCount = 1;
        }

        d3dpp.FullScreen_RefreshRateInHz = 0;
    } else {
        d3dpp.BackBufferWidth = format.size.x;
        d3dpp.BackBufferHeight = format.size.y;
        d3dpp.BackBufferFormat = CGxDeviceD3d::s_GxFormatToD3dFormat[format.colorFormat];
        d3dpp.FullScreen_RefreshRateInHz = format.refreshRate;
    }

    d3dpp.hDeviceWindow = this->m_hwnd;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.EnableAutoDepthStencil = true;
    d3dpp.AutoDepthStencilFormat = CGxDeviceD3d::s_GxFormatToD3dFormat[format.depthFormat];

    switch (format.vsync) {
    case 1:
        d3dpp.PresentationInterval = 1;
        break;
    case 2:
        d3dpp.PresentationInterval = format.window ? 1 : 2;
        break;
    case 3:
        d3dpp.PresentationInterval = format.window ? 1 : 4;
        break;
    case 4:
        d3dpp.PresentationInterval = format.window ? 1 : 8;
        break;
    default:
        d3dpp.PresentationInterval = CW_USEDEFAULT;
        break;
    }

    if (format.sampleCount <= 1) {
        d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
    } else {
        d3dpp.MultiSampleType = static_cast<D3DMULTISAMPLE_TYPE>(format.sampleCount);

        // TODO MultiSampleQuality
    }
}

void CGxDeviceD3d::ISetTexture(uint32_t tmu, CGxTex* texId) {
    if (tmu > 15) {
        return;
    }

    if (texId) {
        this->ITexMarkAsUpdated(texId);
        this->m_d3dDevice->SetTexture(tmu, static_cast<LPDIRECT3DBASETEXTURE9>(texId->m_apiSpecificData));

        // Texture filters
        auto& filters = CGxDeviceD3d::s_filterModes[texId->m_flags.m_filter];
        this->DsSet(static_cast<EDeviceState>(Ds_TssMinFilter0 + tmu), filters[0]);
        this->DsSet(static_cast<EDeviceState>(Ds_TssMagFilter0 + tmu), filters[1]);
        this->DsSet(static_cast<EDeviceState>(Ds_TssMipFilter0 + tmu), filters[2]);

        // Texture addressing
        this->DsSet(static_cast<EDeviceState>(Ds_TssWrapU0 + tmu), CGxDeviceD3d::s_wrapModes[texId->m_flags.m_wrapU]);
        this->DsSet(static_cast<EDeviceState>(Ds_TssWrapV0 + tmu), CGxDeviceD3d::s_wrapModes[texId->m_flags.m_wrapV]);

        // Max anisotropy
        this->DsSet(static_cast<EDeviceState>(Ds_TssMaxAnisotropy0 + tmu), texId->m_flags.m_maxAnisotropy);

        if (tmu < 8) {
            // TODO FFP
        }
    } else {
        this->m_d3dDevice->SetTexture(tmu, nullptr);

        if (tmu < 8) {
            // TODO FFP
        }
    }
}

// OFFSET: 0x6A4AC0
void CGxDeviceD3d::ISetTexCoord(uint32_t a2, int32_t a3) {
    if (a2 < this->Caps().m_numTmus) {
        int32_t state;
        this->RsGet((EGxRenderState)(a2 + GxRs_TexGen0), state);
        this->ISetTexCoordIndex(a2, state, a3);
    }
}

// OFFSET: 0x6A4100
void CGxDeviceD3d::ISetTexCoordIndex(int32_t a2, int32_t a3, int32_t a4) {
    switch (a3) {
    case 0:
        this->DsSet((EDeviceState)(a2 + Ds_TssTexCoordIndex0), a4);
        break;
    case 1:
    case 2:
    case 3:
        this->DsSet((EDeviceState)(a2 + Ds_TssTexCoordIndex0), (a2 | 0x20000));
        break;
    case 4:
    case 6:
        this->DsSet((EDeviceState)(a2 + Ds_TssTexCoordIndex0), (a2 | 0x40000));
        break;
    case 5:
        this->DsSet((EDeviceState)(a2 + Ds_TssTexCoordIndex0), (a2 | 0x10000));
        break;
    }
}

// OFFSET: 0x6A4AF0
void CGxDeviceD3d::ISetTexGen(uint32_t a2, int32_t a3) {
    if (a2 >= this->Caps().m_numTmus)
        return;

    int32_t state;
    this->RsGet((EGxRenderState)(a2 + GxRs_TextureCoord0), state);
    this->ISetTexCoordIndex(a2, a3, state);

    if (a3 <= 0) {
        m_texGen[a2].Identity();
        return;
    }

    C44Matrix mat;

    if (a3 == 1 || a3 == 2) {
        this->XformView(mat);
        mat = mat.AffineInverse();

        if (a3 == 1) {
            C44Matrix biasMat = C44Matrix().Inverse();
            mat *= biasMat;
        }
    } else if (a3 == 6) {
        mat = C44Matrix();
        mat.a0 = 0.5f;
        mat.b1 = 0.5f;
        mat.d0 = 0.5f;
        mat.d1 = 0.5f;
    } else {
        return;
    }

    m_texGen[a2].Top() = mat;
}

// OFFSET: 0x6A41F0
void CGxDeviceD3d::ISetAlphaOp(uint32_t a2, int32_t a3) {
    if (a2 >= this->Caps().m_numTmus)
        return;

    this->DsSet((EDeviceState)(a2 + Ds_TssAlphaOp0), CGxDeviceD3d::s_texOp[a3]);
    this->DsSet((EDeviceState)(a2 + Ds_TssAlphaArg10), CGxDeviceD3d::s_texArgs[a3 * 2]);
    this->DsSet((EDeviceState)(a2 + Ds_TssAlphaArg20), CGxDeviceD3d::s_texArgs[a3 * 2 + 1]);
}

// OFFSET: 0x6A4190
void CGxDeviceD3d::ISetColorOp(uint32_t a2, int32_t a3) {
    if (a2 >= this->Caps().m_numTmus)
        return;

    this->DsSet((EDeviceState)(a2 + Ds_TssColorOp0), CGxDeviceD3d::s_texOp[a3]);
    this->DsSet((EDeviceState)(a2 + Ds_TssColorArg10), CGxDeviceD3d::s_texArgs[a3 * 2]);
    this->DsSet((EDeviceState)(a2 + Ds_TssColorArg20), CGxDeviceD3d::s_texArgs[a3 * 2 + 1]);
}

void CGxDeviceD3d::ISetVertexBuffer(uint32_t stream, LPDIRECT3DVERTEXBUFFER9 buffer, uint32_t offset, uint32_t stride) {
    if (!this->m_caps.int10) {
        offset = 0;
    }

    if (this->m_d3dVertexStreamBuf[stream] != buffer || this->m_d3dVertexStreamOfs[stream] != offset || this->m_d3dVertexStreamStride[stream] != stride) {
        this->m_d3dDevice->SetStreamSource(stream, buffer, offset, stride);

        this->m_d3dVertexStreamBuf[stream] = buffer;
        this->m_d3dVertexStreamOfs[stream] = offset;
        this->m_d3dVertexStreamStride[stream] = stride;
    }
}

void CGxDeviceD3d::IShaderBindPixel(CGxShader* shader) {
    if (!shader) {
        this->m_d3dDevice->SetPixelShader(nullptr);

        // TODO FFP handling

        return;
    }

    if (!shader->loaded) {
        this->IShaderCreatePixel(shader);
    }

    auto d3dShader = static_cast<LPDIRECT3DPIXELSHADER9>(shader->apiSpecific);
    this->m_d3dDevice->SetPixelShader(d3dShader);
}

void CGxDeviceD3d::IShaderBindVertex(CGxShader* shader) {
    if (!shader) {
        this->m_d3dDevice->SetVertexShader(nullptr);
        return;
    }

    if (!shader->loaded) {
        this->IShaderCreateVertex(shader);
    }

    auto d3dShader = static_cast<LPDIRECT3DVERTEXSHADER9>(shader->apiSpecific);
    this->m_d3dDevice->SetVertexShader(d3dShader);
}

void CGxDeviceD3d::IShaderConstantsFlush() {
    // Vertex shader constants
    auto vsConst = &CGxDevice::s_shadowConstants[1];
    if (vsConst->unk2 <= vsConst->unk1) {
        this->m_d3dDevice->SetVertexShaderConstantF(
            vsConst->unk2,
            reinterpret_cast<float*>(&vsConst->constants[vsConst->unk2]),
            vsConst->unk1 - vsConst->unk2 + 1
        );
    }
    vsConst->unk2 = 255;
    vsConst->unk1 = 0;

    // Pixel shader constants
    auto psConst = &CGxDevice::s_shadowConstants[0];
    if (psConst->unk2 <= psConst->unk1) {
        this->m_d3dDevice->SetPixelShaderConstantF(
            psConst->unk2,
            reinterpret_cast<float*>(&psConst->constants[psConst->unk2]),
            psConst->unk1 - psConst->unk2 + 1
        );
    }
    psConst->unk2 = 255;
    psConst->unk1 = 0;
}

void CGxDeviceD3d::IShaderCreate(CGxShader* shader) {
    if (shader->target == GxSh_Vertex) {
        this->IShaderCreateVertex(shader);
    } else if (shader->target == GxSh_Pixel) {
        this->IShaderCreatePixel(shader);
    }
}

void CGxDeviceD3d::IShaderCreatePixel(CGxShader* shader) {
    shader->valid = 0;

    if (!this->m_context) {
        return;
    }

    shader->loaded = 1;

    if (shader->code.Count() == 0) {
        return;
    }

    LPDIRECT3DPIXELSHADER9 d3dShader;
    if (SUCCEEDED(this->m_d3dDevice->CreatePixelShader(reinterpret_cast<DWORD*>(shader->code.Ptr()), &d3dShader))) {
        shader->apiSpecific = d3dShader;
        shader->valid = 1;
    }
}

void CGxDeviceD3d::IShaderCreateVertex(CGxShader* shader) {
    shader->valid = 0;

    if (!this->m_context) {
        return;
    }

    shader->loaded = 1;

    if (shader->code.Count() == 0) {
        return;
    }

    LPDIRECT3DVERTEXSHADER9 d3dShader;
    if (SUCCEEDED(this->m_d3dDevice->CreateVertexShader(reinterpret_cast<DWORD*>(shader->code.Ptr()), &d3dShader))) {
        shader->apiSpecific = d3dShader;
        shader->valid = 1;
    }
}

void CGxDeviceD3d::IStateSetD3dDefaults() {
    this->m_d3dDevice->SetRenderState(D3DRS_ZENABLE, 1);
    this->m_d3dDevice->SetRenderState(D3DRS_LOCALVIEWER, 1);
    this->m_d3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
    this->m_d3dDevice->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);
    this->m_d3dDevice->SetRenderState(D3DRS_FOGDENSITY, 0);

    for (uint32_t tmu = 0; tmu < 16; tmu++) {
        this->m_d3dDevice->SetSamplerState(tmu, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);
    }

    this->IRsForceUpdate();
    this->IRsSync(0);

    this->m_primVertexDirty = -1;
    this->m_primIndexDirty = 0;

    this->m_d3dDevice->SetIndices(nullptr);

    this->m_d3dCurrentVertexDecl = nullptr;
    this->m_d3dCurrentIndexBuf = nullptr;

    for (uint32_t i = 0; i < 8; i++) {
        this->m_d3dVertexStreamBuf[i] = nullptr;
        this->m_d3dVertexStreamOfs[i] = -1;
        this->m_d3dVertexStreamStride[i] = -1;
    }

    this->m_d3dDevice->GetRenderTarget(0, &this->m_defColorSurface);
    this->m_d3dDevice->GetDepthStencilSurface(&this->m_defDepthSurface);

    // TODO

    this->ISceneBegin();
}

void CGxDeviceD3d::IStateSync() {
    // TODO

    this->IShaderConstantsFlush();
    this->IRsSync(0);

    if (this->m_hwRenderStates[GxRs_VertexShader] == nullptr || this->m_appRenderStates[GxRs_VertexShader].m_value == nullptr) {
        this->IStateSyncLights();
        this->IStateSyncMaterial();
        this->IStateSyncXforms();
    }

    this->IStateSyncEnables();

    // TODO

    this->IStateSyncVertexPtrs();
    this->IStateSyncIndexPtr();

    // TODO

    if (this->m_needsReset) {
        this->IXformSetViewport();
    }
}

void CGxDeviceD3d::IStateSyncEnables() {
    // TODO
}

void CGxDeviceD3d::IStateSyncIndexPtr() {
    if (!this->m_primIndexDirty) {
        return;
    }

    this->m_primIndexDirty = 0;

    auto d3dIndexBuf = static_cast<LPDIRECT3DINDEXBUFFER9>(this->m_primIndexBuf->m_pool->m_apiSpecific);

    if (this->m_d3dCurrentIndexBuf != d3dIndexBuf) {
        this->m_d3dDevice->SetIndices(d3dIndexBuf);
        this->m_d3dCurrentIndexBuf = d3dIndexBuf;
    }
}

// OFFSET: 0x6A43D0
void CGxDeviceD3d::IStateSyncLights() {
    uint32_t index = 0;

    if (this->m_appRenderStates[GxRs_Lighting].m_value != 0) {

        this->m_ambientOnlyMode = 0;

        int32_t lightingEnable = this->MasterEnable(GxMasterEnable_Lighting) ? 1 : 0;

        if (this->m_deviceStates[Ds_Lighting] != lightingEnable) {
            this->m_d3dDevice->SetRenderState(D3DRS_LIGHTING, lightingEnable);
            this->m_deviceStates[Ds_Lighting] = lightingEnable;
        }

        if (this->m_deviceStates[Ds_Ambient] != 0) {
            this->m_d3dDevice->SetRenderState(D3DRS_AMBIENT, 0);
            this->m_deviceStates[Ds_Ambient] = 0;
        }

        for (index = 0; index < 4; index++) {
            CGxApiLight& light = this->m_lights[index];

            if (light.flags == 0) {
                continue;
            }

            bool enabledOnDevice = (light.flags & 0x1) != 0;

            if (enabledOnDevice && !light.m_enable) {
                light.flags &= ~0x1;
                this->m_d3dDevice->LightEnable(index, FALSE);
                continue;
            }

            if (!enabledOnDevice && !light.m_enable) {
                continue;
            }

            if (enabledOnDevice && light.m_enable) {
                this->m_d3dDevice->LightEnable(index, TRUE);
            }

            D3DLIGHT9& d3d = s_d3dLight;

            if (light.m_dir.w == 1.0f) {
                d3d.Type = D3DLIGHT_POINT;
                d3d.Position.x = light.m_dir.x;
                d3d.Position.y = light.m_dir.y;
                d3d.Position.z = light.m_dir.z;
            } else {
                d3d.Type = D3DLIGHT_DIRECTIONAL;
                d3d.Direction.x = light.m_dir.x;
                d3d.Direction.y = light.m_dir.y;
                d3d.Direction.z = light.m_dir.z;
            }

            d3d.Diffuse.r = light.m_dirColor.x;
            d3d.Diffuse.g = light.m_dirColor.y;
            d3d.Diffuse.b = light.m_dirColor.z;
            d3d.Diffuse.a = 1.0f;

            d3d.Specular.r = light.m_specColor.x;
            d3d.Specular.g = light.m_specColor.y;
            d3d.Specular.b = light.m_specColor.z;
            d3d.Specular.a = 0.0f;

            d3d.Ambient.r = light.m_ambColor.x;
            d3d.Ambient.g = light.m_ambColor.y;
            d3d.Ambient.b = light.m_ambColor.z;
            d3d.Ambient.a = 0.0f;

            d3d.Range = 10000.0f;
            d3d.Falloff = 1.0f;
            d3d.Attenuation0 = light.m_constantAttenuation;
            d3d.Attenuation1 = light.m_linearAttenuation;
            d3d.Attenuation2 = light.m_quadraticAttenuation;

            this->m_d3dDevice->SetLight(index, &d3d);

            light.flags = 0;
        }

        return;
    }

    bool vertexHasColor = (this->m_primVertexMask & GxPrim_Color0) != 0;
    bool matDiffuseWhite =
        this->m_appRenderStates[GxRs_MatDiffuse].m_value == 0xFFFFFFFF;

    if (vertexHasColor || matDiffuseWhite) {
        this->m_ambientOnlyMode = 0;

        if (this->m_deviceStates[Ds_Lighting] != 0) {
            this->m_d3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
            this->m_deviceStates[Ds_Lighting] = 0;
        }
        if (this->m_deviceStates[Ds_Ambient] != 0) {
            this->m_d3dDevice->SetRenderState(D3DRS_AMBIENT, 0);
            this->m_deviceStates[Ds_Ambient] = 0;
        }
        return;
    }

    if (!this->m_ambientOnlyMode) {
        this->m_ambientOnlyMode = 1;

        for (index = 0; index < 4; index++) {
            CGxApiLight& light = this->m_lights[index];

            this->m_d3dDevice->LightEnable(index, FALSE);

            if (light.m_enable) {
                light.flags |= 0x1;
            } else {
                light.flags &= ~0x1;
            }
        }
    }

    if (this->m_deviceStates[Ds_Lighting] != 1) {
        this->m_d3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
        this->m_deviceStates[Ds_Lighting] = 1;
    }
    if (this->m_deviceStates[Ds_Ambient] != 0xFFFFFFFF) {
        this->m_d3dDevice->SetRenderState(D3DRS_AMBIENT, 0xFFFFFFFF);
        this->m_deviceStates[Ds_Ambient] = 0xFFFFFFFF;
    }
}

// OFFSET: 0x6A4700
void CGxDeviceD3d::IStateSyncMaterial() {
    uint32_t emissiveSource = 0;

    if ((this->m_primVertexMask & GxPrim_Color0) != 0) {
        int32_t colorMaterial = this->m_appRenderStates[GxRs_ColorMaterial].m_value.m_data.i[0];

        uint32_t useVertexColor = (colorMaterial == 0);

        if (this->m_deviceStates[Ds_AmbientMaterialSource] != useVertexColor) {
            this->m_d3dDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, useVertexColor);
            this->m_deviceStates[Ds_AmbientMaterialSource] = useVertexColor;
        }

        if (this->m_deviceStates[Ds_DiffuseMaterialSource] != useVertexColor) {
            this->m_d3dDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, useVertexColor);
            this->m_deviceStates[Ds_DiffuseMaterialSource] = useVertexColor;
        }

        uint32_t specularSource = (colorMaterial == 1);
        if (this->m_deviceStates[Ds_SpecularMaterialSource] != specularSource) {
            this->m_d3dDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, specularSource);
            this->m_deviceStates[Ds_SpecularMaterialSource] = specularSource;
        }

        emissiveSource = (colorMaterial == 2);

    } else {
        if (this->m_deviceStates[Ds_AmbientMaterialSource] != 0) {
            this->m_d3dDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, 0);
            this->m_deviceStates[Ds_AmbientMaterialSource] = 0;
        }
        if (this->m_deviceStates[Ds_DiffuseMaterialSource] != 0) {
            this->m_d3dDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, 0);
            this->m_deviceStates[Ds_DiffuseMaterialSource] = 0;
        }
        if (this->m_deviceStates[Ds_SpecularMaterialSource] != 0) {
            this->m_d3dDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, 0);
            this->m_deviceStates[Ds_SpecularMaterialSource] = 0;
        }
        emissiveSource = 0;
    }

    if (this->m_deviceStates[Ds_EmissiveMaterialSource] != emissiveSource) {
        this->m_d3dDevice->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, emissiveSource);
        this->m_deviceStates[Ds_EmissiveMaterialSource] = emissiveSource;
    }
}

// OFFSET: 0x6A4250
void CGxDeviceD3d::ISetMaterial(uint32_t diffuse, uint32_t emissive, uint32_t specular, float power) {
    constexpr float k = 1.0f / 255.0f;

    s_d3dMaterial.Diffuse.r = ((diffuse >> 16) & 0xFF) * k;
    s_d3dMaterial.Diffuse.g = ((diffuse >> 8) & 0xFF) * k;
    s_d3dMaterial.Diffuse.b = (diffuse & 0xFF) * k;
    s_d3dMaterial.Diffuse.a = (diffuse >> 24) * k;

    s_d3dMaterial.Ambient.r = s_d3dMaterial.Diffuse.r;
    s_d3dMaterial.Ambient.g = s_d3dMaterial.Diffuse.g;
    s_d3dMaterial.Ambient.b = s_d3dMaterial.Diffuse.b;
    s_d3dMaterial.Ambient.a = s_d3dMaterial.Diffuse.a;

    s_d3dMaterial.Emissive.r = ((emissive >> 16) & 0xFF) * k;
    s_d3dMaterial.Emissive.g = ((emissive >> 8) & 0xFF) * k;
    s_d3dMaterial.Emissive.b = (emissive & 0xFF) * k;
    s_d3dMaterial.Emissive.a = (emissive >> 24) * k;

    s_d3dMaterial.Specular.r = ((specular >> 16) & 0xFF) * k;
    s_d3dMaterial.Specular.g = ((specular >> 8) & 0xFF) * k;
    s_d3dMaterial.Specular.b = (specular & 0xFF) * k;
    s_d3dMaterial.Specular.a = (specular >> 24) * k;

    s_d3dMaterial.Power = power;

    this->m_d3dDevice->SetMaterial(&s_d3dMaterial); // vtable +196 = slot 49
}

void CGxDeviceD3d::IStateSyncVertexPtrs() {
    if (this->m_primVertexFormat < GxVertexBufferFormats_Last && this->m_d3dVertexDecl[this->m_primVertexFormat]) {
        auto d3dVertexDecl = this->m_d3dVertexDecl[this->m_primVertexFormat];

        if (this->m_d3dCurrentVertexDecl != d3dVertexDecl) {
            this->m_d3dDevice->SetVertexDeclaration(d3dVertexDecl);
            this->m_d3dCurrentVertexDecl = d3dVertexDecl;
        }

        this->ISetVertexBuffer(
            0,
            static_cast<LPDIRECT3DVERTEXBUFFER9>(this->m_primVertexBuf->m_pool->m_apiSpecific),
            this->m_primVertexBuf->m_index,
            this->m_primVertexSize
        );

        return;
    }

    CGxBuf* streamBufs[GxVAs_Last] = { 0 };
    uint32_t streamSizes[GxVAs_Last] = { 0 };

    D3DVERTEXELEMENT9 elements[GxVAs_Last + 1];
    uint32_t elementCount = 0;
    uint32_t streamCount = 0;

    for (uint32_t i = 0; i < GxVAs_Last; i++) {
        if ((1 << i) & this->m_primVertexMask) {
            uint32_t stream = 0;

            if (streamCount) {
                do {
                    if (streamBufs[stream] == this->m_primVertexFormatBuf[i]) {
                        break;
                    }
                    stream++;
                } while (stream < streamCount);
            }

            if (stream == streamCount) {
                streamBufs[stream] = this->m_primVertexFormatBuf[i];
                streamCount++;
            }

            auto& attrib = this->m_primVertexFormatAttrib[i];

            streamSizes[stream] += CGxDeviceD3d::s_gxAttribToD3dAttribSize[attrib.type];

            elements[elementCount].Stream = stream;
            elements[elementCount].Offset = attrib.offset;
            elements[elementCount].Type = CGxDeviceD3d::s_gxAttribToD3dAttribType[attrib.type];
            elements[elementCount].Method = D3DDECLMETHOD_DEFAULT;
            elements[elementCount].Usage = CGxDeviceD3d::s_gxAttribToD3dAttribUsage[attrib.attrib];
            elements[elementCount].UsageIndex = CGxDeviceD3d::s_gxAttribToD3dAttribUsageIndex[attrib.attrib];

            elementCount++;
        }
    }

    elements[elementCount] = D3DDECL_END();
    elementCount++;

    auto d3dVertexDecl = this->ICreateD3dVertexDecl(elements, elementCount);
    if (this->m_d3dCurrentVertexDecl != d3dVertexDecl) {
        this->m_d3dDevice->SetVertexDeclaration(d3dVertexDecl);
        this->m_d3dCurrentVertexDecl = d3dVertexDecl;
    }

    for (uint32_t stream = 0; stream < streamCount; stream++) {
        auto streamBuf = streamBufs[stream];

        this->ISetVertexBuffer(
            stream,
            static_cast<LPDIRECT3DVERTEXBUFFER9>(streamBuf->m_pool->m_apiSpecific),
            streamBuf->m_index,
            streamSizes[stream]
        );
    }
}

// OFFSET: 0x6A4850
void CGxDeviceD3d::IStateSyncXforms() {
    if (this->m_xforms[GxXform_Projection].m_dirty) {
        this->m_d3dDevice->SetTransform(D3DTS_PROJECTION, reinterpret_cast<D3DMATRIX*>(&this->m_projNative));
        this->m_xforms[GxXform_Projection].m_dirty = 0;
    }

    if (this->m_xforms[GxXform_View].m_dirty) {
        this->m_d3dDevice->SetTransform(D3DTS_VIEW, reinterpret_cast<const D3DMATRIX*>(&this->m_xforms[GxXform_View].TopConst()));
        this->m_xforms[GxXform_View].m_dirty = 0;
    }

    if (this->m_xforms[GxXform_World].m_dirty) {
        this->IXformSetWorld();
    }

    for (int32_t i = 0; i < this->Caps().m_numTmus; i++) {
        CGxMatrixStack texStack = this->m_texGen[i];
        CGxMatrixStack formStack = this->m_xforms[i];

        if (formStack.m_dirty || texStack.m_dirty) {
            this->IXformSetTex(i);
        }
    }
}

void CGxDeviceD3d::ITexCreate(CGxTex* texId) {
    uint32_t width, height, startLevel, endLevel;
    this->ITexWHDStartEnd(texId, width, height, startLevel, endLevel);

    texId->m_format = CGxDeviceD3d::s_GxTexFmtToUse[texId->m_format];

    uint32_t d3dUsage = 0;
    D3DPOOL d3dPool = D3DPOOL_MANAGED;

    if (texId->m_flags.m_renderTarget) {
        d3dUsage = D3DUSAGE_RENDERTARGET;
        d3dPool = D3DPOOL_DEFAULT;
    }

    if (texId->m_flags.m_generateMipMaps) {
        d3dUsage |= D3DUSAGE_AUTOGENMIPMAP;
    }

    // Cube map
    if (texId->m_target == GxTex_CubeMap) {
        auto d3dFormat = CGxDeviceD3d::s_GxTexFmtToD3dFmt[texId->m_format];
        LPDIRECT3DCUBETEXTURE9 d3dTexture;

        if (SUCCEEDED(this->m_d3dDevice->CreateCubeTexture(width, endLevel - startLevel, d3dUsage, d3dFormat, d3dPool, &d3dTexture, nullptr))) {
            texId->m_apiSpecificData = d3dTexture;
            texId->m_needsCreation = 0;
        }

        return;
    }

    // Depth stencil
    if (texId->m_format == GxTex_D24X8) {
        d3dUsage = D3DUSAGE_DEPTHSTENCIL;
        auto d3dFormat = D3DFMT_D24X8;
        LPDIRECT3DTEXTURE9 d3dTexture;

        if (SUCCEEDED(this->m_d3dDevice->CreateTexture(width, height, 1, d3dUsage, d3dFormat, d3dPool, &d3dTexture, nullptr))) {
            texId->m_apiSpecificData = d3dTexture;
            texId->m_needsCreation = 0;
        }

        return;
    }

    // Ordinary texture
    LPDIRECT3DTEXTURE9 d3dTexture;
    auto d3dFormat = CGxDeviceD3d::s_GxTexFmtToD3dFmt[texId->m_format];

    if (SUCCEEDED(this->m_d3dDevice->CreateTexture(width, height, endLevel - startLevel, d3dUsage, d3dFormat, d3dPool, &d3dTexture, nullptr))) {
        texId->m_apiSpecificData = d3dTexture;
        texId->m_needsCreation = 0;

        return;
    }

    // TODO flag check SLOBYTE(texId->m_flags)

    // If texture creation failed, try again with a fallback format
    CGxDeviceD3d::s_GxTexFmtToUse[texId->m_format] = CGxDeviceD3d::s_tolerableTexFmtMapping[texId->m_format];
    texId->m_format = CGxDeviceD3d::s_GxTexFmtToUse[texId->m_format];
    d3dFormat = CGxDeviceD3d::s_GxTexFmtToD3dFmt[texId->m_format];

    if (SUCCEEDED(this->m_d3dDevice->CreateTexture(width, height, endLevel - startLevel, d3dUsage, d3dFormat, d3dPool, &d3dTexture, nullptr))) {
        texId->m_apiSpecificData = d3dTexture;
        texId->m_needsCreation = 0;
    }
}

void CGxDeviceD3d::ITexMarkAsUpdated(CGxTex* texId) {
    if (!texId->m_needsUpdate || !this->m_context) {
        return;
    }

    if (texId->m_needsCreation || (!texId->m_apiSpecificData && !texId->m_apiSpecificData2)) {
        this->ITexCreate(texId);
    }

    if (!texId->m_needsCreation && (texId->m_apiSpecificData || texId->m_apiSpecificData2)) {
        if (texId->m_userFunc) {
            this->ITexUpload(texId);
        }

        CGxDevice::ITexMarkAsUpdated(texId);
    }
}

void CGxDeviceD3d::ITexUpload(CGxTex* texId) {
    uint32_t texelStrideInBytes;
    const void* texels = nullptr;

    texId->m_userFunc(GxTex_Lock, texId->m_width, texId->m_height, 0, 0, texId->m_userArg, texelStrideInBytes, texels);

    uint32_t width;
    uint32_t height;
    uint32_t startLevel;
    uint32_t endLevel;
    this->ITexWHDStartEnd(texId, width, height, startLevel, endLevel);

    int32_t numFace = texId->m_target == GxTex_CubeMap ? 6 : 1;

    for (int32_t face = 0; face < numFace; face++) {
        for (int32_t level = startLevel; level < endLevel; level++) {
            texels = nullptr;

            texId->m_userFunc(
                GxTex_Latch,
                texId->m_width >> level,
                texId->m_height >> level,
                face,
                level,
                texId->m_userArg,
                texelStrideInBytes,
                texels
            );

            STORM_ASSERT(texels != nullptr || texId->m_flags.m_renderTarget);

            LPDIRECT3DSURFACE9 surface = nullptr;
            HRESULT surfaceResult;

            if (texId->m_target == GxTex_CubeMap) {
                auto d3dTexture = static_cast<LPDIRECT3DCUBETEXTURE9>(texId->m_apiSpecificData);
                surfaceResult = d3dTexture->GetCubeMapSurface(CGxDeviceD3d::s_faceTypes[face], level, &surface);
            } else {
                auto d3dTexture = static_cast<LPDIRECT3DTEXTURE9>(texId->m_apiSpecificData);
                surfaceResult = d3dTexture->GetSurfaceLevel(level, &surface);
            }

            if (FAILED(surfaceResult)) {
                goto UNLOCK;
            }

            RECT rect = {
                texId->m_updateRect.minX >> level,  // left
                texId->m_updateRect.minY >> level,  // top
                texId->m_updateRect.maxX >> level,  // right
                texId->m_updateRect.maxY >> level,  // bottom
            };

            rect.right = std::max(rect.right, rect.left + 1);
            rect.bottom = std::max(rect.bottom, rect.top + 1);

            if (texId->m_format == GxTex_Dxt1 || texId->m_format == GxTex_Dxt3 || texId->m_format == GxTex_Dxt5) {
                rect.left &= 0xFFFFFFFC;
                rect.top &= 0xFFFFFFFC;
                rect.bottom = (rect.bottom + 3) & 0xFFFFFFFC;
                rect.right = (rect.right + 3) & 0xFFFFFFFC;

                rect.bottom = std::min(rect.bottom, static_cast<LONG>(height));
                rect.right = std::min(rect.right, static_cast<LONG>(width));
            }

            D3DLOCKED_RECT lockedRect;
            if (FAILED(surface->LockRect(&lockedRect, &rect, 0x0))) {
                surface->Release();
                goto UNLOCK;
            }

            if (texId->m_flags.m_bit15) {
                // TODO
            }

            C2iVector size = { rect.right - rect.left, rect.bottom - rect.top };

            Blit(
                size,
                BlitAlpha_0,
                texels,
                texelStrideInBytes,
                GxGetBlitFormat(texId->m_dataFormat),
                lockedRect.pBits,
                lockedRect.Pitch,
                GxGetBlitFormat(texId->m_format)
            );

            surface->UnlockRect();
            surface->Release();
        }
    }

UNLOCK:
    texels = nullptr;
    texId->m_userFunc(GxTex_Unlock, texId->m_width, texId->m_height, 0, 0, texId->m_userArg, texelStrideInBytes, texels);

    if (!texId->m_flags.m_renderTarget) {
        auto d3dTexture = static_cast<LPDIRECT3DTEXTURE9>(texId->m_apiSpecificData);
        d3dTexture->PreLoad();
    }
}

void CGxDeviceD3d::IXformSetProjection(const C44Matrix& matrix) {
    DirectX::XMMATRIX projNative;
    memcpy(&projNative, &matrix, sizeof(projNative));

    if (NotEqual(projNative._34, 1.0f, WHOA_EPSILON_1) && NotEqual(projNative._34, 0.0f, WHOA_EPSILON_1)) {
        projNative /= projNative._34;
    }

    if (projNative._44 == 0.0f) {
        auto v5 = -(projNative._43 / (projNative._33 + 1.0f));
        auto v6 = -(projNative._43 / (projNative._33 - 1.0f));
        projNative._33 = v6 / (v6 - v5);
        projNative._43 = v6 * v5 / (v5 - v6);
    } else {
        auto v8 = 1.0f / projNative._33;
        auto v9 = (-1.0f - projNative._43) * v8;
        auto v10 = v8 * (1.0f - projNative._43);
        projNative._33 = 1.0f / (v10 - v9);
        projNative._43 = v9 / (v9 - v10);
    }

    if (!this->MasterEnable(GxMasterEnable_NormalProjection) && projNative._44 != 1.0f) {
        DirectX::XMMATRIX shrink = {
            0.2f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.2f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.2f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };

        projNative *= shrink;
    }

    this->m_xforms[GxXform_Projection].m_dirty = 1;
    memcpy(&this->m_projNative, &projNative, sizeof(this->m_projNative));
}

void CGxDeviceD3d::IXformSetViewport() {
    const auto& gxViewport = this->m_viewport;
    auto windowRect = this->DeviceCurWindow();

    D3DVIEWPORT9 d3dViewport;

    d3dViewport.X = (gxViewport.x.l * windowRect.maxX) + 0.5;
    d3dViewport.Y = ((1.0 - gxViewport.y.h) * windowRect.maxY) + 0.5;

    // TODO account for negative X value

    d3dViewport.Width = (gxViewport.x.h * windowRect.maxX) - d3dViewport.X + 0.5;
    d3dViewport.Height = ((1.0 - gxViewport.y.l) * windowRect.maxY) - d3dViewport.Y + 0.5;

    d3dViewport.MinZ = gxViewport.z.l;
    d3dViewport.MaxZ = gxViewport.z.h;

    // TODO conditionally adjust Y value

    this->m_d3dDevice->SetViewport(&d3dViewport);

    this->m_needsReset = 0;
}

void CGxDeviceD3d::IXformSetWorld() {
    static int32_t isIdent = 0;

    auto& stack = this->m_xforms[GxXform_World];

    if (!isIdent || !(stack.m_flags[stack.m_level] & CGxMatrixStack::F_Identity)) {
        this->m_d3dDevice->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&stack.TopConst()));
    }

    isIdent = stack.m_flags[stack.m_level] & CGxMatrixStack::F_Identity;
    stack.m_dirty = 0;
}

// OFFSET: 0x6A5AA0
void CGxDeviceD3d::IXformSetTex(int32_t index) {
    int32_t v3 = static_cast<int32_t>(this->m_appRenderStates[GxRs_TextureShader0 + index].m_value);
    uint32_t v4 = 0;

    if (v3 == 0) {
        C44Matrix& texGenMat = this->m_texGen[index].m_mtx[this->m_texGen[index].m_level];
        bool isProjected = (this->m_texGen[index].m_flags[this->m_texGen[index].m_level] & 1) != 0;
        this->m_d3dDevice->SetTransform((D3DTRANSFORMSTATETYPE)(index + D3DTS_TEXTURE0), (D3DMATRIX*)&texGenMat);

        v4 = isProjected ? D3DTTFF_DISABLE : D3DTTFF_COUNT3;
    } else if (v3 == 1) {
        C44Matrix mat = this->m_texGen[index].TopConst() * this->m_xforms[index].TopConst();
        int32_t state;
        this->RsGet((EGxRenderState)(index + GxRs_TexGen0), state);
        if (state) {
            v4 = D3DTTFF_COUNT3;
        } else {
            v4 = D3DTTFF_COUNT2;
            mat.c0 = mat.d0;
            mat.c1 = mat.d1;
        }
        this->m_d3dDevice->SetTransform((D3DTRANSFORMSTATETYPE)(index + D3DTS_TEXTURE0), (D3DMATRIX*)&mat);
    } else if (v3 == 2) {
        C44Matrix mat = this->m_texGen[index].TopConst() * this->m_xforms[index].TopConst();
        this->m_d3dDevice->SetTransform((D3DTRANSFORMSTATETYPE)(index + D3DTS_TEXTURE0), (D3DMATRIX*)&mat);
        v4 = D3DTTFF_COUNT3 | D3DTTFF_PROJECTED;
    }

    this->DsSet((EDeviceState)(Ds_TssTTF0 + index), v4);
    this->m_xforms[index].m_dirty = 0;
    this->m_texGen[index].m_dirty = 0;
}

void CGxDeviceD3d::PoolSizeSet(CGxPool* pool, uint32_t size) {
    // TODO
}

void CGxDeviceD3d::SceneClear(uint32_t mask, CImVector color) {
    CGxDevice::SceneClear(mask, color);

    if (!this->m_context) {
        return;
    }

    uint32_t flags = 0x0;
    if (mask & 0x1) {
        flags |= 0x1;
    }
    if (mask & 0x2) {
        flags |= 0x2;
    }

    if (this->m_needsReset) {
        this->IXformSetViewport();
    }

    D3DCOLOR d3dColor = D3DCOLOR_RGBA(color.r, color.g, color.b, color.a);

    this->m_d3dDevice->Clear(0, nullptr, flags, d3dColor, 1.0f, 0);
}

void CGxDeviceD3d::ScenePresent() {
    if (this->m_context) {
        CGxDevice::ScenePresent();
        this->ISceneEnd();

        this->ICursorDraw();

        // TODO

        // TODO fixLag

        // TODO

        if (FAILED(this->m_d3dDevice->Present(nullptr, nullptr, nullptr, nullptr))) {
            this->m_context = 0;
        }

        // TODO stereo handling
    }

    this->ISceneBegin();
}

void CGxDeviceD3d::ShaderCreate(CGxShader* shaders[], EGxShTarget target, const char* a4, const char* a5, int32_t permutations) {
    CGxDevice::ShaderCreate(shaders, target, a4, a5, permutations);

    if (permutations == 1 && !shaders[0]->loaded) {
        this->IShaderCreate(shaders[0]);
    }
}

int32_t CGxDeviceD3d::StereoEnabled() {
    // return this->m_d3dStereoEnabled == 1;
    return 0;
}

void CGxDeviceD3d::XformSetProjection(const C44Matrix& matrix) {
    CGxDevice::XformSetProjection(matrix);
    this->IXformSetProjection(matrix);
}

