#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define WIDTH 480
#define HEIGHT 360
#define WINDOW_SCALE 2

// Structured application parameters
typedef struct {
    float A, B, C;             // Rotation angles
    float cubeWidth;           // Half-width dimension of the cube
    float distanceFromCamera;  // Z-translation offset
    float K1;                  // Projection focal scale factor
    float incrementSpeed;      // Step size for surface point generation
} EngineState;

// Calculate 3D space transformations using classic rotation matrices
static inline float calculateX(float i, float j, float k, float A, float B, float C) {
    return j * sinf(A) * sinf(B) * cosf(C) - k * cosf(A) * sinf(B) * cosf(C) +
           j * cosf(A) * sinf(C) + k * sinf(A) * sinf(C) + i * cosf(B) * cosf(C);
}

static inline float calculateY(float i, float j, float k, float A, float B, float C) {
    return j * cosf(A) * cosf(C) + k * sinf(A) * cosf(C) -
           j * sinf(A) * sinf(B) * sinf(C) + k * cosf(A) * sinf(B) * sinf(C) -
           i * cosf(B) * sinf(C);
}

static inline float calculateZ(float i, float j, float k, float A, float B) {
    return k * cosf(A) * cosf(B) - j * sinf(A) * cosf(B) + i * sinf(B);
}

// Maps specific face characters to 32-bit ARGB hex color channels
static inline uint32_t getColor(unsigned char ch) {
    switch (ch) {
        case '@': return 0xFFFF0000; // Red
        case '$': return 0xFF00FF00; // Green
        case '~': return 0xFF0000FF; // Blue
        case '#': return 0xFFFFFF00; // Yellow
        case ';': return 0xFF00FFFF; // Cyan
        case '+': return 0xFFFF00FF; // Magenta
        default:  return 0xFFFFFFFF; // White
    }
}

// Render pipeline step processing points onto the z-buffer and texture target
void calculateForSurface(EngineState *state, float cubeX, float cubeY, float cubeZ, 
                         unsigned char ch, float *zBuffer, uint32_t *pixelBuffer) {
    float x = calculateX(cubeX, cubeY, cubeZ, state->A, state->B, state->C);
    float y = calculateY(cubeX, cubeY, cubeZ, state->A, state->B, state->C);
    float z = calculateZ(cubeX, cubeY, cubeZ, state->A, state->B) + state->distanceFromCamera;

    float ooz = 1.0f / z; // One Over Z (Depth measurement)

    int xp = (int)(WIDTH / 2.0f + state->K1 * ooz * x);
    int yp = (int)(HEIGHT / 2.0f + state->K1 * ooz * y);

    int idx = xp + yp * WIDTH;
    if (idx >= 0 && idx < WIDTH * HEIGHT) {
        if (ooz > zBuffer[idx]) {
            zBuffer[idx] = ooz;
            pixelBuffer[idx] = getColor(ch);
        }
    }
}

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;

    EngineState state = {
        .A = 0.0f, .B = 0.0f, .C = 0.0f,
        .cubeWidth = 45.0f,
        .distanceFromCamera = 100.0f,
        .K1 = 40.0f,
        .incrementSpeed = 0.5f
    };

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Cube",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIDTH * WINDOW_SCALE, HEIGHT * WINDOW_SCALE, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (!window) {
        printf("Window error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!renderer) {
        printf("Renderer error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_RenderSetLogicalSize(renderer, WIDTH, HEIGHT);

    // Dynamic streaming texture acts as our virtual VRAM viewport framework
    SDL_Texture* screenTexture = SDL_CreateTexture(renderer, 
        SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

    if (!screenTexture) {
        printf("Texture error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Allocate runtime internal system frames
    float* zBuffer = (float*)malloc(WIDTH * HEIGHT * sizeof(float));
    uint32_t* pixelBuffer = (uint32_t*)malloc(WIDTH * HEIGHT * sizeof(uint32_t));

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) running = false;
        }

        // Fast clean operations using simple block-level standard clears
        memset(pixelBuffer, 0, WIDTH * HEIGHT * sizeof(uint32_t)); // Clear to pitch black
        memset(zBuffer, 0, WIDTH * HEIGHT * sizeof(float));

        float cW = state.cubeWidth;
        float inc = state.incrementSpeed;

        // Populate face surface clusters
        for (float cubeX = -cW; cubeX < cW; cubeX += inc) {
            for (float cubeY = -cW; cubeY < cW; cubeY += inc) {
                calculateForSurface(&state, cubeX, cubeY, -cW, '@', zBuffer, pixelBuffer);
                calculateForSurface(&state, cW, cubeY, cubeX, '$', zBuffer, pixelBuffer);
                calculateForSurface(&state, -cW, cubeY, -cubeX, '~', zBuffer, pixelBuffer);
                calculateForSurface(&state, -cubeX, cubeY, cW, '#', zBuffer, pixelBuffer);
                calculateForSurface(&state, cubeX, -cW, -cubeY, ';', zBuffer, pixelBuffer);
                calculateForSurface(&state, cubeX, cW, cubeY, '+', zBuffer, pixelBuffer);
            }
        }

        // Step structural rotation angles forward
        state.A += 0.03f;
        state.B += 0.02f;
        state.C += 0.01f;

        // Perform single transactional transfer block copy over to your visual hardware display
        SDL_UpdateTexture(screenTexture, NULL, pixelBuffer, WIDTH * sizeof(uint32_t));
        
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, screenTexture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    // Free resources cleanly
    free(zBuffer);
    free(pixelBuffer);
    SDL_DestroyTexture(screenTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
