#include <ctime>
#include <iostream>
#include <SDL2/SDL.h>

// screen res
#define VGA_H_RES 640
#define VGA_V_RES 480

// for SDL texture
typedef struct Pixel
{
    uint8_t a; // transparency
    uint8_t b; // blue
    uint8_t g; // green
    uint8_t r; // red
} Pixel;

// refer to https://fruttenboel.verhoeven272.nl/Oberon/sdl-ps2.c.html
typedef struct KdbInfo
{
    uint8_t code;
    uint8_t type;
} KdbInfo;

enum KdbKind
{
    K_UNKNOWN = 0,
    K_NORMAL,
    K_EXTENDED,
    K_NUMLOCK,
    K_SHIFT,
};

enum KdbCodeType
{
    K_MAKE = 0,
    K_BREAK,
};

static KdbInfo keymap[] =
    {
        [0] = {0x00, K_UNKNOWN},
        [1] = {0x00, K_UNKNOWN},
        [2] = {0x00, K_UNKNOWN},
        [3] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_A] = {0x1C, K_NORMAL},
        [SDL_SCANCODE_B] = {0x32, K_NORMAL},
        [SDL_SCANCODE_C] = {0x21, K_NORMAL},
        [SDL_SCANCODE_D] = {0x23, K_NORMAL},
        [SDL_SCANCODE_E] = {0x24, K_NORMAL},
        [SDL_SCANCODE_F] = {0x2B, K_NORMAL},
        [SDL_SCANCODE_G] = {0x34, K_NORMAL},
        [SDL_SCANCODE_H] = {0x33, K_NORMAL},
        [SDL_SCANCODE_I] = {0x43, K_NORMAL},
        [SDL_SCANCODE_J] = {0x3B, K_NORMAL},
        [SDL_SCANCODE_K] = {0x42, K_NORMAL},
        [SDL_SCANCODE_L] = {0x4B, K_NORMAL},
        [SDL_SCANCODE_M] = {0x3A, K_NORMAL},
        [SDL_SCANCODE_N] = {0x31, K_NORMAL},
        [SDL_SCANCODE_O] = {0x44, K_NORMAL},
        [SDL_SCANCODE_P] = {0x4D, K_NORMAL},
        [SDL_SCANCODE_Q] = {0x15, K_NORMAL},
        [SDL_SCANCODE_R] = {0x2D, K_NORMAL},
        [SDL_SCANCODE_S] = {0x1B, K_NORMAL},
        [SDL_SCANCODE_T] = {0x2C, K_NORMAL},
        [SDL_SCANCODE_U] = {0x3C, K_NORMAL},
        [SDL_SCANCODE_V] = {0x2A, K_NORMAL},
        [SDL_SCANCODE_W] = {0x1D, K_NORMAL},
        [SDL_SCANCODE_X] = {0x22, K_NORMAL},
        [SDL_SCANCODE_Y] = {0x35, K_NORMAL},
        [SDL_SCANCODE_Z] = {0x1A, K_NORMAL},

        [SDL_SCANCODE_1] = {0x16, K_NORMAL},
        [SDL_SCANCODE_2] = {0x1E, K_NORMAL},
        [SDL_SCANCODE_3] = {0x26, K_NORMAL},
        [SDL_SCANCODE_4] = {0x25, K_NORMAL},
        [SDL_SCANCODE_5] = {0x2E, K_NORMAL},
        [SDL_SCANCODE_6] = {0x36, K_NORMAL},
        [SDL_SCANCODE_7] = {0x3D, K_NORMAL},
        [SDL_SCANCODE_8] = {0x3E, K_NORMAL},
        [SDL_SCANCODE_9] = {0x46, K_NORMAL},
        [SDL_SCANCODE_0] = {0x45, K_NORMAL},

        [SDL_SCANCODE_RETURN] = {0x5A, K_NORMAL},
        [SDL_SCANCODE_ESCAPE] = {0x76, K_NORMAL},
        [SDL_SCANCODE_BACKSPACE] = {0x66, K_NORMAL},
        [SDL_SCANCODE_TAB] = {0x0D, K_NORMAL},
        [SDL_SCANCODE_SPACE] = {0x29, K_NORMAL},

        [SDL_SCANCODE_MINUS] = {0x4E, K_NORMAL},
        [SDL_SCANCODE_EQUALS] = {0x55, K_NORMAL},
        [SDL_SCANCODE_LEFTBRACKET] = {0x54, K_NORMAL},
        [SDL_SCANCODE_RIGHTBRACKET] = {0x5B, K_NORMAL},
        [SDL_SCANCODE_BACKSLASH] = {0x5D, K_NORMAL},
        [SDL_SCANCODE_NONUSHASH] = {0x5D, K_NORMAL}, // same key as BACKSLASH

        [SDL_SCANCODE_SEMICOLON] = {0x4C, K_NORMAL},
        [SDL_SCANCODE_APOSTROPHE] = {0x52, K_NORMAL},
        [SDL_SCANCODE_GRAVE] = {0x0E, K_NORMAL},
        [SDL_SCANCODE_COMMA] = {0x41, K_NORMAL},
        [SDL_SCANCODE_PERIOD] = {0x49, K_NORMAL},
        [SDL_SCANCODE_SLASH] = {0x4A, K_NORMAL},

        [SDL_SCANCODE_CAPSLOCK] = {0x58, K_NORMAL},

        [SDL_SCANCODE_F1] = {0x05, K_NORMAL},
        [SDL_SCANCODE_F2] = {0x06, K_NORMAL},
        [SDL_SCANCODE_F3] = {0x04, K_NORMAL},
        [SDL_SCANCODE_F4] = {0x0C, K_NORMAL},
        [SDL_SCANCODE_F5] = {0x03, K_NORMAL},
        [SDL_SCANCODE_F6] = {0x0B, K_NORMAL},
        [SDL_SCANCODE_F7] = {0x83, K_NORMAL},
        [SDL_SCANCODE_F8] = {0x0A, K_NORMAL},
        [SDL_SCANCODE_F9] = {0x01, K_NORMAL},
        [SDL_SCANCODE_F10] = {0x09, K_NORMAL},
        [SDL_SCANCODE_F11] = {0x78, K_NORMAL},
        [SDL_SCANCODE_F12] = {0x07, K_NORMAL},

        // Most of the keys below are not used
        [SDL_SCANCODE_PRINTSCREEN] = {0x7C, K_NUMLOCK},
        [SDL_SCANCODE_SCROLLLOCK] = {0x7E, K_NORMAL},
        [SDL_SCANCODE_PAUSE] = {0x00, K_UNKNOWN}, // complex
        [SDL_SCANCODE_INSERT] = {0x70, K_EXTENDED},
        [SDL_SCANCODE_HOME] = {0x6C, K_EXTENDED},
        [SDL_SCANCODE_PAGEUP] = {0x7D, K_EXTENDED},
        [SDL_SCANCODE_DELETE] = {0x71, K_EXTENDED},
        [SDL_SCANCODE_END] = {0x69, K_EXTENDED},
        [SDL_SCANCODE_PAGEDOWN] = {0x7A, K_EXTENDED},
        [SDL_SCANCODE_RIGHT] = {0x74, K_EXTENDED},
        [SDL_SCANCODE_LEFT] = {0x6B, K_EXTENDED},
        [SDL_SCANCODE_DOWN] = {0x72, K_EXTENDED},
        [SDL_SCANCODE_UP] = {0x75, K_EXTENDED},

        [SDL_SCANCODE_NUMLOCKCLEAR] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_KP_DIVIDE] = {0x4A, K_EXTENDED},
        [SDL_SCANCODE_KP_MULTIPLY] = {0x7C, K_NORMAL},
        [SDL_SCANCODE_KP_MINUS] = {0x7B, K_NORMAL},
        [SDL_SCANCODE_KP_PLUS] = {0x79, K_NORMAL},
        [SDL_SCANCODE_KP_ENTER] = {0x5A, K_EXTENDED},
        [SDL_SCANCODE_KP_1] = {0x69, K_NORMAL},
        [SDL_SCANCODE_KP_2] = {0x72, K_NORMAL},
        [SDL_SCANCODE_KP_3] = {0x7A, K_NORMAL},
        [SDL_SCANCODE_KP_4] = {0x6B, K_NORMAL},
        [SDL_SCANCODE_KP_5] = {0x73, K_NORMAL},
        [SDL_SCANCODE_KP_6] = {0x74, K_NORMAL},
        [SDL_SCANCODE_KP_7] = {0x6C, K_NORMAL},
        [SDL_SCANCODE_KP_8] = {0x75, K_NORMAL},
        [SDL_SCANCODE_KP_9] = {0x7D, K_NORMAL},
        [SDL_SCANCODE_KP_0] = {0x70, K_NORMAL},
        [SDL_SCANCODE_KP_PERIOD] = {0x71, K_NORMAL},

        [SDL_SCANCODE_NONUSBACKSLASH] = {0x61, K_NORMAL},
        [SDL_SCANCODE_APPLICATION] = {0x2F, K_EXTENDED},
        [SDL_SCANCODE_POWER] = {0x00, K_UNKNOWN},

        [SDL_SCANCODE_KP_EQUALS] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_F13] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_F14] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_F15] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_F16] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_F17] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_F18] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_F19] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_F20] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_F21] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_F22] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_F23] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_F24] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_EXECUTE] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_HELP] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_MENU] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_SELECT] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_STOP] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_AGAIN] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_UNDO] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_CUT] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_COPY] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_PASTE] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_FIND] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_MUTE] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_VOLUMEUP] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_VOLUMEDOWN] = {0x00, K_UNKNOWN},

        [130] = {0x00, K_UNKNOWN},
        [131] = {0x00, K_UNKNOWN},
        [132] = {0x00, K_UNKNOWN},

        [SDL_SCANCODE_KP_COMMA] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_KP_EQUALSAS400] = {0x00, K_UNKNOWN},

        [SDL_SCANCODE_INTERNATIONAL1] = {0x00, K_UNKNOWN},

        [SDL_SCANCODE_INTERNATIONAL2] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_INTERNATIONAL3] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_INTERNATIONAL4] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_INTERNATIONAL5] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_INTERNATIONAL6] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_INTERNATIONAL7] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_INTERNATIONAL8] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_INTERNATIONAL9] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_LANG1] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_LANG2] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_LANG3] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_LANG4] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_LANG5] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_LANG6] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_LANG7] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_LANG8] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_LANG9] = {0x00, K_UNKNOWN},

        [SDL_SCANCODE_ALTERASE] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_SYSREQ] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_CANCEL] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_CLEAR] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_PRIOR] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_RETURN2] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_SEPARATOR] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_OUT] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_OPER] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_CLEARAGAIN] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_CRSEL] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_EXSEL] = {0x00, K_UNKNOWN},

        [165] = {0x00, K_UNKNOWN},
        [166] = {0x00, K_UNKNOWN},
        [167] = {0x00, K_UNKNOWN},
        [168] = {0x00, K_UNKNOWN},
        [169] = {0x00, K_UNKNOWN},
        [170] = {0x00, K_UNKNOWN},
        [171] = {0x00, K_UNKNOWN},
        [172] = {0x00, K_UNKNOWN},
        [173] = {0x00, K_UNKNOWN},
        [174] = {0x00, K_UNKNOWN},
        [175] = {0x00, K_UNKNOWN},

        [SDL_SCANCODE_KP_00] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_KP_000] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_THOUSANDSSEPARATOR] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_DECIMALSEPARATOR] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_CURRENCYUNIT] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_CURRENCYSUBUNIT] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_KP_LEFTPAREN] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_KP_RIGHTPAREN] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_KP_LEFTBRACE] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_KP_RIGHTBRACE] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_KP_TAB] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_KP_BACKSPACE] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_KP_A] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_KP_B] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_KP_C] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_KP_D] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_KP_E] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_KP_F] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_KP_XOR] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_KP_POWER] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_KP_PERCENT] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_KP_LESS] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_KP_GREATER] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_KP_AMPERSAND] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_KP_DBLAMPERSAND] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_KP_VERTICALBAR] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_KP_DBLVERTICALBAR] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_KP_COLON] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_KP_HASH] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_KP_SPACE] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_KP_AT] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_KP_EXCLAM] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_KP_MEMSTORE] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_KP_MEMRECALL] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_KP_MEMCLEAR] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_KP_MEMADD] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_KP_MEMSUBTRACT] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_KP_MEMMULTIPLY] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_KP_MEMDIVIDE] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_KP_PLUSMINUS] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_KP_CLEAR] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_KP_CLEARENTRY] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_KP_BINARY] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_KP_OCTAL] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_KP_DECIMAL] = {0x00, K_UNKNOWN},
        [SDL_SCANCODE_KP_HEXADECIMAL] = {0x00, K_UNKNOWN},

        [222] = {0x00, K_UNKNOWN},
        [223] = {0x00, K_UNKNOWN},

        [SDL_SCANCODE_LCTRL] = {0x14, K_NORMAL},
        [SDL_SCANCODE_LSHIFT] = {0x12, K_NORMAL},
        [SDL_SCANCODE_LALT] = {0x11, K_NORMAL},
        [SDL_SCANCODE_LGUI] = {0x1F, K_EXTENDED},
        [SDL_SCANCODE_RCTRL] = {0x14, K_EXTENDED},
        [SDL_SCANCODE_RSHIFT] = {0x59, K_NORMAL},
        [SDL_SCANCODE_RALT] = {0x11, K_EXTENDED},
        [SDL_SCANCODE_RGUI] = {0x27, K_EXTENDED},
};

// extern "C" void ps2_read(char dat);

class MediaWindow
{
public:
    MediaWindow()
    {
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            std::cout << "SDL could not be initialized: " << SDL_GetError();
        }
        else
        {
            std::cout << "SDL video system is ready to go" << std::endl;
        }

        win = SDL_CreateWindow("PS/2 & VGA Test",
                               SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED,
                               VGA_H_RES,
                               VGA_V_RES,
                               SDL_WINDOW_SHOWN);

        // NOTE:  need to handle excpt cond
        if (!win)
        {
            printf("Window creation failed: %s\n", SDL_GetError());
        }
        rdr = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
        if (!rdr)
        {
            printf("Renderer creation failed: %s\n", SDL_GetError());
        }
        txr = SDL_CreateTexture(rdr, SDL_PIXELFORMAT_RGBA8888,
                                SDL_TEXTUREACCESS_TARGET, VGA_H_RES, VGA_V_RES);
        if (!txr)
        {
            printf("Texture creation failed: %s\n", SDL_GetError());
        }
    }

    void initFPS()
    {
        startTick = SDL_GetPerformanceCounter();
        frameCnt = 0;
    }

    void calcFPS()
    {
        uint64_t endTick = SDL_GetPerformanceCounter();
        double duration = ((double)(endTick - startTick)) / SDL_GetPerformanceFrequency();
        double fps = (double)frameCnt / duration;
        std::cout << "Frames per second: " << fps << std::endl;
    }

    void initFB()
    {
        for (int cnt = 0; cnt < VGA_V_RES; ++cnt)
            for (int j = 0; j < VGA_H_RES; ++j)
            {
                auto p = &fb[cnt * VGA_H_RES + j];
                p->a = 0xFF;
                p->b = 0xFF;
                p->g = 0x00;
                p->r = 0x00;
            }
    }

    // now dont support break code
    void encode(int sdlCode, KdbCodeType codeType = K_MAKE)
    {
        int cnt = 0;
        auto info = keymap[sdlCode];
        switch (info.type)
        {
        case K_UNKNOWN:
        {
            break;
        }
        case K_NORMAL:
        {
            if (codeType == K_BREAK)
            {
                kdbCode[cnt++] = 0xF0;
            }
            kdbCode[cnt++] = info.code;
            break;
        }
        case K_EXTENDED:
        {
            kdbCode[cnt++] = 0xE0;
            if (codeType == K_BREAK)
            {
                kdbCode[cnt++] = 0xF0;
            }
            kdbCode[cnt++] = info.code;
            break;
        }
        case K_NUMLOCK:
        {
            // assumes num lock is always active
            if (codeType == K_MAKE)
            {
                // fake shift press
                kdbCode[cnt++] = 0xE0;
                kdbCode[cnt++] = 0x12;
                kdbCode[cnt++] = 0xE0;
                kdbCode[cnt++] = info.code;
            }
            else
            {
                kdbCode[cnt++] = 0xE0;
                kdbCode[cnt++] = 0xF0;
                kdbCode[cnt++] = info.code;
                // fake shift release
                kdbCode[cnt++] = 0xE0;
                kdbCode[cnt++] = 0xF0;
                kdbCode[cnt++] = 0x12;
            }
            break;
        }
        case K_SHIFT:
        {
            SDL_Keymod mod = SDL_GetModState();
            if (codeType == K_MAKE)
            {
                // fake shift release
                if (mod & KMOD_LSHIFT)
                {
                    kdbCode[cnt++] = 0xE0;
                    kdbCode[cnt++] = 0xF0;
                    kdbCode[cnt++] = 0x12;
                }
                if (mod & KMOD_RSHIFT)
                {
                    kdbCode[cnt++] = 0xE0;
                    kdbCode[cnt++] = 0xF0;
                    kdbCode[cnt++] = 0x59;
                }
                kdbCode[cnt++] = 0xE0;
                kdbCode[cnt++] = info.code;
            }
            else
            {
                kdbCode[cnt++] = 0xE0;
                kdbCode[cnt++] = 0xF0;
                kdbCode[cnt++] = info.code;
                // fake shift press
                if (mod & KMOD_RSHIFT)
                {
                    kdbCode[cnt++] = 0xE0;
                    kdbCode[cnt++] = 0x59;
                }
                if (mod & KMOD_LSHIFT)
                {
                    kdbCode[cnt++] = 0xE0;
                    kdbCode[cnt++] = 0x12;
                }
            }
            break;
        }
        }
    }

    bool step()
    {
        SDL_Event event;
        if (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                return false;
            }
            const Uint8 *kdbState = SDL_GetKeyboardState(NULL);
            int keyLen = sizeof(keymap) / sizeof(KdbInfo);
            for (int i = 0; i < keyLen; ++i)
            {
                if (kdbState[i])
                {
                    std::cout << i << std::endl;
                    encode(i);
                    // ps2_read(kdbCode[0]);
                }
            }
        }

        SDL_UpdateTexture(txr, NULL, fb, VGA_H_RES * sizeof(Pixel));
        SDL_RenderClear(rdr);
        SDL_RenderCopy(rdr, txr, NULL, NULL);
        SDL_RenderPresent(rdr);
        frameCnt++;
        return true;
    }

    void release()
    {
        SDL_DestroyTexture(txr);
        SDL_DestroyRenderer(rdr);
        SDL_DestroyWindow(win);
        SDL_Delay(500);
        SDL_Quit();
    }

private:
    SDL_Window *win = nullptr;
    SDL_Renderer *rdr = nullptr;
    SDL_Texture *txr = nullptr;
    clock_t start;
    uint8_t kdbCode[6] = {0};
    Pixel fb[VGA_H_RES * VGA_V_RES];
    uint64_t startTick = 0;
    uint64_t frameCnt = 0;
};