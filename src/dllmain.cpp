#include <iostream>
#include <Windows.h>
#include <cstdint>
#include "MinHook/MinHook.h"
#include "Structures.h"
#include "ConsoleManager.h"
#include "EasingsHolder.h"
#include <intrin.h>
#include <chrono>
#include <thread>

DWORD dwSAMP = 0;
DWORD RakPeer__Connect = 0;
bool Inited = false;

#pragma intrinsic(_ReturnAddress)

void MH_CreateAndEnableHook(unsigned __int32&& TargetAddress, LPVOID pDetour, LPVOID* ppOriginal) {
    MH_CreateHook(reinterpret_cast<LPVOID>(TargetAddress), pDetour, ppOriginal);
    MH_EnableHook(reinterpret_cast<LPVOID>(TargetAddress));
}

struct RenderEntryParametes {
    RenderEntryParametes(void* _dis, void* _EDX, const char* _szText, CRect _rect, D3DCOLOR _color) : dis{ _dis }, EDX{ _EDX }, color{ _color }, rect{ _rect }, szText{_szText} {}
    void* dis;
    void* EDX;
    const char* szText;
    D3DCOLOR color;
    CRect rect;
};

/*
* F L A G S 
* FOR USAGE
*/
bool bNewEntryAdded = false;
bool start = false;
int max_adjust_value = 20;
int increments = 0;
double prev_easing = 0;
// offset for last message
int message_top_offset = 0;


typedef void(__fastcall* RecalcFontSize_t)(void*, void*);
RecalcFontSize_t fpRecalcFontSize = 0;
void __fastcall HOOK_RecalcFontSize(void* dis, void* EDX) {
    fpRecalcFontSize(dis, EDX);
    // recalc / get message top offset
    static DWORD g_chat = 0;
    static DWORD oldProtect{};
    if (!g_chat) {
        VirtualProtect((void*)(dwSAMP + 0x21A0E4), 4, PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy(&g_chat, (void*)(dwSAMP + 0x21A0E4), 4);
        VirtualProtect((void*)(dwSAMP + 0x21A0E4), 4, oldProtect, &oldProtect);
    }
    memcpy(&message_top_offset, (void*)(g_chat + 0x63E2), 4);
    message_top_offset *= 9;
    message_top_offset += 19;
}

typedef void(__fastcall* AddEntry_t)(void*, void*, int, const char*, const char*, D3DCOLOR, D3DCOLOR);
AddEntry_t fpAddEntry = 0;
void __fastcall HOOK_AddEntry(void* dis, void* EDX, int nType, const char* szText, const char* szPrefix, D3DCOLOR textColor, D3DCOLOR prefixColor) {
    bNewEntryAdded = true;
    fpAddEntry(dis, EDX, nType, szText, szPrefix, textColor, prefixColor);
}

int time_duration = 1000;
int start_time_point = 0;

typedef void(__fastcall* RenderEntry_t)(void*, void*, const char*, CRect, D3DCOLOR);
RenderEntry_t fpRenderEntry = 0;
void __fastcall HOOK_RenderEntry(void* dis, void* EDX, const char* szText, CRect rect, D3DCOLOR color) {

    //std::cout << "rect.top: " << rect.top << "\t\trect.bottom: " << rect.bottom << std::endl;
    std::cout << message_top_offset << std::endl;

    if (message_top_offset && bNewEntryAdded && rect.top == message_top_offset) {
        bNewEntryAdded = false;
        if (!start) {
            start = true;
            increments = 0;
        }
    }

    if (start && rect.top == message_top_offset) {
        if (increments == 250) {
            start = false;
        }
        double easing = max_adjust_value * EasingsHolder::get_instance().easeOutBounce(increments * (0.004));
        rect.top += max_adjust_value - long(easing);
        rect.bottom += max_adjust_value - long(easing);
        prev_easing = easing;
        increments += 1;
    }

    fpRenderEntry(dis, EDX, szText, rect, color);
}



typedef bool(__fastcall* RakPeer_Connect)(void*, void*, const char*, unsigned short, char*, int);
RakPeer_Connect fpConnect = 0;
bool __fastcall HOOK_RakPeer_Connect(void* dis, void* EDX, const char* host, unsigned short port, char* passwordData, int passwordDataLength) {
    if (Inited == false) {
        // hard install m_bRedraw flag to 1
        DWORD oldProtect{};
        VirtualProtect((void*)(dwSAMP + 0x6441C + 6U), 1, PAGE_EXECUTE_READWRITE, &oldProtect);
        memset((void*)(dwSAMP + 0x6441C + 6U), 0x01, 1);
        VirtualProtect((void*)(dwSAMP + 0x6441C + 6U), 1, oldProtect, &oldProtect);
        
        // get message top offset
        DWORD g_chat = 0;
        VirtualProtect((void*)(dwSAMP + 0x21A0E4), 4, PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy(&g_chat, (void*)(dwSAMP + 0x21A0E4), 4);
        VirtualProtect((void*)(dwSAMP + 0x21A0E4), 4, oldProtect, &oldProtect);
        memcpy(&message_top_offset, (void*)(g_chat + 0x63E2), 4);
        message_top_offset *= 9;
        message_top_offset += 19;

        // Hook on samp.dll - RenderEntry()
        MH_CreateHook(reinterpret_cast<LPVOID>(dwSAMP + 0x638A0), &HOOK_RenderEntry, reinterpret_cast<LPVOID*>(&fpRenderEntry));
        MH_EnableHook(reinterpret_cast<LPVOID>(dwSAMP + 0x638A0));

        // Hook on samp.dll - AddEntry()
        MH_CreateHook(reinterpret_cast<LPVOID>(dwSAMP + 0x64010), &HOOK_AddEntry, reinterpret_cast<LPVOID*>(&fpAddEntry));
        MH_EnableHook(reinterpret_cast<LPVOID>(dwSAMP + 0x64010));

        // Hook on samp.dll - RecalcFontSize()
        MH_CreateHook(reinterpret_cast<LPVOID>(dwSAMP + 0x63550), &HOOK_RecalcFontSize, reinterpret_cast<LPVOID*>(&fpRecalcFontSize));
        MH_EnableHook(reinterpret_cast<LPVOID>(dwSAMP + 0x63550));

        Inited = true;
    }
    return fpConnect(dis, EDX, host, port, passwordData, passwordDataLength);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    UNREFERENCED_PARAMETER(lpvReserved);
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH: {
        DisableThreadLibraryCalls(hinstDLL);
        ConsoleManager::get_instance().create_console();
        MH_Initialize();
        auto hSAMP = GetModuleHandle(L"samp.dll");
        if (hSAMP) {
            dwSAMP = reinterpret_cast<DWORD>(hSAMP);
            RakPeer__Connect = 0x3ABB0;
            if (RakPeer__Connect != 0) {
                MH_STATUS rkpr_hook_status = MH_CreateHook(reinterpret_cast<LPVOID>(dwSAMP + RakPeer__Connect), &HOOK_RakPeer_Connect, reinterpret_cast<LPVOID*>(&fpConnect));
                rkpr_hook_status = MH_EnableHook(reinterpret_cast<LPVOID>(dwSAMP + RakPeer__Connect));
            }
        }
        break;
    }
    }
    return TRUE;
}