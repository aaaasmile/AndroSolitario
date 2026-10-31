#include <SDL3/SDL.h>

int main() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    if (!SDL_CreateWindowAndRenderer("SDL3 C++ Harness", 800, 600, 0,
                                     &window, &renderer)) {
        SDL_Log("Could not create window and renderer: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT ||
                (event.type == SDL_EVENT_KEY_DOWN &&
                 event.key.key == SDLK_ESCAPE)) {
                running = false;
            }
        }

        const float x = static_cast<float>((SDL_GetTicks() / 4) % 720);
        const SDL_FRect rectangle{x, 270.0f, 80.0f, 60.0f};

        if (!SDL_SetRenderDrawColor(renderer, 24, 32, 48, SDL_ALPHA_OPAQUE) ||
            !SDL_RenderClear(renderer) ||
            !SDL_SetRenderDrawColor(renderer, 80, 190, 160, SDL_ALPHA_OPAQUE) ||
            !SDL_RenderFillRect(renderer, &rectangle) ||
            !SDL_RenderPresent(renderer)) {
            SDL_Log("Rendering failed: %s", SDL_GetError());
            running = false;
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
