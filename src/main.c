/*
 * Copyright (C) 2021
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <stdbool.h>

#include <chip8.h>
#include <gfx.h>

#define PIXEL_SIZE    10
#define SCREEN_WIDTH  MATRIX_COLS * PIXEL_SIZE
#define SCREEN_HEIGHT MATRIX_ROWS * PIXEL_SIZE

int main_loop(void);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usages: %s </path/to/rom>\n", argv[0]);
        exit(1);
    }

    init_sdl(SCREEN_WIDTH, SCREEN_HEIGHT);

    chip8_init();
    chip8_load_rom(argv[1]);

    int cont;
    do {
        cont = main_loop();
    } while (cont);

    deinit_sdl();
    return 0;
}

void redraw_renderer() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    for (size_t y = 0; y < MATRIX_ROWS; y++) {
        for (size_t x = 0; x < MATRIX_COLS; x++) {
            if (matrix[y * MATRIX_COLS + x]) {
                SDL_Rect pixel = {
                    PIXEL_SIZE * x, PIXEL_SIZE * y,
                    PIXEL_SIZE, PIXEL_SIZE
                };
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderFillRect(renderer, &pixel);
            }
        }
    }
}

int keymap(unsigned key) {
    switch (key) {
    case SDLK_3:
        return 0x1;
        break;
    case SDLK_4:
        return 0x2;
        break;
    case SDLK_5:
        return 0x3;
        break;
    case SDLK_e:
        return 0x4;
        break;
    case SDLK_r:
        return 0x5;
        break;
    case SDLK_t:
        return 0x6;
        break;
    case SDLK_d:
        return 0x7;
        break;
    case SDLK_f:
        return 0x8;
        break;
    case SDLK_g:
        return 0x9;
        break;
    case SDLK_c:
        return 0xA;
        break;
    case SDLK_v:
        return 0xB;
        break;
    case SDLK_b:
        return 0xC;
        break;
    default:
        return -1;
        break;
    }
}

int main_loop() {
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type) {
    case SDL_QUIT:
        return 0;
    case SDL_KEYDOWN:
    {
        int key = keymap(event.key.keysym.sym);
        if (key != -1)
            printf("%x\n", key);
        break;
    }
    }
    if (draw_flag) {
        redraw_renderer();
    }
    SDL_RenderPresent(renderer);
    SDL_RenderPresent(renderer);
    chip8_cycle();
    chip8_tick();
    return 1;
}
