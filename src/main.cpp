#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "print_hi.h"

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;

// Runs once on game startup
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
  HiSpace::printHi();
  bool isCreated = SDL_CreateWindowAndRenderer("Hello World", 800, 600, NULL, &window, &renderer);
  if (!isCreated)
  {
    SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  return SDL_APP_CONTINUE;
}

// Runs when a new event occurs like input events
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
  if (event->type == SDL_EVENT_KEY_DOWN || event->type == SDL_EVENT_QUIT)
  {
    return SDL_APP_SUCCESS;
  }

  return SDL_APP_CONTINUE;
}


// Runs every frame
SDL_AppResult SDL_AppIterate(void* appstate)
{
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);


  return SDL_APP_CONTINUE;
}

// Runs once on game exit
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
}
