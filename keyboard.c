#include "keyboard.h"

// Portions of this file are stolen from:

/*
 *  linux/drivers/acorn/char/keyb_ps2.c
 *
 *  Copyright (C) 2000 Russell King
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *  Keyboard driver for RiscPC ARM Linux.
 *  [...]
 */

#define KBD_REPORT_ERR
#define KBD_REPORT_UNKN
 
#define KBD_ESCAPEE0    0xe0            /* in */
#define KBD_ESCAPEE1    0xe1            /* in */
 
#define ESCE0(x)        (0xe000|(x))
#define ESCE1(x)        (0xe100|(x))
 
#define KBD_BAT         0xaa            /* in */
#define KBD_SETLEDS     0xed            /* out */
#define KBD_ECHO        0xee            /* in/out */
#define KBD_BREAK       0xf0            /* in */
#define KBD_TYPRATEDLY  0xf3            /* out */
#define KBD_SCANENABLE  0xf4            /* out */
#define KBD_DEFDISABLE  0xf5            /* out */
#define KBD_DEFAULT     0xf6            /* out */
#define KBD_ACK         0xfa            /* in */
#define KBD_DIAGFAIL    0xfd            /* in */
#define KBD_RESEND      0xfe            /* in/out */
#define KBD_RESET       0xff            /* out */
 
#define CODE_BREAK      1
#define CODE_ESCAPEE0   2
#define CODE_ESCAPEE1   4
#define CODE_ESCAPE12   8

#define K_NONE		0x7f
#define K_ESC		0x00
#define K_F1		0x01
#define K_F2		0x02
#define K_F3		0x03
#define K_F4		0x04
#define K_F5		0x05
#define K_F6		0x06
#define K_F7		0x07
#define K_F8		0x08
#define K_F9		0x09
#define K_F10		0x0a
#define K_F11		0x0b
#define K_F12		0x0c
#define K_PRNT		0x0d
#define K_SCRL		0x0e
#define K_BRK		0x0f
#define K_AGR		0x10
#define K_1		0x11
#define K_2		0x12
#define K_3		0x13
#define K_4		0x14
#define K_5		0x15
#define K_6		0x16
#define K_7		0x17
#define K_8		0x18
#define K_9		0x19
#define K_0		0x1a
#define K_MINS		0x1b
#define K_EQLS		0x1c
#define K_BKSP		0x1e
#define K_INS		0x1f
#define K_HOME		0x20
#define K_PGUP		0x21
#define K_NUML		0x22
#define KP_SLH		0x23
#define KP_STR		0x24
#define KP_MNS		0x3a
#define K_TAB		0x26
#define K_Q		0x27
#define K_W		0x28
#define K_E		0x29
#define K_R		0x2a
#define K_T		0x2b
#define K_Y		0x2c
#define K_U		0x2d
#define K_I		0x2e
#define K_O		0x2f
#define K_P		0x30
#define K_LSBK		0x31
#define K_RSBK		0x32
#define K_ENTR		0x47
#define K_DEL		0x34
#define K_END		0x35
#define K_PGDN		0x36
#define KP_7		0x37
#define KP_8		0x38
#define KP_9		0x39
#define KP_PLS		0x4b
#define K_CAPS		0x5d
#define K_A		0x3c
#define K_S		0x3d
#define K_D		0x3e
#define K_F		0x3f
#define K_G		0x40
#define K_H		0x41
#define K_J		0x42
#define K_K		0x43
#define K_L		0x44
#define K_SEMI		0x45
#define K_SQOT		0x46
#define K_HASH		0x1d
#define KP_4		0x48
#define KP_5		0x49
#define KP_6		0x4a
#define K_LSFT		0x4c
#define K_BSLH		0x33
#define K_Z		0x4e
#define K_X		0x4f
#define K_C		0x50
#define K_V		0x51
#define K_B		0x52
#define K_N		0x53
#define K_M		0x54
#define K_COMA		0x55
#define K_DOT		0x56
#define K_FSLH		0x57
#define K_RSFT		0x58
#define K_UP		0x59
#define KP_1		0x5a
#define KP_2		0x5b
#define KP_3		0x5c
#define KP_ENT		0x67
#define K_LCTL		0x3b
#define K_LALT		0x5e
#define K_SPCE		0x5f
#define K_RALT		0x60
#define K_RCTL		0x61
#define K_LEFT		0x62
#define K_DOWN		0x63
#define K_RGHT		0x64
#define KP_0		0x65
#define KP_DOT		0x66

static unsigned char keycode_translate[256] =
{
/* 00 */  K_NONE, K_F9  , K_NONE, K_F5  , K_F3  , K_F1  , K_F2  , K_F12 ,
/* 08 */  K_NONE, K_F10 , K_F8  , K_F6  , K_F4  , K_TAB , K_AGR , K_NONE,
/* 10 */  K_NONE, K_LALT, K_LSFT, K_NONE, K_LCTL, K_Q   , K_1   , K_NONE,
/* 18 */  K_NONE, K_NONE, K_Z   , K_S   , K_A   , K_W   , K_2   , K_NONE,
/* 20 */  K_NONE, K_C   , K_X   , K_D   , K_E   , K_4   , K_3   , K_NONE,
/* 28 */  K_NONE, K_SPCE, K_V   , K_F   , K_T   , K_R   , K_5   , K_NONE,
/* 30 */  K_NONE, K_N   , K_B   , K_H   , K_G   , K_Y   , K_6   , K_NONE,
/* 38 */  K_NONE, K_NONE, K_M   , K_J   , K_U   , K_7   , K_8   , K_NONE,
/* 40 */  K_NONE, K_COMA, K_K   , K_I   , K_O   , K_0   , K_9   , K_NONE,
/* 48 */  K_NONE, K_DOT , K_FSLH, K_L   , K_SEMI, K_P   , K_MINS, K_NONE,
/* 50 */  K_NONE, K_NONE, K_SQOT, K_NONE, K_LSBK, K_EQLS, K_NONE, K_NONE,
/* 58 */  K_CAPS, K_RSFT, K_ENTR, K_RSBK, K_NONE, K_HASH, K_NONE, K_NONE,
/* 60 */  K_NONE, K_BSLH, K_NONE, K_NONE, K_NONE, K_NONE, K_BKSP, K_NONE,
/* 68 */  K_NONE, KP_1  , K_NONE, KP_4  , KP_7  , K_NONE, K_NONE, K_NONE,
/* 70 */  KP_0  , KP_DOT, KP_2  , KP_5  , KP_6  , KP_8  , K_ESC , K_NUML,
/* 78 */  K_F11 , KP_PLS, KP_3  , KP_MNS, KP_STR, KP_9  , K_SCRL, K_NONE,
	  K_NONE, K_NONE, K_NONE, K_F7  , K_NONE, K_NONE, K_NONE, K_NONE,
	  K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE,
	  K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE,
	  K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE,
	  K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE,
	  K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE,
	  K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE,
	  K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE,
	  K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE,
	  K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE,
	  K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE,
	  K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE,
	  K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE,
	  K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE
};

