#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <assert.h>
#include <iostream>
#include <stdio.h>

#include "print_hi.h"

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;

class Vector2
{
public:
  float x, y;
  explicit Vector2(): x(0.0f), y(0.0f) {}
  explicit Vector2(float _x, float _y): x(_x), y(_y) {}
  Vector2(const Vector2& other): x(other.x), y(other.y) {}
  ~Vector2() {}

  // operator overloading
  Vector2 operator+(const Vector2& other) const 
  {
    return Vector2(x + other.x, y + other.y);
  }
  
  Vector2 operator-(const Vector2& other) const 
  {
    return Vector2(x - other.x, y - other.y);
  }

  Vector2 operator*(const Vector2& other) const 
  {
    return Vector2(x * other.x, y * other.y);
  }

  Vector2 operator*(const float& scale) const
  {
    return Vector2(scale * x, scale * y);
  }

  Vector2 operator+=(const Vector2& other)
  {
    *this = *this + other;
    return *this;
  }

  Vector2 operator-=(const Vector2& other)
  {
    *this = *this - other;
    return *this;
  }

  Vector2 operator*=(const float& scale)
  {
    x *= scale;
    y *= scale;
    return *this;
  }

  Vector2 operator/=(const float& scale)
  {
    assert(scale != 0.0f && "Vector2 cannot divide by zero");
    x /= scale;
    y /= scale;
    return *this;
  }

  friend std::ostream& operator<<(std::ostream& os, const Vector2& v)
  {
    os << "Vector2(" << v.x << ", " << v.y << ")";
    return os;
  }
};

// supports operations like 4.0f * Vector2(2.0f, 3.0f);
Vector2 operator*(float scalar, const Vector2& other)
{
  return Vector2(scalar * other.x, scalar * other.y);
}

class Rectangle
{
public:
  Vector2 position;
  Vector2 dimension;
  Vector2 velocity;

  Rectangle() {}
  Rectangle(const Rectangle& other): position(other.position), dimension(other.dimension), velocity(other.velocity) {}
  ~Rectangle() {}

  bool isOverlapping(const Rectangle& other)
  {
    bool isXOverlap = 
      position.x <= other.position.x + other.dimension.x && 
      other.position.x <= position.x + dimension.x;
    bool isYOverlap = 
      position.y <= other.position.y + other.dimension.y && 
      other.position.y <= position.y + dimension.y;

    return isXOverlap && isYOverlap;
  }
};

static Rectangle paddle1;
static Rectangle paddle2;
static Rectangle ball;

static bool isP1UpPressed = false;
static bool isP1DownPressed = false;
static bool isP2UpPressed = false;
static bool isP2DownPressed = false;

static int p1Score = 0;
static int p2Score = 0;

// measurement units used here: high resolution counter based on the processor's hardware counter
static Uint64 lastTime;
static Uint64 currentTime;

// this depends on the computer's hardware counter which is in charge of 
// providing high resolution counter
// Different processors may output different counts per second and from
// my own testing of 2 different kinds of computers, they return consistent counts per second frame by frame
const static Uint64 COUNTS_PER_SECOND = SDL_GetPerformanceFrequency();

#define TARGET_FPS 60.0f

// measured in milliseconds per frame
#define TARGET_DELTA_TIME ((float)SDL_MS_PER_SECOND / TARGET_FPS)
static float deltaTime = 0.0f;

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define BALL_SPEED 1000.0f
#define PADDLE_SPEED 1000.0f

// Runs once on game startup
// for appstate I can initialize it on the heap to add like OpenGL context
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
  HiSpace::printHi();
  SDL_WindowFlags windowFlags = (Uint64)NULL; // set no window flags
  bool isCreated = SDL_CreateWindowAndRenderer("Hello World", SCREEN_WIDTH, SCREEN_HEIGHT, windowFlags, &window, &renderer);
  if (!isCreated)
  {
    SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  paddle1.dimension = Vector2(20.0f, 80.0f);
  paddle2.dimension = Vector2(20.0f, 80.0f);
  ball.dimension = Vector2(20.0f, 20.0f);

  paddle1.position = Vector2(20.0f, (SCREEN_HEIGHT - paddle1.dimension.y) * 0.5f);
  paddle2.position = Vector2(SCREEN_WIDTH - 40.0f, (SCREEN_HEIGHT - paddle1.dimension.y) * 0.5f);
  ball.position = Vector2((SCREEN_WIDTH - ball.dimension.x) * 0.5f, (SCREEN_HEIGHT - ball.dimension.y) * 0.5f);

  ball.velocity = Vector2(BALL_SPEED, BALL_SPEED);

  deltaTime = TARGET_DELTA_TIME;

  return SDL_APP_CONTINUE;
}

// Runs when a new event occurs like input events
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
  if (event->type == SDL_EVENT_QUIT)
  {
    return SDL_APP_SUCCESS;
  }

  if (event->type == SDL_EVENT_KEY_DOWN)
  {
    SDL_Keycode key = event->key.key;
    switch (key)
    {
      case SDLK_W:
        isP1UpPressed = true;
        break;
      case SDLK_S:
        isP1DownPressed = true;
        break;
      case SDLK_UP:
        isP2UpPressed = true;
        break;
      case SDLK_DOWN:
        isP2DownPressed = true;
        break;
    }
  }

  if (event->type == SDL_EVENT_KEY_UP)
  {
    SDL_Keycode key = event->key.key;
    switch (key)
    {
      case SDLK_W:
        isP1UpPressed = false;
        break;
      case SDLK_S:
        isP1DownPressed = false;
        break;
      case SDLK_UP:
        isP2UpPressed = false;
        break;
      case SDLK_DOWN:
        isP2DownPressed = false;
        break;
    }
  }

  return SDL_APP_CONTINUE;
}

// Runs every frame
SDL_AppResult SDL_AppIterate(void* appstate)
{


  lastTime = SDL_GetPerformanceCounter();

  // multiply by -1 because y coordinates are inverted
  Vector2 up(0.0f, -PADDLE_SPEED);
  Vector2 down = -1.0f * up;

  // move logic
  if (isP1UpPressed)
  {
    paddle1.velocity = up;
  }
  else if (isP1DownPressed)
  {
    paddle1.velocity = down;
  }
  else
  {
    paddle1.velocity = Vector2(0.0f, 0.0f);
  }

  if (isP2UpPressed)
  {
    paddle2.velocity = up;
  }
  else if (isP2DownPressed)
  {
    paddle2.velocity = down;
  }
  else
  {
    paddle2.velocity = Vector2(0.0f, 0.0f);
  }


  float dtSeconds = deltaTime / SDL_MS_PER_SECOND;

  ball.position += ball.velocity * dtSeconds;
  
  paddle1.position += paddle1.velocity * dtSeconds;
  paddle2.position += paddle2.velocity * dtSeconds;

  // naive check
  if (paddle1.isOverlapping(ball))
  {
    ball.velocity.x *= -1.0f;
  }
  if (paddle2.isOverlapping(ball))
  {
    ball.velocity.x *= -1.0f;
  }

  // top and bottom bounds check
  if (ball.position.y <= 0.0f || ball.position.y + ball.dimension.y >= SCREEN_HEIGHT)
  {
    ball.velocity.y *= -1.0f;
  }

  // left and right bounds edge check
  if (ball.position.x <= 0.0f || ball.position.x + ball.dimension.x >= SCREEN_WIDTH)
  {
    ball.velocity.x *= -1.0f;
  }

  if (ball.position.x <= 0.0f)
  {
    p2Score += 1;
  }

  if (ball.position.x + ball.dimension.x >= SCREEN_WIDTH)
  {
    p1Score += 1;
  }


  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  
  SDL_FRect renderRects[3] {
    SDL_FRect { paddle1.position.x, paddle1.position.y, paddle1.dimension.x, paddle1.dimension.y },
    SDL_FRect { paddle2.position.x, paddle2.position.y, paddle2.dimension.x, paddle2.dimension.y },
    SDL_FRect { ball.position.x, ball.position.y, ball.dimension.x, ball.dimension.y }
  };
  
  SDL_RenderRects(renderer, renderRects, 3);

  float x1 = 80.0f;
  float y1 = 20.0f;
  char message1[256];
  snprintf(message1, sizeof(message1), "P1: %d", p1Score);

  float x2 = SCREEN_WIDTH - 160.0f;
  float y2 = 20.0f;
  char message2[256];
  snprintf(message2, sizeof(message2), "P2: %d", p2Score);

  SDL_RenderDebugText(renderer, x1, y1, message1);
  SDL_RenderDebugText(renderer, x2, y2, message2);



  SDL_RenderPresent(renderer);

  currentTime = SDL_GetPerformanceCounter();

  // use absolute value to ensure Uint64 wraparound is handled
  // might not realistically happen as an unsigned long on 64 bit systems
  // is 2^64 max exclusive upper bound and having hardware count up that high will take A LONG TIME
  Uint64 elapsedCount = lastTime > currentTime ?
    (lastTime - currentTime) : (currentTime - lastTime);

  deltaTime = SDL_MS_PER_SECOND * (float)elapsedCount / COUNTS_PER_SECOND;

  std::cout << "target deltaTime: " << TARGET_DELTA_TIME << std::endl;
  std::cout << "deltaTime: " << deltaTime << std::endl;
  std::cout << "dtSeconds: " << dtSeconds << std::endl;

  // IMPROVEMENT: to be more precise I can do a delay based on nanosecond granularity...

  // if work this frame is finished earlier than the target deltaTime
  // set a delay here to ensure all PCs operate at the same target FPS
  if (deltaTime < TARGET_DELTA_TIME)
  {
    Uint32 delayMs = (Uint32)(TARGET_DELTA_TIME - deltaTime);
    SDL_Delay(delayMs);
  }

  return SDL_APP_CONTINUE;
}

// Runs once on game exit
// if I have appstate initialized on the heap, here is the area where I can delete any resources that the appstate has that is
// also managed on the heap AND then delete appstate afterwards (like a destructor in C++)
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
  HiSpace::printHi();
  //SDL_DestroyRenderer(renderer);
  //SDL_DestroyWindow(window);
}


