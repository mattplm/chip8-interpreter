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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <chip8.h>

unsigned char memory[MEM_SIZE];
unsigned char V[NB_REGS];
unsigned short I;
unsigned short pc;
unsigned char matrix[MATRIX_ROWS * MATRIX_COLS];
unsigned char delay_timer;
unsigned char sound_timer;
unsigned short stack[STACK_SIZE];
unsigned short sp;
unsigned char key[NB_KEYS];
unsigned char draw_flag;

unsigned char chip8_fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void _clear_screen (void) {
    for (int i = 0; i < MATRIX_COLS * MATRIX_ROWS; i++) {
        matrix[i] = 0;
    }
}

void _draw (unsigned col, unsigned row, unsigned n) {
    unsigned byte_index;
    unsigned bit_index;

    V[0xF] = 0;
    for (byte_index = 0; byte_index < n; byte_index++) {
        unsigned char byte = memory[I + byte_index];

        for (bit_index = 0; bit_index < 8; bit_index++) {
            unsigned char bit = (byte >> bit_index) & 0x1;
            unsigned char *pixel = &matrix[
                ((row + byte_index) % MATRIX_ROWS) * MATRIX_COLS +
                (col + (7 - bit_index)) % MATRIX_COLS];

            if (bit == 1 && *pixel ==1) V[0xF] = 1;

            *pixel = *pixel ^ bit;
        }
    }
    draw_flag = 1;
}

void chip8_init () {
    pc = 0x200;
    I = 0;
    sp = 0;

    _clear_screen ();

    memset (stack, 0, 16 * sizeof (short));
    memset (V, 0, 16);
    memset (memory, 0, 4096);

    /* Load fonts */
    for (int i = 0; i < 80; i++)
        memory[i] = chip8_fontset[i];

    delay_timer = 0;
    sound_timer = 0;

    time_t t;
    srand((unsigned) time(&t));
}

void chip8_load_rom (char * filename) {
    FILE * fp;
    fp = fopen (filename, "r");
    fread (&memory[0x200], 1, MAX_GAME_SIZE, fp);
}


void chip8_cycle ()
{
    assert (pc >= 0x200);
    unsigned tmp;

    unsigned short opcode = memory[pc] << 8 | memory[pc + 1];

    pc += 2;

    unsigned x   = (opcode >> 8) & 0xF;
    unsigned y   = (opcode >> 4) & 0xF;
    unsigned n   = (opcode >> 0) & 0xF;
    unsigned kk  = (opcode >> 0) & 0xFF;
    unsigned nnn = (opcode >> 0) & 0x0FFF;

    unsigned instruction_error = 0;
    unsigned short foo = 0xf265;
    switch ((opcode & 0xF000) >> 12) {
    case 0x0:
        if (nnn == 0xE0)
            _clear_screen();
        else if (nnn = 0xEE)
            pc = stack[sp-- % 12];
        break;
    case 0x1:
        pc = nnn;
        break;
    case 0x2:
        stack[++sp % 12] = pc;
        pc = nnn;
        break;
    case 0x3:
        if (V[x] == kk)
            pc += 2;
        break;
    case 0x4:
        if (V[x] != kk)
            pc += 2;
        break;
    case 0x5:
        if (V[x] == V[y] && n == 0)
            pc += 2;
        break;
    case 0x6:
        V[x] = kk;
        break;
    case 0x7:
        V[x] += kk;
        break;
    case 0x8:
        if (n == 0)
            V[x] = V[y];
        else if (n == 1)
            V[x] |= V[y];
        else if (n == 2)
            V[x] &= V[y];
        else if (n == 3)
            V[x] ^= V[y];
        else if (n == 4) {
            tmp = V[x] + V[y];
            V[0xF] = tmp >> 8;
            V[x] = tmp;
        } else if (n == 5) {
            tmp = V[x] - V[y];
            V[0xF] = !(tmp >> 8);
            V[x] = tmp;
        } else if (n == 6) {
            V[0xF] = V[y] & 1;
            V[x] = V[y] >> 1;
        } else if (n == 7) {
            tmp = V[y] - V[x];
            V[0xF] = !(tmp >> 8);
            V[x] = tmp;
        } else if (n == 0xE) {
            V[0xF] = V[y] >> 7;
            V[x] = V[y] << 1;
        }
        break;
    case 0x9:
        if (n == 0 && V[x] != V[y])
            pc += 2;
        break;
    case 0xA:
        I = nnn;
        break;
    case 0xB:
        pc = nnn + V[0];
        break;
    case 0xC:
        V[x] = rand() & kk;
        break;
    case 0xD:
        _draw(V[x], V[y], n);
        break;
    case 0xE:
        if (kk == 0x9E)
            if (key[V[x]])
                pc += 2;
            else if (kk == 0xA1)
                if (!key[V[x]])
                    pc += 2;
        break;
    case 0xF:
        switch (kk) {
        case 0x07:
            V[x] = delay_timer;
            break;
        case 0x0A:
        {
            unsigned key_pressed = 0;
            while (!key_pressed) {
                for (tmp = 0; tmp < NB_KEYS; tmp++) {
                    if (key[tmp]) {
                        key_pressed = 1;
                        V[x] = tmp;
                    }
                }
            }
            break;
        }
        case 0x15:
            delay_timer = V[x];
            break;
        case 0x18:
            sound_timer = V[x];
            break;
        case 0x1E:
            I = (I & 0xFFF) + V[x];
            V[0xF] = I >> 12;
            break;
        case 0x29:
            I = &chip8_fontset[(V[x] & 0xF) * 5] - memory;
            break;
        case 0x33:
            memory[(I + 0) & 0xFFF] = (V[x] / 100) % 10;
            memory[(I + 1) & 0xFFF] = (V[x] / 10)  % 10;
            memory[(I + 2) & 0xFFF] = (V[x] / 1)   % 10;
            break;
        case 0x55:
            for (tmp = 0; tmp <= x; tmp++)
                memory[I++ & 0xFFF] = V[tmp];
            break;
        case 0x65:
            for (tmp = 0; tmp <= x; tmp++)
                V[tmp] = memory[I++ & 0xFFF];
            break;
        }
    }

    if (instruction_error) {
        fprintf(stderr, "Insctruction %x does not exist\n", opcode);
    }
}

void chip8_tick () {
    if (delay_timer > 0) {
        --delay_timer;
    }
    if (sound_timer > 0) {
        --sound_timer;
        if (sound_timer == 0)
            printf("BEEP!\n");
    }
}
