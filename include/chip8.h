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

#ifndef _CHIP8_H_INCLUDED
#define _CHIP8_H_INCLUDED

/* Macros */
#define MATRIX_ROWS 32
#define MATRIX_COLS 64
#define MAX_GAME_SIZE (0x1000 - 0x200)
#define MEM_SIZE 4096
#define NB_REGS 16
#define STACK_SIZE 16
#define NB_KEYS 15

/* 4K of memory */
extern unsigned char memory[MEM_SIZE];
/* 15 8bit registers from V0 up to VE + 1 register for the carry
   flag */
extern unsigned char V[NB_REGS];
/* The index register */
extern unsigned short I;
/* The program counter */
extern unsigned short pc;
/* Matrix of pixels */
extern unsigned char matrix[MATRIX_ROWS * MATRIX_COLS];
/* Timers */
extern unsigned char delay_timer;
extern unsigned char sound_timer;
/* Stack */
extern unsigned short stack[STACK_SIZE];
/* Stack pointer */
extern unsigned short sp;
/* Current state of the keypad */
extern unsigned char key[NB_KEYS];
/* Flag to indicate if we should refresh the screen on this cycle */
extern unsigned char draw_flag;

extern void chip8_init (void);
extern void chip8_load_rom (char *);
extern void chip8_cycle (void);
extern void chip8_tick (void);
extern void chip8_update_graphics (void);
extern void clear_screen (void);

#endif /* _CHIP8_H_INCLUDED */
