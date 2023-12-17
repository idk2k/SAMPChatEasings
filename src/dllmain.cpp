#include <Windows.h>
#include <intrin.h>

#include <cstdint>
#include <iostream>

#include "ConsoleManager.h"
#include "EasingsHolder.h"
#include "MinHook/MinHook.h"
#include "Structures.h"
#include "UtilityHelper.h"
#include "d3d9.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#pragma execution_character_set("utf-8")

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#pragma intrinsic(_ReturnAddress)

bool initialize{false};
bool bMenuOpen{false};
bool bCursorShown{ false };

WNDPROC m_pWindowProc;

using ProtPresent = HRESULT(__stdcall*)(IDirect3DDevice9*, CONST RECT*, CONST RECT*, HWND, CONST RGNDATA*);
using ProtReset = HRESULT(__stdcall*)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
ProtReset Reset;
ProtPresent Present;
HRESULT __stdcall Hooked_Present(IDirect3DDevice9* pDevice, CONST RECT* pSrcRect, CONST RECT* pDestRect, HWND hDestWindow, CONST RGNDATA* pDirtyRegion);
HRESULT __stdcall Hooked_Reset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentParams);
void InstallD3DHook() {
    DWORD pDevice = *reinterpret_cast<DWORD*>(0xC97C28);
    void** vTable = *reinterpret_cast<void***>(pDevice);

    MH_CreateHook(vTable[17], Hooked_Present, reinterpret_cast<LPVOID*>(&Present));
    MH_EnableHook(vTable[17]);
    MH_CreateHook(vTable[16], Hooked_Reset, reinterpret_cast<LPVOID*>(&Reset));
    MH_EnableHook(vTable[16]);
}

DWORD SAMP_GAME_PTR = 0x21A10C;
DWORD SAMP_FUNC_ADDCLIENTCMD = 0x65AD0;
DWORD SAMP_FUNC_TOGGLECURSOR = 0x9BD30;
DWORD SAMP_FUNC_CURSORUNLOCKACTORCAM = 0x9BC10;


bool ToggleCursor(bool state);
HRESULT __stdcall Hooked_Present(IDirect3DDevice9* pDevice, CONST RECT* pSrcRect, CONST RECT* pDestRect, HWND hDestWindow, CONST RGNDATA* pDirtyRegion) {
    static bool once = false;
    if (!once) {
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(**reinterpret_cast<HWND**>(0xC17054));
#pragma warning(push)
#pragma warning(disable: 4996)
        std::string font{ getenv("WINDIR") }; font += "\\Fonts\\Tahoma.TTF";
#pragma warning(pop)
        ImGui::GetIO().Fonts->AddFontFromFileTTF(font.c_str(), 14.0f, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
        //Theme();
        ImGui_ImplDX9_Init(pDevice);
        once = true;
    }
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    if (bMenuOpen) {
        if (bCursorShown == 0) {
            bCursorShown = ToggleCursor(1);
        }
        int sx = *reinterpret_cast<int*>(0x00C17044), sy = *reinterpret_cast<int*>(0x00C17048);
        ImGui::SetNextWindowPos(ImVec2(sx / 2, sy / 2), ImGuiCond_FirstUseEver);
        ImGui::Begin("Настройки", &bMenuOpen, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);


        ImGui::End();
    }
    else if (bCursorShown == 1) {
        bCursorShown = ToggleCursor(0);
    }
    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
    return Present(pDevice, pSrcRect, pDestRect, hDestWindow, pDirtyRegion);
}

HRESULT __stdcall Hooked_Reset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentParams) {
    ImGui_ImplDX9_InvalidateDeviceObjects();
    return Reset(pDevice, pPresentParams);
}

struct Addresses {
    std::uintptr_t samp_base;
    std::uintptr_t g_chat;
    std::uintptr_t rakpeer_connect;
} addresses{};

bool ToggleCursor(bool state) {
    void* obj = *(void**)(addresses.samp_base + SAMP_GAME_PTR);
    ((void(__thiscall*) (void*, int, bool)) (addresses.samp_base + SAMP_FUNC_TOGGLECURSOR))(obj, state ? 3 : 0, !state);
    if (!state)
        ((void(__thiscall*) (void*)) (addresses.samp_base + SAMP_FUNC_CURSORUNLOCKACTORCAM))(obj);
    return state;
}

struct Parameters {
    bool bNewEntryAdded;
    bool running;
    int max_adjust_value;
    int increments;
    double prev_easing;
    int message_top_offset;
} global_params{false, false, 20, 0, .0f, 0};

using RecalcFontSize_t = void(__fastcall*)(void*, void*);
RecalcFontSize_t fpRecalcFontSize{};
auto __fastcall RecalcFontSizeHooked(void* reg_ecx, void* reg_edx) -> void {
    fpRecalcFontSize(reg_ecx, reg_edx);
    // recalc / get message top offset
    if (!addresses.g_chat) {
        utils::protect_safe_memory_copy(&addresses.g_chat, (void*)(addresses.samp_base + 0x21A0E4), 0x4);
    }
    if (memcmp(&global_params.message_top_offset, (void*)(addresses.g_chat + 0x63E2), 4) != 0) {
        memcpy(&global_params.message_top_offset, (void*)(addresses.g_chat + 0x63E2), 4);
        global_params.message_top_offset *= 9;
        global_params.message_top_offset += 19;
    }
}

using AddEntry_t = void(__fastcall*)(void*, void*, int, const char*, const char*, structures::D3DCOLOR,
                                     structures::D3DCOLOR);
AddEntry_t fpAddEntry{};
auto __fastcall AddEntryHooked(void* reg_ecx, void* reg_edx, int nType, const char* szText, const char* szPrefix,
                               structures::D3DCOLOR textColor, structures::D3DCOLOR prefixColor) -> void {
    global_params.bNewEntryAdded = true;
    fpAddEntry(reg_ecx, reg_edx, nType, szText, szPrefix, textColor, prefixColor);
}

using RenderEntry_t = void(__fastcall*)(void*, void*, const char*, structures::CRect, structures::D3DCOLOR);
RenderEntry_t fpRenderEntry{};
auto __fastcall RenderEntryHooked(void* reg_ecx, void* reg_edx, const char* szText, structures::CRect rect,
                                  structures::D3DCOLOR color) -> void {
    if (global_params.message_top_offset && global_params.bNewEntryAdded &&
        rect.top == global_params.message_top_offset) {
        global_params.bNewEntryAdded = false;
        if (!global_params.running) {
            global_params.running = true;
            global_params.increments = 0;
        }
    }

    if (global_params.running && rect.top == global_params.message_top_offset) {
        if (global_params.increments >= 250) {
            global_params.running = false;
        }
        double easing = global_params.max_adjust_value *
                        EasingsHolder::get_instance().easeOutBounce(global_params.increments * (0.004));
        rect.top += global_params.max_adjust_value - long(easing);
        rect.bottom += global_params.max_adjust_value - long(easing);
        global_params.prev_easing = easing;
        global_params.increments += 1;
    }

    fpRenderEntry(reg_ecx, reg_edx, szText, rect, color);
}

LRESULT __stdcall WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_KEYDOWN: {
            switch (wParam) {
                case VK_F12: {
                    bMenuOpen = !bMenuOpen;
                    break;
                }
            }
            break;
        }
    }
    if (bMenuOpen) {
        if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) {
            return true;
        }
        ImGuiIO& io = ImGui::GetIO();
        if (io.WantCaptureMouse || io.WantCaptureKeyboard) {
            return true;
        }
    }
    return CallWindowProcA(m_pWindowProc, hWnd, msg, wParam, lParam);
}

using RakPeer_Connect = bool(__fastcall*)(void*, void*, const char*, unsigned short, char*, int);
RakPeer_Connect fpConnect{};
auto __fastcall RakPeerConnectHooked(void* reg_ecx, void* reg_edx, const char* host, unsigned short port,
                                     char* passwordData, int passwordDataLength) -> bool {
    if (!initialize) {
        // wndproc hook
        m_pWindowProc = reinterpret_cast<WNDPROC>(
            SetWindowLongA(*reinterpret_cast<HWND*>(0xC97C1CU), GWL_WNDPROC, reinterpret_cast<LONG>(WndProcHandler)));
        // d3d9 hook (present, reset)
        InstallD3DHook();

        // force install m_bRedraw flag to 1
        utils::protect_safe_memory_set((void*)(addresses.samp_base + 0x6441C + 6U), 1, 1);

        // get message top offset
        if (!addresses.g_chat) {
            utils::protect_safe_memory_copy(&addresses.g_chat, (void*)(addresses.samp_base + 0x21A0E4), 0x4);
        }
        if (memcmp(&global_params.message_top_offset, (void*)(addresses.g_chat + 0x63E2), 4) != 0) {
            memcpy(&global_params.message_top_offset, (void*)(addresses.g_chat + 0x63E2), 4);
            global_params.message_top_offset *= 9;
            global_params.message_top_offset += 19;
        }

        utils::MH_CreateAndEnableHook(static_cast<std::uintptr_t>(addresses.samp_base + 0x638A0), &RenderEntryHooked,
                                      reinterpret_cast<LPVOID*>(&fpRenderEntry));
        utils::MH_CreateAndEnableHook(static_cast<std::uintptr_t>(addresses.samp_base + 0x64010), &AddEntryHooked,
                                      reinterpret_cast<LPVOID*>(&fpAddEntry));
        utils::MH_CreateAndEnableHook(static_cast<std::uintptr_t>(addresses.samp_base + 0x63550), &RecalcFontSizeHooked,
                                      reinterpret_cast<LPVOID*>(&fpRecalcFontSize));

        initialize = true;
    }
    return fpConnect(reg_ecx, reg_edx, host, port, passwordData, passwordDataLength);
}

auto WINAPI DllMain(HINSTANCE dllinstance, DWORD reason, LPVOID lpvReserved) -> BOOL {
    UNREFERENCED_PARAMETER(lpvReserved);
    switch (reason) {
        case DLL_PROCESS_ATTACH: {
            DisableThreadLibraryCalls(dllinstance);
            if (ConsoleManager::get_instance().create_console() == FALSE) {
                std::cout << "[ERROR]: ConsoleManager::get_instance().create_console() failed" << std::endl;
                return FALSE;
            }
            if (MH_STATUS mh_init_status = MH_Initialize(); mh_init_status != MH_OK) {
                std::cout << "[ERROR]: MH_Initialize() failed, return code: " << mh_init_status << std::endl;
                return FALSE;
            }
            auto samp_handle = GetModuleHandleW(L"samp.dll");
            if (!samp_handle) {
                std::cout << "[ERROR]: samp_handle getting failed" << std::endl;
                return FALSE;
            }
            addresses.samp_base = reinterpret_cast<std::uintptr_t>(samp_handle);
            addresses.rakpeer_connect = 0x3ABB0;
            if (!addresses.rakpeer_connect) {
                std::cout << "[ERROR]: no RakPeer__Connect address" << std::endl;
                return FALSE;
            }
            utils::MH_CreateAndEnableHook(static_cast<std::uintptr_t>(addresses.samp_base + addresses.rakpeer_connect),
                                          &RakPeerConnectHooked, reinterpret_cast<LPVOID*>(&fpConnect));
            break;
        }
    }
    return TRUE;
}