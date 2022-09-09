#include <iostream>
#include <SDL2/SDL.h>

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
                                  0,
                                  2500,
                                  640,
                                  480,
                                  SDL_WINDOW_SHOWN);
    }

    void run()
    {
        bool gameIsRunning = true;
        while (gameIsRunning)
        {
            SDL_Event event;
            // Start our event loop
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                {
                    gameIsRunning = false;
                }
                else if (event.type == SDL_KEYDOWN)
                {
                    std::cout << "a key has been pressed" << std::endl;;
                    if (event.key.keysym.sym == SDLK_0)
                    {
                        std::cout << "0 was pressed" << std::endl;;
                    }
                }
                // Retrieve the state of all of the keys
                // Then we can query the scan code of one or more
                // keys at a time
                const Uint8 *state = SDL_GetKeyboardState(NULL);
                if (state[SDL_SCANCODE_RIGHT])
                {
                    std::cout << "right arrow key is pressed" << std::endl;;
                }
            }
        }

        SDL_DestroyWindow(win);
        SDL_Delay(3000);
        SDL_Quit();
    }

private:
    SDL_Window *win = nullptr;
};