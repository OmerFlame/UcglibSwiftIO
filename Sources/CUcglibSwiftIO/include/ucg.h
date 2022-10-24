/*

  ucg.h

  ucglib = universal color graphics library
  ucglib = micro controller graphics library
  
  Universal uC Color Graphics Library
  
  Copyright (c) 2013, olikraus@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list 
    of conditions and the following disclaimer.
    
  * Redistributions in binary form must reproduce the above copyright notice, this 
    list of conditions and the following disclaimer in the documentation and/or other 
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
  

  SW layers
  
  High Level Procedures
    [ hline, init message interface ]
  display callback procedure
    [Calls to]
  device dev cb 		
    [calls to COM API]
  com callback


  font data:
  offset	bytes	description
  0		1		glyph_cnt		number of glyphs
  1		1		bbx_mode	0: proportional, 1: common height, 2: monospace, 3: multiple of 8
  2		1		bits_per_0	glyph rle parameter
  3		1		bits_per_1	glyph rle parameter

  4		1		bits_per_char_width		glyph rle parameter
  5		1		bits_per_char_height	glyph rle parameter
  6		1		bits_per_char_x		glyph rle parameter
  7		1		bits_per_char_y		glyph rle parameter
  8		1		bits_per_delta_x		glyph rle parameter

  9		1		max_char_width
  10		1		max_char_height
  11		1		x offset
  12		1		y offset (descent)
  
  13		1		ascent (capital A)
  14		1		descent (lower g)
  15		1		ascent '('
  16		1		descent ')'
  
  17		1		start pos 'A' high byte
  18		1		start pos 'A' low byte

  19		1		start pos 'a' high byte
  20		1		start pos 'a' low byte


*/

#ifndef _UCG_H
#define _UCG_H

#include <stdint.h>
#include <stddef.h>
#include <swift_gpio.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef USE_PIN_LIST
#define USE_PIN_LIST
#endif

#ifdef __GNUC__
#  define UCG_NOINLINE __attribute__((noinline))
#  define UCG_SECTION(name) __attribute__ ((section (name)))
#  if defined(__MSPGCC__)
/* mspgcc does not have .progmem sections. Use -fdata-sections. */
#    define UCG_FONT_SECTION(name)
#  elif defined(__AVR__)
#    define UCG_FONT_SECTION(name) UCG_SECTION(".progmem." name)
#  else
#    define UCG_FONT_SECTION(name)
#  endif
#else
#  define UCG_NOINLINE
#  define UCG_SECTION(name)
#  define UCG_FONT_SECTION(name)
#endif

#if defined(__AVR__)
#include <avr/pgmspace.h>
/* UCG_PROGMEM is used by the XBM example */
#define UCG_PROGMEM UCG_SECTION(".progmem.data")
typedef uint8_t PROGMEM ucg_pgm_uint8_t;
typedef uint8_t ucg_fntpgm_uint8_t;
#define ucg_pgm_read(adr) pgm_read_byte_near(adr)
#define UCG_PSTR(s) ((ucg_pgm_uint8_t *)PSTR(s))
#else
#define UCG_PROGMEM
#define PROGMEM
typedef uint8_t ucg_pgm_uint8_t;
typedef uint8_t ucg_fntpgm_uint8_t;
#define ucg_pgm_read(adr) (*(const ucg_pgm_uint8_t *)(adr)) 
#define UCG_PSTR(s) ((ucg_pgm_uint8_t *)(s))
#endif

/*================================================*/
/* several type and forward definitions */

typedef int16_t ucg_int_t;
typedef struct _ucg_t ucg_t;
typedef struct _ucg_xy_t ucg_xy_t;
typedef struct _ucg_wh_t ucg_wh_t;
typedef struct _ucg_box_t ucg_box_t;
typedef struct _ucg_color_t ucg_color_t;
typedef struct _ucg_ccs_t ucg_ccs_t;
typedef struct _ucg_pixel_t ucg_pixel_t;
typedef struct _ucg_arg_t ucg_arg_t;
typedef struct _ucg_com_info_t ucg_com_info_t;

typedef ucg_int_t (*ucg_dev_fnptr)(ucg_t *ucg, ucg_int_t msg, void *data); 
typedef int16_t (*ucg_com_fnptr)(ucg_t *ucg, int16_t msg, uint16_t arg, uint8_t *data); 
typedef ucg_int_t (*ucg_font_calc_vref_fnptr)(ucg_t *ucg);
//typedef ucg_int_t (*ucg_font_mode_fnptr)(ucg_t *ucg, ucg_int_t x, ucg_int_t y, uint8_t dir, uint8_t encoding);


/*================================================*/
/* list of supported display modules */

ucg_int_t ucg_dev_ssd1351_18x128x128_ilsoft(ucg_t *ucg, ucg_int_t msg, void *data);
ucg_int_t ucg_dev_ssd1351_18x128x128_ft(ucg_t *ucg, ucg_int_t msg, void *data);
ucg_int_t ucg_dev_ili9325_18x240x320_itdb02(ucg_t *ucg, ucg_int_t msg, void *data);
ucg_int_t ucg_dev_ili9325_spi_18x240x320(ucg_t *ucg, ucg_int_t msg, void *data); /*  1 May 2014: Currently, this is not working */
ucg_int_t ucg_dev_ili9341_18x240x320(ucg_t *ucg, ucg_int_t msg, void *data);
ucg_int_t ucg_dev_hx8352c_18x240x400(ucg_t *ucg, ucg_int_t msg, void *data);
ucg_int_t ucg_dev_ili9486_18x320x480(ucg_t *ucg, ucg_int_t msg, void *data);
ucg_int_t ucg_dev_ili9163_18x128x128(ucg_t *ucg, ucg_int_t msg, void *data);
ucg_int_t ucg_dev_st7735_18x128x160(ucg_t *ucg, ucg_int_t msg, void *data);
ucg_int_t ucg_dev_pcf8833_16x132x132(ucg_t *ucg, ucg_int_t msg, void *data);
ucg_int_t ucg_dev_ld50t6160_18x160x128_samsung(ucg_t *ucg, ucg_int_t msg, void *data);
ucg_int_t ucg_dev_ssd1331_18x96x64_univision(ucg_t *ucg, ucg_int_t msg, void *data);
ucg_int_t ucg_dev_seps225_16x128x128_univision(ucg_t *ucg, ucg_int_t msg, void *data);



/*================================================*/
/* 
  list of extensions for the controllers 
  
  each module can have the "none" extension (ucg_ext_none) or the specific
  extensions, that matches the controller name and color depth.
  
  example: for the module ucg_dev_ssd1351_18x128x128_ilsoft
  valid extensions are:
    ucg_ext_none
    ucg_ext_ssd1351_18
*/

ucg_int_t ucg_ext_none(ucg_t *ucg, ucg_int_t msg, void *data);

ucg_int_t ucg_ext_ssd1351_18(ucg_t *ucg, ucg_int_t msg, void *data);
ucg_int_t ucg_ext_ili9325_18(ucg_t *ucg, ucg_int_t msg, void *data);
ucg_int_t ucg_ext_ili9325_spi_18(ucg_t *ucg, ucg_int_t msg, void *data);
ucg_int_t ucg_ext_ili9341_18(ucg_t *ucg, ucg_int_t msg, void *data);
ucg_int_t ucg_ext_hx8352c_18(ucg_t *ucg, ucg_int_t msg, void *data);
ucg_int_t ucg_ext_ili9486_18(ucg_t *ucg, ucg_int_t msg, void *data);
ucg_int_t ucg_ext_ili9163_18(ucg_t *ucg, ucg_int_t msg, void *data);
ucg_int_t ucg_ext_st7735_18(ucg_t *ucg, ucg_int_t msg, void *data);
ucg_int_t ucg_ext_pcf8833_16(ucg_t *ucg, ucg_int_t msg, void *data);
ucg_int_t ucg_ext_ld50t6160_18(ucg_t *ucg, ucg_int_t msg, void *data);
ucg_int_t ucg_ext_ssd1331_18(ucg_t *ucg, ucg_int_t msg, void *data);
ucg_int_t ucg_ext_seps225_16(ucg_t *ucg, ucg_int_t msg, void *data);


/*================================================*/
/* list of supported display controllers */

ucg_int_t ucg_dev_ic_ssd1351_18(ucg_t *ucg, ucg_int_t msg, void *data);
ucg_int_t ucg_dev_ic_ili9325_18(ucg_t *ucg, ucg_int_t msg, void *data);
ucg_int_t ucg_dev_ic_ili9325_spi_18(ucg_t *ucg, ucg_int_t msg, void *data);
ucg_int_t ucg_dev_ic_ili9341_18(ucg_t *ucg, ucg_int_t msg, void *data);
ucg_int_t ucg_dev_ic_hx8352c_18(ucg_t *ucg, ucg_int_t msg, void *data);
ucg_int_t ucg_dev_ic_ili9486_18(ucg_t *ucg, ucg_int_t msg, void *data);
ucg_int_t ucg_dev_ic_ili9163_18(ucg_t *ucg, ucg_int_t msg, void *data);
ucg_int_t ucg_dev_ic_st7735_18(ucg_t *ucg, ucg_int_t msg, void *data);
ucg_int_t ucg_dev_ic_pcf8833_16(ucg_t *ucg, ucg_int_t msg, void *data);
ucg_int_t ucg_dev_ic_ld50t6160_18(ucg_t *ucg, ucg_int_t msg, void *data);
ucg_int_t ucg_dev_ic_ssd1331_18(ucg_t *ucg, ucg_int_t msg, void *data);   /* actually this display only has 65k colors */
ucg_int_t ucg_dev_ic_seps225_16(ucg_t *ucg, ucg_int_t msg, void *data);   /* this display could do 262k colors, but only 65k are supported via SPI */


/*================================================*/
/* struct declarations */

struct _ucg_xy_t
{
  ucg_int_t x;
  ucg_int_t y;
};

struct _ucg_wh_t
{
  ucg_int_t w;  
  ucg_int_t h;
};

struct _ucg_box_t
{
  ucg_xy_t ul;
  ucg_wh_t size;
};

struct _ucg_color_t
{
  uint8_t color[3];		/* 0: Red, 1: Green, 2: Blue */
};

struct _ucg_ccs_t
{
  uint8_t current;	/* contains the current color component */
  uint8_t start;
  ucg_int_t dir;		/* 1 if start < end or -1 if start > end */
  ucg_int_t num;
  ucg_int_t quot;
  
  ucg_int_t den;
  ucg_int_t rem;  
  ucg_int_t frac;
};

struct _ucg_pixel_t
{
  ucg_xy_t pos;
  ucg_color_t rgb;  
};

struct _ucg_arg_t
{
  ucg_pixel_t pixel;
  ucg_int_t len;
  ucg_int_t dir;
  ucg_int_t offset;			/* calculated offset from the inital point to the start of the clip window (ucg_clip_l90fx) */
  ucg_int_t scale;			/* upscale factor, used by UCG_MSG_DRAW_L90BF */
  const unsigned char *bitmap;
  ucg_int_t pixel_skip;		/* within the "bitmap" skip the specified number of pixel with the bit. pixel_skip is always <= 7 */
  ucg_color_t rgb[4];			/* start and end color for L90SE , two more colors for the gradient box */
  ucg_ccs_t ccs_line[3];		/* color component sliders used by L90SE */
};

#define UCG_FONT_HEIGHT_MODE_TEXT 0
#define UCG_FONT_HEIGHT_MODE_XTEXT 1
#define UCG_FONT_HEIGHT_MODE_ALL 2

struct _ucg_com_info_t
{
  uint16_t serial_clk_speed;	/* nano seconds cycle time */
  uint16_t parallel_clk_speed;	/* nano seconds cycle time */
};


struct _ucg_font_info_t
{
  /* offset 0 */
  uint8_t glyph_cnt;
  uint8_t bbx_mode;
  uint8_t bits_per_0;
  uint8_t bits_per_1;
  
  /* offset 4 */
  uint8_t bits_per_char_width;
  uint8_t bits_per_char_height;		
  uint8_t bits_per_char_x;
  uint8_t bits_per_char_y;
  uint8_t bits_per_delta_x;
  
  /* offset 9 */
  int8_t max_char_width;
  int8_t max_char_height; /* overall height, NOT ascent. Instead ascent = max_char_height + y_offset */
  int8_t x_offset;
  int8_t y_offset;
  
  /* offset 13 */
  int8_t  ascent_A;
  int8_t  descent_g;
  int8_t  ascent_para;
  int8_t  descent_para;
  
  /* offset 17 */
  uint16_t start_pos_upper_A;
  uint16_t start_pos_lower_a;  
};
typedef struct _ucg_font_info_t ucg_font_info_t;

struct _ucg_font_decode_t
{
  const uint8_t *decode_ptr;			/* pointer to the compressed data */
  
  ucg_int_t target_x;
  ucg_int_t target_y;
  
  int8_t x;						/* local coordinates, (0,0) is upper left */
  int8_t y;
  int8_t glyph_width;	
  int8_t glyph_height;

  uint8_t decode_bit_pos;			/* bitpos inside a byte of the compressed data */
  uint8_t is_transparent;
  uint8_t dir;				/* direction */
};
typedef struct _ucg_font_decode_t ucg_font_decode_t;



#ifdef USE_PIN_LIST
#define UCG_PIN_RST 0
#define UCG_PIN_CD 1
#define UCG_PIN_CS 2
#define UCG_PIN_SCL 3
#define UCG_PIN_WR 3
#define UCG_PIN_SDA 4

#define UCG_PIN_D0 5
#define UCG_PIN_D1 6
#define UCG_PIN_D2 7
#define UCG_PIN_D3 8
#define UCG_PIN_D4 9
#define UCG_PIN_D5 10
#define UCG_PIN_D6 11
#define UCG_PIN_D7 12

#define UCG_PIN_COUNT 13

#define UCG_PIN_VAL_NONE 255
#endif

struct _ucg_t
{
  unsigned is_power_up:1;
  /* the dimension of the display */
  ucg_wh_t dimension;
  /* display callback procedure to handle display specific code */
  //ucg_dev_fnptr display_cb;
  /* controller and device specific code, high level procedure will call this */
  ucg_dev_fnptr device_cb;
  /* name of the extension cb. will be called by device_cb if required */
  ucg_dev_fnptr ext_cb;
  /* if rotation is applied, than this cb is called after rotation */
  ucg_dev_fnptr rotate_chain_device_cb;
  ucg_wh_t rotate_dimension;

  /* if rotation is applied, than this cb is called by the scale device */
  ucg_dev_fnptr scale_chain_device_cb;
  
  /* communication interface */
  ucg_com_fnptr com_cb;
  
  /* offset, that is additionally added to UCG_VARX/UCG_VARY */
  /* seems to be required for the Nokia display */
  // ucg_xy_t display_offset;
  
  /* data which is passed to the cb procedures */
  /* can be modified by the cb procedures (rotation, clipping, etc) */
  ucg_arg_t arg;
  /* current window to which all drawing is clipped */
  /* should be modified via UCG_MSG_SET_CLIP_BOX by a device callback. */
  /* by default this is done by ucg_dev_default_cb */
  ucg_box_t clip_box;
  

  /* information about the current font */
  const unsigned char *font;             /* current font for all text procedures */
  ucg_font_calc_vref_fnptr font_calc_vref;
  //ucg_font_mode_fnptr font_mode;		/* OBSOLETE?? UCG_FONT_MODE_TRANSPARENT, UCG_FONT_MODE_SOLID, UCG_FONT_MODE_NONE */

  ucg_font_decode_t font_decode;		/* new font decode structure */
  ucg_font_info_t font_info;			/* new font info structure */

  int8_t glyph_dx;			/* OBSOLETE */
  int8_t glyph_x;			/* OBSOLETE */
  int8_t glyph_y;			/* OBSOLETE */
  uint8_t glyph_width;		/* OBSOLETE */
  uint8_t glyph_height;		/* OBSOLETE */
  
  uint8_t font_height_mode;
  int8_t font_ref_ascent;
  int8_t font_ref_descent;

  /* only for Arduino/C++ Interface */
#ifdef USE_PIN_LIST
  //uint8_t pin_list[UCG_PIN_COUNT];

#ifdef __AVR__
  volatile uint8_t *data_port[UCG_PIN_COUNT];
  uint8_t data_mask[UCG_PIN_COUNT];
#endif

#endif

  /* 
    Small amount of RAM for the com interface (com_cb).
    Might be unused on unix systems, where the com sub system is 
    not required, but should be usefull for all uC projects.
  */
  uint8_t com_initial_change_sent;	/* Bit 0: CD/A0 Line Status, Bit 1: CS Line Status, Bit 2: Reset Line Status */
  uint8_t com_status;		/* Bit 0: CD/A0 Line Status, Bit 1: CS Line Status, Bit 2: Reset Line Status,  Bit 3: 1 for power up */
  uint8_t com_cfg_cd;		/* Bit 0: Argument Level, Bit 1: Command Level */
  
  
};

ucg_int_t ucg_GetWidth(ucg_t *ucg) {
    return ucg->dimension.w;
}

ucg_int_t ucg_GetHeight(ucg_t *ucg) {
    return ucg->dimension.h;
}

//#define ucg_GetWidth(ucg) ((ucg)->dimension.w)
//#define ucg_GetHeight(ucg) ((ucg)->dimension.h)

#define UCG_MSG_DEV_POWER_UP	10
#define UCG_MSG_DEV_POWER_DOWN 11
#define UCG_MSG_SET_CLIP_BOX 12
#define UCG_MSG_GET_DIMENSION 15

/* draw pixel with color from idx 0 */
#define UCG_MSG_DRAW_PIXEL 20
#define UCG_MSG_DRAW_L90FX 21
/* draw  bit pattern, transparent and draw color (idx 0) color */
//#define UCG_MSG_DRAW_L90TC 22		/* can be commented, used by ucg_DrawTransparentBitmapLine */
#define UCG_MSG_DRAW_L90SE 23		/* this part of the extension */
//#define UCG_MSG_DRAW_L90RL 24	/* not yet implemented */
/* draw  bit pattern with foreground (idx 1) and background (idx 0) color */
//#define UCG_MSG_DRAW_L90BF 25	 /* can be commented, used by ucg_DrawBitmapLine */


#define UCG_COM_STATUS_MASK_POWER 8
#define UCG_COM_STATUS_MASK_RESET 4
#define UCG_COM_STATUS_MASK_CS 2
#define UCG_COM_STATUS_MASK_CD 1

/*
  arg:	0
  data:	ucg_com_info_t *
  return:	0 for error
  note: 
    - power up is the first command, which is sent
*/
#define UCG_COM_MSG_POWER_UP 10

/*
  note: power down my be followed only by power up command
*/
#define UCG_COM_MSG_POWER_DOWN 11

/*
  arg:	delay in microseconds  (0..4095) 
*/
#define UCG_COM_MSG_DELAY 12

/*
  ucg->com_status	contains previous status of reset line
  arg:			new logic level for reset line
*/
#define UCG_COM_MSG_CHANGE_RESET_LINE 13
/*
  ucg->com_status	contains previous status of cs line
  arg:	new logic level for cs line
*/
#define UCG_COM_MSG_CHANGE_CS_LINE 14

/*
  ucg->com_status	contains previous status of cd line
  arg:	new logic level for cd line
*/
#define UCG_COM_MSG_CHANGE_CD_LINE 15

/*
  ucg->com_status	current status of Reset, CS and CD line (ucg->com_status)
  arg:			byte for display
*/
#define UCG_COM_MSG_SEND_BYTE 16

/*
  ucg->com_status	current status of Reset, CS and CD line (ucg->com_status)
  arg:			how often to repeat the 2/3 byte sequence 	
  data:			pointer to two or three bytes
*/
#define UCG_COM_MSG_REPEAT_1_BYTE 17
#define UCG_COM_MSG_REPEAT_2_BYTES 18
#define UCG_COM_MSG_REPEAT_3_BYTES 19

/*
  ucg->com_status	current status of Reset, CS and CD line (ucg->com_status)
  arg:			length of string 	
  data:			string
*/
#define UCG_COM_MSG_SEND_STR 20

/*
  ucg->com_status	current status of Reset, CS and CD line (ucg->com_status)
  arg:			number of cd_info and data pairs (half value of total byte cnt) 	
  data:			uint8_t with CD and data information
	cd_info data cd_info data cd_info data cd_info data ... cd_info data cd_info data
	cd_info is the level, which is directly applied to the CD line. This means,
	information applied to UCG_CFG_CD is not relevant.
*/
#define UCG_COM_MSG_SEND_CD_DATA_SEQUENCE 21



/*================================================*/
/* interrupt safe code */
#define UCG_INTERRUPT_SAFE
#if defined(UCG_INTERRUPT_SAFE)
#  if defined(__AVR__)
extern uint8_t global_SREG_backup;	/* ucg_init.c */
#    define UCG_ATOMIC_START()		do { global_SREG_backup = SREG; cli(); } while(0)
#    define UCG_ATOMIC_END()			SREG = global_SREG_backup
#    define UCG_ATOMIC_OR(ptr, val) 	do { uint8_t tmpSREG = SREG; cli(); (*(ptr) |= (val)); SREG = tmpSREG; } while(0)
#    define UCG_ATOMIC_AND(ptr, val) 	do { uint8_t tmpSREG = SREG; cli(); (*(ptr) &= (val)); SREG = tmpSREG; } while(0)
#  else
#    define UCG_ATOMIC_OR(ptr, val) (*(ptr) |= (val))
#    define UCG_ATOMIC_AND(ptr, val) (*(ptr) &= (val))
#    define UCG_ATOMIC_START()
#    define UCG_ATOMIC_END()
#  endif /* __AVR__ */
#else
#  define UCG_ATOMIC_OR(ptr, val) (*(ptr) |= (val))
#  define UCG_ATOMIC_AND(ptr, val) (*(ptr) &= (val))
#  define UCG_ATOMIC_START()
#  define UCG_ATOMIC_END()
#endif /* UCG_INTERRUPT_SAFE */

/*================================================*/
/* ucg_dev_msg_api.c */
void ucg_PowerDown(ucg_t *ucg);
ucg_int_t ucg_PowerUp(ucg_t *ucg);
void ucg_SetClipBox(ucg_t *ucg, ucg_box_t *clip_box);
void ucg_SetClipRange(ucg_t *ucg, ucg_int_t x, ucg_int_t y, ucg_int_t w, ucg_int_t h);
void ucg_SetMaxClipRange(ucg_t *ucg);
void ucg_GetDimension(ucg_t *ucg);
void ucg_DrawPixelWithArg(ucg_t *ucg);
void ucg_DrawL90FXWithArg(ucg_t *ucg);
void ucg_DrawL90TCWithArg(ucg_t *ucg);
void ucg_DrawL90BFWithArg(ucg_t *ucg);
void ucg_DrawL90SEWithArg(ucg_t *ucg);
/* void ucg_DrawL90RLWithArg(ucg_t *ucg); */

/*================================================*/
/* ucg_init.c */
ucg_int_t ucg_Init(ucg_t *ucg, ucg_dev_fnptr device_cb, ucg_dev_fnptr ext_cb, ucg_com_fnptr com_cb);


/*================================================*/
/* ucg_dev_sdl.c */
ucg_int_t ucg_sdl_dev_cb(ucg_t *ucg, ucg_int_t msg, void *data);

/*================================================*/
/* ucg_pixel.c */
void ucg_SetColor(ucg_t *ucg, uint8_t idx, uint8_t r, uint8_t g, uint8_t b);
void ucg_DrawPixel(ucg_t *ucg, ucg_int_t x, ucg_int_t y);

/*================================================*/
/* ucg_line.c */
void ucg_Draw90Line(ucg_t *ucg, ucg_int_t x, ucg_int_t y, ucg_int_t len, ucg_int_t dir, ucg_int_t col_idx);
void ucg_DrawHLine(ucg_t *ucg, ucg_int_t x, ucg_int_t y, ucg_int_t len);
void ucg_DrawVLine(ucg_t *ucg, ucg_int_t x, ucg_int_t y, ucg_int_t len);
void ucg_DrawHRLine(ucg_t *ucg, ucg_int_t x, ucg_int_t y, ucg_int_t len);
void ucg_DrawLine(ucg_t *ucg, ucg_int_t x1, ucg_int_t y1, ucg_int_t x2, ucg_int_t y2);
/* the following procedure is only available with the extended callback */
void ucg_DrawGradientLine(ucg_t *ucg, ucg_int_t x, ucg_int_t y, ucg_int_t len, ucg_int_t dir);


/*================================================*/
/* ucg_box.c */
void ucg_DrawBox(ucg_t *ucg, ucg_int_t x, ucg_int_t y, ucg_int_t w, ucg_int_t h);
void ucg_ClearScreen(ucg_t *ucg);
void ucg_DrawRBox(ucg_t *ucg, ucg_int_t x, ucg_int_t y, ucg_int_t w, ucg_int_t h, ucg_int_t r);
void ucg_DrawGradientBox(ucg_t *ucg, ucg_int_t x, ucg_int_t y, ucg_int_t w, ucg_int_t h);
void ucg_DrawFrame(ucg_t *ucg, ucg_int_t x, ucg_int_t y, ucg_int_t w, ucg_int_t h);
void ucg_DrawRFrame(ucg_t *ucg, ucg_int_t x, ucg_int_t y, ucg_int_t w, ucg_int_t h, ucg_int_t r);


/*================================================*/
/* ucg_circle.c */
#define UCG_DRAW_UPPER_RIGHT 0x01
#define UCG_DRAW_UPPER_LEFT  0x02
#define UCG_DRAW_LOWER_LEFT 0x04
#define UCG_DRAW_LOWER_RIGHT  0x08
#define UCG_DRAW_ALL (UCG_DRAW_UPPER_RIGHT|UCG_DRAW_UPPER_LEFT|UCG_DRAW_LOWER_RIGHT|UCG_DRAW_LOWER_LEFT)
void ucg_DrawDisc(ucg_t *ucg, ucg_int_t x0, ucg_int_t y0, ucg_int_t rad, uint8_t option);
void ucg_DrawCircle(ucg_t *ucg, ucg_int_t x0, ucg_int_t y0, ucg_int_t rad, uint8_t option);

/*================================================*/
/* ucg_bitmap.c */
void ucg_DrawTransparentBitmapLine(ucg_t *ucg, ucg_int_t x, ucg_int_t y, ucg_int_t dir, ucg_int_t len, const unsigned char *bitmap);
void ucg_DrawBitmapLine(ucg_t *ucg, ucg_int_t x, ucg_int_t y, ucg_int_t dir, ucg_int_t len, const unsigned char *bitmap);
/* void ucg_DrawRLBitmap(ucg_t *ucg, ucg_int_t x, ucg_int_t y, ucg_int_t dir, const unsigned char *rl_bitmap); */


/*================================================*/
/* ucg_rotate.c */
void ucg_UndoRotate(ucg_t *ucg);
void ucg_SetRotate90(ucg_t *ucg);
void ucg_SetRotate180(ucg_t *ucg);
void ucg_SetRotate270(ucg_t *ucg);

/*================================================*/
/* ucg_scale.c */
void ucg_UndoScale(ucg_t *ucg);
void ucg_SetScale2x2(ucg_t *ucg);


/*================================================*/
/* ucg_polygon.c */

typedef int16_t pg_word_t;

#define PG_NOINLINE UCG_NOINLINE

struct pg_point_struct
{
  pg_word_t x;
  pg_word_t y;
};

typedef struct _pg_struct pg_struct;	/* forward declaration */

struct pg_edge_struct
{
  pg_word_t x_direction;	/* 1, if x2 is greater than x1, -1 otherwise */
  pg_word_t height;
  pg_word_t current_x_offset;
  pg_word_t error_offset;
  
  /* --- line loop --- */
  pg_word_t current_y;
  pg_word_t max_y;
  pg_word_t current_x;
  pg_word_t error;

  /* --- outer loop --- */
  uint8_t (*next_idx_fn)(pg_struct *pg, uint8_t i);
  uint8_t curr_idx;
};

/* maximum number of points in the polygon */
/* can be redefined, but highest possible value is 254 */
#define PG_MAX_POINTS 4

/* index numbers for the pge structures below */
#define PG_LEFT 0
#define PG_RIGHT 1


struct _pg_struct
{
  struct pg_point_struct list[PG_MAX_POINTS];
  uint8_t cnt;
  uint8_t is_min_y_not_flat;
  pg_word_t total_scan_line_cnt;
  struct pg_edge_struct pge[2];	/* left and right line draw structures */
};

void pg_ClearPolygonXY(pg_struct *pg);
void pg_AddPolygonXY(pg_struct *pg, ucg_t *ucg, int16_t x, int16_t y);
void pg_DrawPolygon(pg_struct *pg, ucg_t *ucg);
void ucg_ClearPolygonXY(void);
void ucg_AddPolygonXY(ucg_t *ucg, int16_t x, int16_t y);
void ucg_DrawPolygon(ucg_t *ucg);
void ucg_DrawTriangle(ucg_t *ucg, int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2);
/* the polygon procedure only works for convex tetragons (http://en.wikipedia.org/wiki/Convex_polygon) */
void ucg_DrawTetragon(ucg_t *ucg, int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3);


/*================================================*/
/* ucg_font.c */

//ucg_int_t ucg_draw_transparent_glyph(ucg_t *ucg, ucg_int_t x, ucg_int_t y, uint8_t dir, uint8_t encoding);
//ucg_int_t ucg_draw_solid_glyph(ucg_t *ucg, ucg_int_t x, ucg_int_t y, uint8_t dir, uint8_t encoding);

// old font procedures
//#define UCG_FONT_MODE_TRANSPARENT ucg_draw_transparent_glyph
//#define UCG_FONT_MODE_SOLID ucg_draw_solid_glyph
//#define UCG_FONT_MODE_NONE ((ucg_font_mode_fnptr)0)

// new font procedures
#define UCG_FONT_MODE_TRANSPARENT 1
#define UCG_FONT_MODE_SOLID 0
#define UCG_FONT_MODE_NONE 1


/* Information on a specific given font */
uint8_t ucg_font_GetFontStartEncoding(const void *font);
uint8_t ucg_font_GetFontEndEncoding(const void *font);

uint8_t ucg_font_GetCapitalAHeight(const void *font);

int8_t ucg_font_GetFontAscent(const void *font);
int8_t ucg_font_GetFontDescent(const void *font);

int8_t ucg_font_GetFontXAscent(const void *font);
int8_t ucg_font_GetFontXDescent(const void *font);

size_t ucg_font_GetSize(const void *font);

/* Information on the current font */

uint8_t ucg_GetFontBBXWidth(ucg_t *ucg);
uint8_t ucg_GetFontBBXHeight(ucg_t *ucg);
uint8_t ucg_GetFontCapitalAHeight(ucg_t *ucg) UCG_NOINLINE; 
uint8_t ucg_IsGlyph(ucg_t *ucg, uint8_t requested_encoding);
int8_t ucg_GetGlyphWidth(ucg_t *ucg, uint8_t requested_encoding);

ucg_int_t ucg_GetFontAscent(ucg_t *ucg) {
    return ucg->font_ref_ascent;
}

ucg_int_t ucg_GetFontDescent(ucg_t *ucg) {
    return ucg->font_ref_descent;
}

//#define ucg_GetFontAscent(ucg)	((ucg)->font_ref_ascent)
//#define ucg_GetFontDescent(ucg)	((ucg)->font_ref_descent)

/* Drawing procedures */

ucg_int_t ucg_DrawGlyph(ucg_t *ucg, ucg_int_t x, ucg_int_t y, uint8_t dir, uint8_t encoding);
ucg_int_t ucg_DrawString(ucg_t *ucg, ucg_int_t x, ucg_int_t y, uint8_t dir, const char *str);

/* Mode selection/Font assignment */

void ucg_SetFontRefHeightText(ucg_t *ucg);
void ucg_SetFontRefHeightExtendedText(ucg_t *ucg);
void ucg_SetFontRefHeightAll(ucg_t *ucg);

void ucg_SetFontPosBaseline(ucg_t *ucg) UCG_NOINLINE;
void ucg_SetFontPosBottom(ucg_t *ucg);
void ucg_SetFontPosTop(ucg_t *ucg);
void ucg_SetFontPosCenter(ucg_t *ucg);

void ucg_SetFont(ucg_t *ucg, const ucg_fntpgm_uint8_t  *font);
//void ucg_SetFontMode(ucg_t *ucg, ucg_font_mode_fnptr font_mode);
void ucg_SetFontMode(ucg_t *ucg, uint8_t is_transparent);

ucg_int_t ucg_GetStrWidth(ucg_t *ucg, const char *s);


/*================================================*/
/* LOW LEVEL PROCEDRUES, ONLY CALLED BY DEV CB */

/*================================================*/
/* ucg_clip.c */
ucg_int_t ucg_clip_is_pixel_visible(ucg_t *ucg);
ucg_int_t ucg_clip_l90fx(ucg_t *ucg);
ucg_int_t ucg_clip_l90tc(ucg_t *ucg);
ucg_int_t ucg_clip_l90se(ucg_t *ucg);


/*================================================*/
/* ucg_ccs.c */
void ucg_ccs_init(ucg_ccs_t *ccs, uint8_t start, uint8_t end, ucg_int_t steps);
void ucg_ccs_step(ucg_ccs_t *ccs);
void ucg_ccs_seek(ucg_ccs_t *ccs, ucg_int_t pos);


/*================================================*/
/* ucg_dev_default_cb.c */
ucg_int_t ucg_dev_default_cb(ucg_t *ucg, ucg_int_t msg, void *data);
ucg_int_t ucg_handle_l90fx(ucg_t *ucg, ucg_dev_fnptr dev_cb);
ucg_int_t ucg_handle_l90tc(ucg_t *ucg, ucg_dev_fnptr dev_cb);
ucg_int_t ucg_handle_l90se(ucg_t *ucg, ucg_dev_fnptr dev_cb);
ucg_int_t ucg_handle_l90bf(ucg_t *ucg, ucg_dev_fnptr dev_cb);
void ucg_handle_l90rl(ucg_t *ucg, ucg_dev_fnptr dev_cb);


/*================================================*/
/* ucg_com_msg_api.c */

/* send command bytes and optional arguments */
#define UCG_C10(c0)				0x010, (c0)
#define UCG_C20(c0,c1)				0x020, (c0),(c1)
#define UCG_C11(c0,a0)				0x011, (c0),(a0)
#define UCG_C21(c0,c1,a0)			0x021, (c0),(c1),(a0)
#define UCG_C12(c0,a0,a1)			0x012, (c0),(a0),(a1)
#define UCG_C22(c0,c1,a0,a1)		0x022, (c0),(c1),(a0),(a1)
#define UCG_C13(c0,a0,a1,a2)		0x013, (c0),(a0),(a1),(a2)
#define UCG_C23(c0,c1,a0,a1,a2)		0x023, (c0),(c1),(a0),(a1),(a2)
#define UCG_C14(c0,a0,a1,a2,a3)		0x014, (c0),(a0),(a1),(a2),(a3)
#define UCG_C24(c0,c1,a0,a1,a2,a3)	0x024, (c0),(c1),(a0),(a1),(a2),(a3)
#define UCG_C15(c0,a0,a1,a2,a3,a4)	0x015, (c0),(a0),(a1),(a2),(a3),(a4)


#define UCG_C25(c0,c1,a0,a1,a2,a3,a4)	0x025, (c0),(c1),(a0),(a1),(a2),(a3),(a4)
#define UCG_C16(c0,a0,a1,a2,a3,a4,a5)	0x016, (c0),(a0),(a1),(a2),(a3),(a4),(a5)
#define UCG_C26(c0,c1,a0,a1,a2,a3,a4,a5)	0x026, (c0),(c1),(a0),(a1),(a2),(a3),(a4),(a5)
#define UCG_C17(c0,a0,a1,a2,a3,a4,a5,a6)	0x017, (c0),(a0),(a1),(a2),(a3),(a4),(a5),(a6)
#define UCG_C27(c0,c1,a0,a1,a2,a3,a4,a5,a6)	0x027, (c0),(c1),(a0),(a1),(a2),(a3),(a4),(a5),(a6)
#define UCG_C18(c0,a0,a1,a2,a3,a4,a5,a6,a7)	0x018, (c0),(a0),(a1),(a2),(a3),(a4),(a5),(a6),(a7)
#define UCG_C28(c0,c1,a0,a1,a2,a3,a4,a5,a6,a7)	0x028, (c0),(c1),(a0),(a1),(a2),(a3),(a4),(a5),(a6),(a7)
#define UCG_C19(c0,a0,a1,a2,a3,a4,a5,a6,a7,a8)	0x019, (c0),(a0),(a1),(a2),(a3),(a4),(a5),(a6),(a7),(a8)
#define UCG_C29(c0,c1,a0,a1,a2,a3,a4,a5,a6,a7,a8)	0x029, (c0),(c1),(a0),(a1),(a2),(a3),(a4),(a5),(a6),(a7),(a8)
#define UCG_C1A(c0,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9)	0x01f, (c0),(a0),(a1),(a2),(a3),(a4),(a5),(a6),(a7),(a8),(a9)
#define UCG_C2A(c0,c1,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9)	0x02f, (c0),(c1),(a0),(a1),(a2),(a3),(a4),(a5),(a6),(a7),(a8),(a9)
#define UCG_C1B(c0,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,aa)	0x01f, (c0),(a0),(a1),(a2),(a3),(a4),(a5),(a6),(a7),(a8),(a9),(aa)
#define UCG_C2B(c0,c1,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,aa)	0x02f, (c0),(c1),(a0),(a1),(a2),(a3),(a4),(a5),(a6),(a7),(a8),(a9),(aa)
#define UCG_C1C(c0,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,aa,ab)	0x01f, (c0),(a0),(a1),(a2),(a3),(a4),(a5),(a6),(a7),(a8),(a9),(aa),(ab)
#define UCG_C2C(c0,c1,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,aa,ab)	0x02f, (c0),(c1),(a0),(a1),(a2),(a3),(a4),(a5),(a6),(a7),(a8),(a9),(aa),(ab)
#define UCG_C1D(c0,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,aa,ab,ac)	0x01f, (c0),(a0),(a1),(a2),(a3),(a4),(a5),(a6),(a7),(a8),(a9),(aa),(ab),(ac)
#define UCG_C2D(c0,c1,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,aa,ab,ac)	0x02f, (c0),(c1),(a0),(a1),(a2),(a3),(a4),(a5),(a6),(a7),(a8),(a9),(aa),(ab),(ac)
#define UCG_C1E(c0,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,aa,ab,ac,ad)	0x01f, (c0),(a0),(a1),(a2),(a3),(a4),(a5),(a6),(a7),(a8),(a9),(aa),(ab),(ac),(ad)
#define UCG_C2E(c0,c1,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,aa,ab,ac,ad)	0x02f, (c0),(c1),(a0),(a1),(a2),(a3),(a4),(a5),(a6),(a7),(a8),(a9),(aa),(ab),(ac),(ad)
#define UCG_C1F(c0,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,aa,ab,ac,ad,ae)	0x01f, (c0),(a0),(a1),(a2),(a3),(a4),(a5),(a6),(a7),(a8),(a9),(aa),(ab),(ac),(ad),(ae)
#define UCG_C2F(c0,c1,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,aa,ab,ac,ad,ae)	0x02f, (c0),(c1),(a0),(a1),(a2),(a3),(a4),(a5),(a6),(a7),(a8),(a9),(aa),(ab),(ac),(ad),(ae)
 


/* send one or more argument bytes */
#define UCG_A1(d0)					0x061, (d0)
#define UCG_A2(d0,d1)					0x062, (d0),(d1)
#define UCG_A3(d0,d1,d2)				0x063, (d0),(d1),(d2)
#define UCG_A4(d0,d1,d2,d3)				0x064, (d0),(d1),(d2),(d3)
#define UCG_A5(d0,d1,d2,d3,d4)			0x065, (d0),(d1),(d2),(d3),(d4)
#define UCG_A6(d0,d1,d2,d3,d4,d5)		0x066, (d0),(d1),(d2),(d3),(d4),(d5)
#define UCG_A7(d0,d1,d2,d3,d4,d5,d6)		0x067, (d0),(d1),(d2),(d3),(d4),(d5),(d6)
#define UCG_A8(d0,d1,d2,d3,d4,d5,d6,d7)	0x068, (d0),(d1),(d2),(d3),(d4),(d5),(d6),(d7)

/* force data mode on CD line */
#define UCG_DATA()					0x070
/* send one or more data bytes */
#define UCG_D1(d0)				0x071, (d0)
#define UCG_D2(d0,d1)				0x072, (d0),(d1)
#define UCG_D3(d0,d1,d2)			0x073, (d0),(d1),(d2)
#define UCG_D4(d0,d1,d2,d3)			0x074, (d0),(d1),(d2),(d3)
#define UCG_D5(d0,d1,d2,d3,d4)		0x075, (d0),(d1),(d2),(d3),(d4)
#define UCG_D6(d0,d1,d2,d3,d4,d5)	0x076, (d0),(d1),(d2),(d3),(d4),(d5)

/* delay by specified value. t = [0..4095] */
#define UCG_DLY_MS(t)				0x080 | (((t)>>8)&15), (t)&255
#define UCG_DLY_US(t)				0x090 | (((t)>>8)&15), (t)&255

/* access procedures to ucg->arg.pixel.pos.x und ucg->arg.pixel.pos.y */
#define UCG_VARX(s,a,o)				0x0a0 | ((s)&15), (a), (o)
#define UCG_VARY(s,a,o)				0x0b0 | ((s)&15), (a), (o)

/* force specific level on RST und CS */
#define UCG_RST(level)				0x0f0 | ((level)&1)
#define UCG_CS(level)				0x0f4 | ((level)&1)

/* Configure CD line for command, arguments and data */
/* Configure CMD/DATA line: "c" logic level CMD, "a" logic level CMD Args */
#define UCG_CFG_CD(c,a)			0x0fc | (((c)&1)<<1) | ((a)&1)

/* Termination byte */
#define UCG_END()					0x00

/*
#define ucg_com_SendByte(ucg, byte) \
  (ucg)->com_cb((ucg), UCG_COM_MSG_SEND_BYTE, (byte), NULL)
*/

#define ucg_com_SendRepeat3Bytes(ucg, cnt, byte_ptr) \
  (ucg)->com_cb((ucg), UCG_COM_MSG_REPEAT_3_BYTES, (cnt), (byte_ptr))

void ucg_com_PowerDown(ucg_t *ucg);
int16_t ucg_com_PowerUp(ucg_t *ucg, uint16_t serial_clk_speed, uint16_t parallel_clk_speed);  /* values are nano seconds */
void ucg_com_SetLineStatus(ucg_t *ucg, uint8_t level, uint8_t mask, uint8_t msg) UCG_NOINLINE;
void ucg_com_SetResetLineStatus(ucg_t *ucg, uint8_t level);
void ucg_com_SetCSLineStatus(ucg_t *ucg, uint8_t level);
void ucg_com_SetCDLineStatus(ucg_t *ucg, uint8_t level);
void ucg_com_DelayMicroseconds(ucg_t *ucg, uint16_t delay) UCG_NOINLINE;
void ucg_com_DelayMilliseconds(ucg_t *ucg, uint16_t delay) UCG_NOINLINE;
#ifndef ucg_com_SendByte
void ucg_com_SendByte(ucg_t *ucg, uint8_t byte);
#endif
void ucg_com_SendRepeatByte(ucg_t *ucg, uint16_t cnt, uint8_t byte);
void ucg_com_SendRepeat2Bytes(ucg_t *ucg, uint16_t cnt, uint8_t *byte_ptr);
#ifndef ucg_com_SendRepeat3Bytes
void ucg_com_SendRepeat3Bytes(ucg_t *ucg, uint16_t cnt, uint8_t *byte_ptr);
#endif
void ucg_com_SendString(ucg_t *ucg, uint16_t cnt, const uint8_t *byte_ptr);
void ucg_com_SendCmdDataSequence(ucg_t *ucg, uint16_t cnt, const uint8_t *byte_ptr, uint8_t cd_line_status_at_end);
void ucg_com_SendCmdSeq(ucg_t *ucg, const ucg_pgm_uint8_t *data);


/*================================================*/
/* ucg_dev_tga.c */
int tga_init(uint16_t w, uint16_t h);
void tga_save(const char *name);

ucg_int_t ucg_dev_tga(ucg_t *ucg, ucg_int_t msg, void *data);




/*================================================*/

#ifdef OLD_FONTS

const ucg_fntpgm_uint8_t ucg_font_04b_03b[];
const ucg_fntpgm_uint8_t ucg_font_04b_03bn[];
const ucg_fntpgm_uint8_t ucg_font_04b_03br[];
const ucg_fntpgm_uint8_t ucg_font_04b_03[];
const ucg_fntpgm_uint8_t ucg_font_04b_03n[];
const ucg_fntpgm_uint8_t ucg_font_04b_03r[];
const ucg_fntpgm_uint8_t ucg_font_04b_24[];
const ucg_fntpgm_uint8_t ucg_font_04b_24n[];
const ucg_fntpgm_uint8_t ucg_font_04b_24r[];
const ucg_fntpgm_uint8_t ucg_font_10x20_67_75[];
const ucg_fntpgm_uint8_t ucg_font_10x20_75r[];
const ucg_fntpgm_uint8_t ucg_font_10x20_78_79[];
const ucg_fntpgm_uint8_t ucg_font_10x20[];
const ucg_fntpgm_uint8_t ucg_font_10x20r[];
const ucg_fntpgm_uint8_t ucg_font_4x6[];
const ucg_fntpgm_uint8_t ucg_font_4x6r[];
const ucg_fntpgm_uint8_t ucg_font_5x7[];
const ucg_fntpgm_uint8_t ucg_font_5x7r[];
const ucg_fntpgm_uint8_t ucg_font_5x8[];
const ucg_fntpgm_uint8_t ucg_font_5x8r[];
const ucg_fntpgm_uint8_t ucg_font_6x10[];
const ucg_fntpgm_uint8_t ucg_font_6x10r[];
const ucg_fntpgm_uint8_t ucg_font_6x12_67_75[];
const ucg_fntpgm_uint8_t ucg_font_6x12_75r[];
const ucg_fntpgm_uint8_t ucg_font_6x12_78_79[];
const ucg_fntpgm_uint8_t ucg_font_6x12[];
const ucg_fntpgm_uint8_t ucg_font_6x12r[];
const ucg_fntpgm_uint8_t ucg_font_6x13_67_75[];
const ucg_fntpgm_uint8_t ucg_font_6x13_75r[];
const ucg_fntpgm_uint8_t ucg_font_6x13_78_79[];
const ucg_fntpgm_uint8_t ucg_font_6x13B[];
const ucg_fntpgm_uint8_t ucg_font_6x13Br[];
const ucg_fntpgm_uint8_t ucg_font_6x13[];
const ucg_fntpgm_uint8_t ucg_font_6x13O[];
const ucg_fntpgm_uint8_t ucg_font_6x13Or[];
const ucg_fntpgm_uint8_t ucg_font_6x13r[];
const ucg_fntpgm_uint8_t ucg_font_7x13_67_75[];
const ucg_fntpgm_uint8_t ucg_font_7x13_75r[];
const ucg_fntpgm_uint8_t ucg_font_7x13B[];
const ucg_fntpgm_uint8_t ucg_font_7x13Br[];
const ucg_fntpgm_uint8_t ucg_font_7x13[];
const ucg_fntpgm_uint8_t ucg_font_7x13O[];
const ucg_fntpgm_uint8_t ucg_font_7x13Or[];
const ucg_fntpgm_uint8_t ucg_font_7x13r[];
const ucg_fntpgm_uint8_t ucg_font_7x14B[];
const ucg_fntpgm_uint8_t ucg_font_7x14Br[];
const ucg_fntpgm_uint8_t ucg_font_7x14[];
const ucg_fntpgm_uint8_t ucg_font_7x14r[];
const ucg_fntpgm_uint8_t ucg_font_8x13_67_75[];
const ucg_fntpgm_uint8_t ucg_font_8x13_75r[];
const ucg_fntpgm_uint8_t ucg_font_8x13B[];
const ucg_fntpgm_uint8_t ucg_font_8x13Br[];
const ucg_fntpgm_uint8_t ucg_font_8x13[];
const ucg_fntpgm_uint8_t ucg_font_8x13O[];
const ucg_fntpgm_uint8_t ucg_font_8x13Or[];
const ucg_fntpgm_uint8_t ucg_font_8x13r[];
const ucg_fntpgm_uint8_t ucg_font_9x15_67_75[];
const ucg_fntpgm_uint8_t ucg_font_9x15_75r[];
const ucg_fntpgm_uint8_t ucg_font_9x15_78_79[];
const ucg_fntpgm_uint8_t ucg_font_9x15B[];
const ucg_fntpgm_uint8_t ucg_font_9x15Br[];
const ucg_fntpgm_uint8_t ucg_font_9x15[];
const ucg_fntpgm_uint8_t ucg_font_9x15r[];
const ucg_fntpgm_uint8_t ucg_font_9x18_67_75[];
const ucg_fntpgm_uint8_t ucg_font_9x18_75r[];
const ucg_fntpgm_uint8_t ucg_font_9x18_78_79[];
const ucg_fntpgm_uint8_t ucg_font_9x18B[];
const ucg_fntpgm_uint8_t ucg_font_9x18Br[];
const ucg_fntpgm_uint8_t ucg_font_9x18[];
const ucg_fntpgm_uint8_t ucg_font_9x18r[];
const ucg_fntpgm_uint8_t ucg_font_baby[];
const ucg_fntpgm_uint8_t ucg_font_babyn[];
const ucg_fntpgm_uint8_t ucg_font_babyr[];
const ucg_fntpgm_uint8_t ucg_font_blipfest_07[];
const ucg_fntpgm_uint8_t ucg_font_blipfest_07n[];
const ucg_fntpgm_uint8_t ucg_font_blipfest_07r[];
const ucg_fntpgm_uint8_t ucg_font_chikita[];
const ucg_fntpgm_uint8_t ucg_font_chikitan[];
const ucg_fntpgm_uint8_t ucg_font_chikitar[];
const ucg_fntpgm_uint8_t ucg_font_courB08[];
const ucg_fntpgm_uint8_t ucg_font_courB08r[];
const ucg_fntpgm_uint8_t ucg_font_courB10[];
const ucg_fntpgm_uint8_t ucg_font_courB10r[];
const ucg_fntpgm_uint8_t ucg_font_courB12[];
const ucg_fntpgm_uint8_t ucg_font_courB12r[];
const ucg_fntpgm_uint8_t ucg_font_courB14[];
const ucg_fntpgm_uint8_t ucg_font_courB14r[];
const ucg_fntpgm_uint8_t ucg_font_courB18[];
const ucg_fntpgm_uint8_t ucg_font_courB18r[];
const ucg_fntpgm_uint8_t ucg_font_courB24[];
const ucg_fntpgm_uint8_t ucg_font_courB24r[];
const ucg_fntpgm_uint8_t ucg_font_courB24n[];
const ucg_fntpgm_uint8_t ucg_font_courR08[];
const ucg_fntpgm_uint8_t ucg_font_courR08r[];
const ucg_fntpgm_uint8_t ucg_font_courR10[];
const ucg_fntpgm_uint8_t ucg_font_courR10r[];
const ucg_fntpgm_uint8_t ucg_font_courR12[];
const ucg_fntpgm_uint8_t ucg_font_courR12r[];
const ucg_fntpgm_uint8_t ucg_font_courR14[];
const ucg_fntpgm_uint8_t ucg_font_courR14r[];
const ucg_fntpgm_uint8_t ucg_font_courR18[];
const ucg_fntpgm_uint8_t ucg_font_courR18r[];
const ucg_fntpgm_uint8_t ucg_font_courR24[];
const ucg_fntpgm_uint8_t ucg_font_courR24n[];
const ucg_fntpgm_uint8_t ucg_font_courR24r[];
const ucg_fntpgm_uint8_t ucg_font_cu12_67_75[];
const ucg_fntpgm_uint8_t ucg_font_cu12_75r[];
const ucg_fntpgm_uint8_t ucg_font_cu12[];
const ucg_fntpgm_uint8_t ucg_font_cursor[];
const ucg_fntpgm_uint8_t ucg_font_cursorr[];
const ucg_fntpgm_uint8_t ucg_font_fixed_v0[];
const ucg_fntpgm_uint8_t ucg_font_fixed_v0n[];
const ucg_fntpgm_uint8_t ucg_font_fixed_v0r[];
const ucg_fntpgm_uint8_t ucg_font_freedoomr10r[];
const ucg_fntpgm_uint8_t ucg_font_freedoomr25n[];
const ucg_fntpgm_uint8_t ucg_font_helvB08[];
const ucg_fntpgm_uint8_t ucg_font_helvB08r[];
const ucg_fntpgm_uint8_t ucg_font_helvB10[];
const ucg_fntpgm_uint8_t ucg_font_helvB10r[];
const ucg_fntpgm_uint8_t ucg_font_helvB12[];
const ucg_fntpgm_uint8_t ucg_font_helvB12r[];
const ucg_fntpgm_uint8_t ucg_font_helvB14[];
const ucg_fntpgm_uint8_t ucg_font_helvB14r[];
const ucg_fntpgm_uint8_t ucg_font_helvB18[];
const ucg_fntpgm_uint8_t ucg_font_helvB18r[];
const ucg_fntpgm_uint8_t ucg_font_helvB24[];
const ucg_fntpgm_uint8_t ucg_font_helvB24n[];
const ucg_fntpgm_uint8_t ucg_font_helvB24r[];
const ucg_fntpgm_uint8_t ucg_font_helvR08[];
const ucg_fntpgm_uint8_t ucg_font_helvR08r[];
const ucg_fntpgm_uint8_t ucg_font_helvR10[];
const ucg_fntpgm_uint8_t ucg_font_helvR10r[];
const ucg_fntpgm_uint8_t ucg_font_helvR12[];
const ucg_fntpgm_uint8_t ucg_font_helvR12r[];
const ucg_fntpgm_uint8_t ucg_font_helvR14[];
const ucg_fntpgm_uint8_t ucg_font_helvR14r[];
const ucg_fntpgm_uint8_t ucg_font_helvR18[];
const ucg_fntpgm_uint8_t ucg_font_helvR18r[];
const ucg_fntpgm_uint8_t ucg_font_helvR24[];
const ucg_fntpgm_uint8_t ucg_font_helvR24n[];
const ucg_fntpgm_uint8_t ucg_font_helvR24r[];
const ucg_fntpgm_uint8_t ucg_font_lucasfont_alternate[];
const ucg_fntpgm_uint8_t ucg_font_lucasfont_alternaten[];
const ucg_fntpgm_uint8_t ucg_font_lucasfont_alternater[];
const ucg_fntpgm_uint8_t ucg_font_m2icon_5[];
const ucg_fntpgm_uint8_t ucg_font_m2icon_7[];
const ucg_fntpgm_uint8_t ucg_font_m2icon_9[];
const ucg_fntpgm_uint8_t ucg_font_micro[];
const ucg_fntpgm_uint8_t ucg_font_ncenB08[];
const ucg_fntpgm_uint8_t ucg_font_ncenB08r[];
const ucg_fntpgm_uint8_t ucg_font_ncenB10[];
const ucg_fntpgm_uint8_t ucg_font_ncenB10r[];
const ucg_fntpgm_uint8_t ucg_font_ncenB12[];
const ucg_fntpgm_uint8_t ucg_font_ncenB12r[];
const ucg_fntpgm_uint8_t ucg_font_ncenB14[];
const ucg_fntpgm_uint8_t ucg_font_ncenB14r[];
const ucg_fntpgm_uint8_t ucg_font_ncenB18[];
const ucg_fntpgm_uint8_t ucg_font_ncenB18r[];
const ucg_fntpgm_uint8_t ucg_font_ncenB24[];
const ucg_fntpgm_uint8_t ucg_font_ncenB24n[];
const ucg_fntpgm_uint8_t ucg_font_ncenB24r[];
const ucg_fntpgm_uint8_t ucg_font_ncenR08[];
const ucg_fntpgm_uint8_t ucg_font_ncenR08r[];
const ucg_fntpgm_uint8_t ucg_font_ncenR10[];
const ucg_fntpgm_uint8_t ucg_font_ncenR10r[];
const ucg_fntpgm_uint8_t ucg_font_ncenR12[];
const ucg_fntpgm_uint8_t ucg_font_ncenR12r[];
const ucg_fntpgm_uint8_t ucg_font_ncenR14[];
const ucg_fntpgm_uint8_t ucg_font_ncenR14r[];
const ucg_fntpgm_uint8_t ucg_font_ncenR18[];
const ucg_fntpgm_uint8_t ucg_font_ncenR18r[];
const ucg_fntpgm_uint8_t ucg_font_ncenR24[];
const ucg_fntpgm_uint8_t ucg_font_ncenR24n[];
const ucg_fntpgm_uint8_t ucg_font_ncenR24r[];
const ucg_fntpgm_uint8_t ucg_font_orgv01[];
const ucg_fntpgm_uint8_t ucg_font_orgv01n[];
const ucg_fntpgm_uint8_t ucg_font_orgv01r[];
const ucg_fntpgm_uint8_t ucg_font_p01type[];
const ucg_fntpgm_uint8_t ucg_font_p01typen[];
const ucg_fntpgm_uint8_t ucg_font_p01typer[];
const ucg_fntpgm_uint8_t ucg_font_pixelle_micro[];
const ucg_fntpgm_uint8_t ucg_font_pixelle_micron[];
const ucg_fntpgm_uint8_t ucg_font_pixelle_micror[];
const ucg_fntpgm_uint8_t ucg_font_profont10[];
const ucg_fntpgm_uint8_t ucg_font_profont10r[];
const ucg_fntpgm_uint8_t ucg_font_profont11[];
const ucg_fntpgm_uint8_t ucg_font_profont11r[];
const ucg_fntpgm_uint8_t ucg_font_profont12[];
const ucg_fntpgm_uint8_t ucg_font_profont12r[];
const ucg_fntpgm_uint8_t ucg_font_profont15[];
const ucg_fntpgm_uint8_t ucg_font_profont15r[];
const ucg_fntpgm_uint8_t ucg_font_profont17[];
const ucg_fntpgm_uint8_t ucg_font_profont17r[];
const ucg_fntpgm_uint8_t ucg_font_profont22[];
const ucg_fntpgm_uint8_t ucg_font_profont22n[];
const ucg_fntpgm_uint8_t ucg_font_profont22r[];
const ucg_fntpgm_uint8_t ucg_font_profont29[];
const ucg_fntpgm_uint8_t ucg_font_profont29n[];
const ucg_fntpgm_uint8_t ucg_font_profont29r[];
const ucg_fntpgm_uint8_t ucg_font_robot_de_niro[];
const ucg_fntpgm_uint8_t ucg_font_robot_de_niron[];
const ucg_fntpgm_uint8_t ucg_font_robot_de_niror[];
const ucg_fntpgm_uint8_t ucg_font_symb08[];
const ucg_fntpgm_uint8_t ucg_font_symb08r[];
const ucg_fntpgm_uint8_t ucg_font_symb10[];
const ucg_fntpgm_uint8_t ucg_font_symb10r[];
const ucg_fntpgm_uint8_t ucg_font_symb12[];
const ucg_fntpgm_uint8_t ucg_font_symb12r[];
const ucg_fntpgm_uint8_t ucg_font_symb14[];
const ucg_fntpgm_uint8_t ucg_font_symb14r[];
const ucg_fntpgm_uint8_t ucg_font_symb18[];
const ucg_fntpgm_uint8_t ucg_font_symb18r[];
const ucg_fntpgm_uint8_t ucg_font_symb24[];
const ucg_fntpgm_uint8_t ucg_font_symb24r[];
const ucg_fntpgm_uint8_t ucg_font_timB08[];
const ucg_fntpgm_uint8_t ucg_font_timB08r[];
const ucg_fntpgm_uint8_t ucg_font_timB10[];
const ucg_fntpgm_uint8_t ucg_font_timB10r[];
const ucg_fntpgm_uint8_t ucg_font_timB12[];
const ucg_fntpgm_uint8_t ucg_font_timB12r[];
const ucg_fntpgm_uint8_t ucg_font_timB14[];
const ucg_fntpgm_uint8_t ucg_font_timB14r[];
const ucg_fntpgm_uint8_t ucg_font_timB18[];
const ucg_fntpgm_uint8_t ucg_font_timB18r[];
const ucg_fntpgm_uint8_t ucg_font_timB24[];
const ucg_fntpgm_uint8_t ucg_font_timB24n[];
const ucg_fntpgm_uint8_t ucg_font_timB24r[];
const ucg_fntpgm_uint8_t ucg_font_timR08[];
const ucg_fntpgm_uint8_t ucg_font_timR08r[];
const ucg_fntpgm_uint8_t ucg_font_timR10[];
const ucg_fntpgm_uint8_t ucg_font_timR10r[];
const ucg_fntpgm_uint8_t ucg_font_timR12[];
const ucg_fntpgm_uint8_t ucg_font_timR12r[];
const ucg_fntpgm_uint8_t ucg_font_timR14[];
const ucg_fntpgm_uint8_t ucg_font_timR14r[];
const ucg_fntpgm_uint8_t ucg_font_timR18[];
const ucg_fntpgm_uint8_t ucg_font_timR18r[];
const ucg_fntpgm_uint8_t ucg_font_timR24[];
const ucg_fntpgm_uint8_t ucg_font_timR24n[];
const ucg_fntpgm_uint8_t ucg_font_timR24r[];
const ucg_fntpgm_uint8_t ucg_font_tpssb[];
const ucg_fntpgm_uint8_t ucg_font_tpssbn[];
const ucg_fntpgm_uint8_t ucg_font_tpssbr[];
const ucg_fntpgm_uint8_t ucg_font_tpss[];
const ucg_fntpgm_uint8_t ucg_font_tpssn[];
const ucg_fntpgm_uint8_t ucg_font_tpssr[];
const ucg_fntpgm_uint8_t ucg_font_trixel_square[];
const ucg_fntpgm_uint8_t ucg_font_trixel_squaren[];
const ucg_fntpgm_uint8_t ucg_font_trixel_squarer[];
const ucg_fntpgm_uint8_t ucg_font_u8glib_4[];
const ucg_fntpgm_uint8_t ucg_font_u8glib_4r[];
const ucg_fntpgm_uint8_t ucg_font_unifont_0_8[];
const ucg_fntpgm_uint8_t ucg_font_unifont_12_13[];
const ucg_fntpgm_uint8_t ucg_font_unifont_18_19[];
const ucg_fntpgm_uint8_t ucg_font_unifont_2_3[];
const ucg_fntpgm_uint8_t ucg_font_unifont_4_5[];
const ucg_fntpgm_uint8_t ucg_font_unifont_67_75[];
const ucg_fntpgm_uint8_t ucg_font_unifont_72_73[];
const ucg_fntpgm_uint8_t ucg_font_unifont_75r[];
const ucg_fntpgm_uint8_t ucg_font_unifont_76[];
const ucg_fntpgm_uint8_t ucg_font_unifont_77[];
const ucg_fntpgm_uint8_t ucg_font_unifont_78_79[];
const ucg_fntpgm_uint8_t ucg_font_unifont_86[];
const ucg_fntpgm_uint8_t ucg_font_unifont_8_9[];
const ucg_fntpgm_uint8_t ucg_font_unifont[];
const ucg_fntpgm_uint8_t ucg_font_unifontr[];
const ucg_fntpgm_uint8_t ucg_font_fub11[];
const ucg_fntpgm_uint8_t ucg_font_fub11n[];
const ucg_fntpgm_uint8_t ucg_font_fub11r[];
const ucg_fntpgm_uint8_t ucg_font_fub14[];
const ucg_fntpgm_uint8_t ucg_font_fub14n[];
const ucg_fntpgm_uint8_t ucg_font_fub14r[];
const ucg_fntpgm_uint8_t ucg_font_fub17[];
const ucg_fntpgm_uint8_t ucg_font_fub17n[];
const ucg_fntpgm_uint8_t ucg_font_fub17r[];
const ucg_fntpgm_uint8_t ucg_font_fub20[];
const ucg_fntpgm_uint8_t ucg_font_fub20n[];
const ucg_fntpgm_uint8_t ucg_font_fub20r[];
const ucg_fntpgm_uint8_t ucg_font_fub25[];
const ucg_fntpgm_uint8_t ucg_font_fub25n[];
const ucg_fntpgm_uint8_t ucg_font_fub25r[];
const ucg_fntpgm_uint8_t ucg_font_fub30[];
const ucg_fntpgm_uint8_t ucg_font_fub30n[];
const ucg_fntpgm_uint8_t ucg_font_fub30r[];
const ucg_fntpgm_uint8_t ucg_font_fub35n[];
const ucg_fntpgm_uint8_t ucg_font_fub42n[];
const ucg_fntpgm_uint8_t ucg_font_fub49n[];
const ucg_fntpgm_uint8_t ucg_font_fur11[];
const ucg_fntpgm_uint8_t ucg_font_fur11n[];
const ucg_fntpgm_uint8_t ucg_font_fur11r[];
const ucg_fntpgm_uint8_t ucg_font_fur14[];
const ucg_fntpgm_uint8_t ucg_font_fur14n[];
const ucg_fntpgm_uint8_t ucg_font_fur14r[];
const ucg_fntpgm_uint8_t ucg_font_fur17[];
const ucg_fntpgm_uint8_t ucg_font_fur17n[];
const ucg_fntpgm_uint8_t ucg_font_fur17r[];
const ucg_fntpgm_uint8_t ucg_font_fur20[];
const ucg_fntpgm_uint8_t ucg_font_fur20n[];
const ucg_fntpgm_uint8_t ucg_font_fur20r[];
const ucg_fntpgm_uint8_t ucg_font_fur25[];
const ucg_fntpgm_uint8_t ucg_font_fur25n[];
const ucg_fntpgm_uint8_t ucg_font_fur25r[];
const ucg_fntpgm_uint8_t ucg_font_fur30[];
const ucg_fntpgm_uint8_t ucg_font_fur30n[];
const ucg_fntpgm_uint8_t ucg_font_fur30r[];
const ucg_fntpgm_uint8_t ucg_font_fur35n[];
const ucg_fntpgm_uint8_t ucg_font_fur42n[];
const ucg_fntpgm_uint8_t ucg_font_fur49n[];
const ucg_fntpgm_uint8_t ucg_font_logisoso16[];
const ucg_fntpgm_uint8_t ucg_font_logisoso16n[];
const ucg_fntpgm_uint8_t ucg_font_logisoso16r[];
const ucg_fntpgm_uint8_t ucg_font_logisoso18[];
const ucg_fntpgm_uint8_t ucg_font_logisoso18n[];
const ucg_fntpgm_uint8_t ucg_font_logisoso18r[];
const ucg_fntpgm_uint8_t ucg_font_logisoso20[];
const ucg_fntpgm_uint8_t ucg_font_logisoso20n[];
const ucg_fntpgm_uint8_t ucg_font_logisoso20r[];
const ucg_fntpgm_uint8_t ucg_font_logisoso22[];
const ucg_fntpgm_uint8_t ucg_font_logisoso22n[];
const ucg_fntpgm_uint8_t ucg_font_logisoso22r[];
const ucg_fntpgm_uint8_t ucg_font_logisoso24[];
const ucg_fntpgm_uint8_t ucg_font_logisoso24n[];
const ucg_fntpgm_uint8_t ucg_font_logisoso24r[];
const ucg_fntpgm_uint8_t ucg_font_logisoso26[];
const ucg_fntpgm_uint8_t ucg_font_logisoso26n[];
const ucg_fntpgm_uint8_t ucg_font_logisoso26r[];
const ucg_fntpgm_uint8_t ucg_font_logisoso28[];
const ucg_fntpgm_uint8_t ucg_font_logisoso28n[];
const ucg_fntpgm_uint8_t ucg_font_logisoso28r[];
const ucg_fntpgm_uint8_t ucg_font_logisoso30[];
const ucg_fntpgm_uint8_t ucg_font_logisoso30n[];
const ucg_fntpgm_uint8_t ucg_font_logisoso30r[];
const ucg_fntpgm_uint8_t ucg_font_logisoso32[];
const ucg_fntpgm_uint8_t ucg_font_logisoso32n[];
const ucg_fntpgm_uint8_t ucg_font_logisoso32r[];
const ucg_fntpgm_uint8_t ucg_font_logisoso34[];
const ucg_fntpgm_uint8_t ucg_font_logisoso34n[];
const ucg_fntpgm_uint8_t ucg_font_logisoso34r[];
const ucg_fntpgm_uint8_t ucg_font_logisoso38[];
const ucg_fntpgm_uint8_t ucg_font_logisoso38n[];
const ucg_fntpgm_uint8_t ucg_font_logisoso38r[];
const ucg_fntpgm_uint8_t ucg_font_logisoso42[];
const ucg_fntpgm_uint8_t ucg_font_logisoso42n[];
const ucg_fntpgm_uint8_t ucg_font_logisoso42r[];
const ucg_fntpgm_uint8_t ucg_font_logisoso46n[];
const ucg_fntpgm_uint8_t ucg_font_logisoso46r[];
const ucg_fntpgm_uint8_t ucg_font_logisoso50n[];
const ucg_fntpgm_uint8_t ucg_font_logisoso50r[];
const ucg_fntpgm_uint8_t ucg_font_logisoso54n[];
const ucg_fntpgm_uint8_t ucg_font_logisoso58n[];
const ucg_fntpgm_uint8_t ucg_font_logisoso62n[];
const ucg_fntpgm_uint8_t ucg_font_osb18[];
const ucg_fntpgm_uint8_t ucg_font_osb18n[];
const ucg_fntpgm_uint8_t ucg_font_osb18r[];
const ucg_fntpgm_uint8_t ucg_font_osb21[];
const ucg_fntpgm_uint8_t ucg_font_osb21n[];
const ucg_fntpgm_uint8_t ucg_font_osb21r[];
const ucg_fntpgm_uint8_t ucg_font_osb26[];
const ucg_fntpgm_uint8_t ucg_font_osb26n[];
const ucg_fntpgm_uint8_t ucg_font_osb26r[];
const ucg_fntpgm_uint8_t ucg_font_osb29[];
const ucg_fntpgm_uint8_t ucg_font_osb29n[];
const ucg_fntpgm_uint8_t ucg_font_osb29r[];
const ucg_fntpgm_uint8_t ucg_font_osb35[];
const ucg_fntpgm_uint8_t ucg_font_osb35n[];
const ucg_fntpgm_uint8_t ucg_font_osb35r[];
const ucg_fntpgm_uint8_t ucg_font_osr18[];
const ucg_fntpgm_uint8_t ucg_font_osr18n[];
const ucg_fntpgm_uint8_t ucg_font_osr18r[];
const ucg_fntpgm_uint8_t ucg_font_osr21[];
const ucg_fntpgm_uint8_t ucg_font_osr21n[];
const ucg_fntpgm_uint8_t ucg_font_osr21r[];
const ucg_fntpgm_uint8_t ucg_font_osr26[];
const ucg_fntpgm_uint8_t ucg_font_osr26n[];
const ucg_fntpgm_uint8_t ucg_font_osr26r[];
const ucg_fntpgm_uint8_t ucg_font_osr29[];
const ucg_fntpgm_uint8_t ucg_font_osr29n[];
const ucg_fntpgm_uint8_t ucg_font_osr29r[];
const ucg_fntpgm_uint8_t ucg_font_osr35[];
const ucg_fntpgm_uint8_t ucg_font_osr35n[];
const ucg_fntpgm_uint8_t ucg_font_osr35r[];

#else

const ucg_fntpgm_uint8_t ucg_font_04b_03b_hf[];
const ucg_fntpgm_uint8_t ucg_font_04b_03b_hn[];
const ucg_fntpgm_uint8_t ucg_font_04b_03b_hr[];
const ucg_fntpgm_uint8_t ucg_font_04b_03b_tf[];
const ucg_fntpgm_uint8_t ucg_font_04b_03b_tn[];
const ucg_fntpgm_uint8_t ucg_font_04b_03b_tr[];
const ucg_fntpgm_uint8_t ucg_font_04b_03_hf[];
const ucg_fntpgm_uint8_t ucg_font_04b_03_hn[];
const ucg_fntpgm_uint8_t ucg_font_04b_03_hr[];
const ucg_fntpgm_uint8_t ucg_font_04b_03_tf[];
const ucg_fntpgm_uint8_t ucg_font_04b_03_tn[];
const ucg_fntpgm_uint8_t ucg_font_04b_03_tr[];
const ucg_fntpgm_uint8_t ucg_font_04b_24_hf[];
const ucg_fntpgm_uint8_t ucg_font_04b_24_hn[];
const ucg_fntpgm_uint8_t ucg_font_04b_24_hr[];
const ucg_fntpgm_uint8_t ucg_font_04b_24_tf[];
const ucg_fntpgm_uint8_t ucg_font_04b_24_tn[];
const ucg_fntpgm_uint8_t ucg_font_04b_24_tr[];
const ucg_fntpgm_uint8_t ucg_font_10x20_67_75[];
const ucg_fntpgm_uint8_t ucg_font_10x20_78_79[];
const ucg_fntpgm_uint8_t ucg_font_10x20_mf[];
const ucg_fntpgm_uint8_t ucg_font_10x20_mr[];
const ucg_fntpgm_uint8_t ucg_font_10x20_tf[];
const ucg_fntpgm_uint8_t ucg_font_10x20_tr[];
const ucg_fntpgm_uint8_t ucg_font_4x6_mf[];
const ucg_fntpgm_uint8_t ucg_font_4x6_mr[];
const ucg_fntpgm_uint8_t ucg_font_4x6_tf[];
const ucg_fntpgm_uint8_t ucg_font_4x6_tr[];
const ucg_fntpgm_uint8_t ucg_font_5x7_8f[];
const ucg_fntpgm_uint8_t ucg_font_5x7_8r[];
const ucg_fntpgm_uint8_t ucg_font_5x7_mf[];
const ucg_fntpgm_uint8_t ucg_font_5x7_mr[];
const ucg_fntpgm_uint8_t ucg_font_5x7_tf[];
const ucg_fntpgm_uint8_t ucg_font_5x7_tr[];
const ucg_fntpgm_uint8_t ucg_font_5x8_8f[];
const ucg_fntpgm_uint8_t ucg_font_5x8_8r[];
const ucg_fntpgm_uint8_t ucg_font_5x8_mf[];
const ucg_fntpgm_uint8_t ucg_font_5x8_mr[];
const ucg_fntpgm_uint8_t ucg_font_5x8_tf[];
const ucg_fntpgm_uint8_t ucg_font_5x8_tr[];
const ucg_fntpgm_uint8_t ucg_font_6x10_mf[];
const ucg_fntpgm_uint8_t ucg_font_6x10_mr[];
const ucg_fntpgm_uint8_t ucg_font_6x10_tf[];
const ucg_fntpgm_uint8_t ucg_font_6x10_tr[];
const ucg_fntpgm_uint8_t ucg_font_6x12_67_75[];
const ucg_fntpgm_uint8_t ucg_font_6x12_75[];
const ucg_fntpgm_uint8_t ucg_font_6x12_78_79[];
const ucg_fntpgm_uint8_t ucg_font_6x12_mf[];
const ucg_fntpgm_uint8_t ucg_font_6x12_mr[];
const ucg_fntpgm_uint8_t ucg_font_6x12_tf[];
const ucg_fntpgm_uint8_t ucg_font_6x12_tr[];
const ucg_fntpgm_uint8_t ucg_font_6x13_67_75[];
const ucg_fntpgm_uint8_t ucg_font_6x13_78_79[];
const ucg_fntpgm_uint8_t ucg_font_6x13B_tf[];
const ucg_fntpgm_uint8_t ucg_font_6x13B_tr[];
const ucg_fntpgm_uint8_t ucg_font_6x13_mf[];
const ucg_fntpgm_uint8_t ucg_font_6x13_mr[];
const ucg_fntpgm_uint8_t ucg_font_6x13O_tf[];
const ucg_fntpgm_uint8_t ucg_font_6x13O_tr[];
const ucg_fntpgm_uint8_t ucg_font_6x13_tf[];
const ucg_fntpgm_uint8_t ucg_font_6x13_tr[];
const ucg_fntpgm_uint8_t ucg_font_7x13_67_75[];
const ucg_fntpgm_uint8_t ucg_font_7x13_78_79[];
const ucg_fntpgm_uint8_t ucg_font_7x13B_tf[];
const ucg_fntpgm_uint8_t ucg_font_7x13B_tr[];
const ucg_fntpgm_uint8_t ucg_font_7x13_mf[];
const ucg_fntpgm_uint8_t ucg_font_7x13_mr[];
const ucg_fntpgm_uint8_t ucg_font_7x13O_tf[];
const ucg_fntpgm_uint8_t ucg_font_7x13O_tr[];
const ucg_fntpgm_uint8_t ucg_font_7x13_tf[];
const ucg_fntpgm_uint8_t ucg_font_7x13_tr[];
const ucg_fntpgm_uint8_t ucg_font_7x14B_mf[];
const ucg_fntpgm_uint8_t ucg_font_7x14B_mr[];
const ucg_fntpgm_uint8_t ucg_font_7x14B_tf[];
const ucg_fntpgm_uint8_t ucg_font_7x14B_tr[];
const ucg_fntpgm_uint8_t ucg_font_7x14_mf[];
const ucg_fntpgm_uint8_t ucg_font_7x14_mr[];
const ucg_fntpgm_uint8_t ucg_font_7x14_tf[];
const ucg_fntpgm_uint8_t ucg_font_7x14_tr[];
const ucg_fntpgm_uint8_t ucg_font_8x13_67_75[];
const ucg_fntpgm_uint8_t ucg_font_8x13B_mf[];
const ucg_fntpgm_uint8_t ucg_font_8x13B_mr[];
const ucg_fntpgm_uint8_t ucg_font_8x13B_tf[];
const ucg_fntpgm_uint8_t ucg_font_8x13B_tr[];
const ucg_fntpgm_uint8_t ucg_font_8x13_mf[];
const ucg_fntpgm_uint8_t ucg_font_8x13_mr[];
const ucg_fntpgm_uint8_t ucg_font_8x13O_mf[];
const ucg_fntpgm_uint8_t ucg_font_8x13O_mr[];
const ucg_fntpgm_uint8_t ucg_font_8x13O_tf[];
const ucg_fntpgm_uint8_t ucg_font_8x13O_tr[];
const ucg_fntpgm_uint8_t ucg_font_8x13_tf[];
const ucg_fntpgm_uint8_t ucg_font_8x13_tr[];
const ucg_fntpgm_uint8_t ucg_font_9x15_67_75[];
const ucg_fntpgm_uint8_t ucg_font_9x15_78_79[];
const ucg_fntpgm_uint8_t ucg_font_9x15B_mf[];
const ucg_fntpgm_uint8_t ucg_font_9x15B_mr[];
const ucg_fntpgm_uint8_t ucg_font_9x15B_tf[];
const ucg_fntpgm_uint8_t ucg_font_9x15B_tr[];
const ucg_fntpgm_uint8_t ucg_font_9x15_mf[];
const ucg_fntpgm_uint8_t ucg_font_9x15_mr[];
const ucg_fntpgm_uint8_t ucg_font_9x15_tf[];
const ucg_fntpgm_uint8_t ucg_font_9x15_tr[];
const ucg_fntpgm_uint8_t ucg_font_9x18_67_75[];
const ucg_fntpgm_uint8_t ucg_font_9x18_78_79[];
const ucg_fntpgm_uint8_t ucg_font_9x18B_mf[];
const ucg_fntpgm_uint8_t ucg_font_9x18B_mr[];
const ucg_fntpgm_uint8_t ucg_font_9x18B_tf[];
const ucg_fntpgm_uint8_t ucg_font_9x18B_tr[];
const ucg_fntpgm_uint8_t ucg_font_9x18_mf[];
const ucg_fntpgm_uint8_t ucg_font_9x18_mr[];
const ucg_fntpgm_uint8_t ucg_font_9x18_tf[];
const ucg_fntpgm_uint8_t ucg_font_9x18_tr[];
const ucg_fntpgm_uint8_t ucg_font_amstrad_cpc_8f[];
const ucg_fntpgm_uint8_t ucg_font_amstrad_cpc_8r[];
const ucg_fntpgm_uint8_t ucg_font_baby_hf[];
const ucg_fntpgm_uint8_t ucg_font_baby_hn[];
const ucg_fntpgm_uint8_t ucg_font_baby_hr[];
const ucg_fntpgm_uint8_t ucg_font_baby_tf[];
const ucg_fntpgm_uint8_t ucg_font_baby_tn[];
const ucg_fntpgm_uint8_t ucg_font_baby_tr[];
const ucg_fntpgm_uint8_t ucg_font_blipfest_07_hf[];
const ucg_fntpgm_uint8_t ucg_font_blipfest_07_hn[];
const ucg_fntpgm_uint8_t ucg_font_blipfest_07_hr[];
const ucg_fntpgm_uint8_t ucg_font_blipfest_07_tf[];
const ucg_fntpgm_uint8_t ucg_font_blipfest_07_tn[];
const ucg_fntpgm_uint8_t ucg_font_blipfest_07_tr[];
const ucg_fntpgm_uint8_t ucg_font_chikita_hf[];
const ucg_fntpgm_uint8_t ucg_font_chikita_hn[];
const ucg_fntpgm_uint8_t ucg_font_chikita_hr[];
const ucg_fntpgm_uint8_t ucg_font_chikita_tf[];
const ucg_fntpgm_uint8_t ucg_font_chikita_tn[];
const ucg_fntpgm_uint8_t ucg_font_chikita_tr[];
const ucg_fntpgm_uint8_t ucg_font_courB08_mf[];
const ucg_fntpgm_uint8_t ucg_font_courB08_mr[];
const ucg_fntpgm_uint8_t ucg_font_courB08_tf[];
const ucg_fntpgm_uint8_t ucg_font_courB08_tr[];
const ucg_fntpgm_uint8_t ucg_font_courB10_mf[];
const ucg_fntpgm_uint8_t ucg_font_courB10_mr[];
const ucg_fntpgm_uint8_t ucg_font_courB10_tf[];
const ucg_fntpgm_uint8_t ucg_font_courB10_tr[];
const ucg_fntpgm_uint8_t ucg_font_courB12_mf[];
const ucg_fntpgm_uint8_t ucg_font_courB12_mr[];
const ucg_fntpgm_uint8_t ucg_font_courB12_tf[];
const ucg_fntpgm_uint8_t ucg_font_courB12_tr[];
const ucg_fntpgm_uint8_t ucg_font_courB14_mf[];
const ucg_fntpgm_uint8_t ucg_font_courB14_mr[];
const ucg_fntpgm_uint8_t ucg_font_courB14_tf[];
const ucg_fntpgm_uint8_t ucg_font_courB14_tr[];
const ucg_fntpgm_uint8_t ucg_font_courB18_mf[];
const ucg_fntpgm_uint8_t ucg_font_courB18_mn[];
const ucg_fntpgm_uint8_t ucg_font_courB18_mr[];
const ucg_fntpgm_uint8_t ucg_font_courB18_tf[];
const ucg_fntpgm_uint8_t ucg_font_courB18_tn[];
const ucg_fntpgm_uint8_t ucg_font_courB18_tr[];
const ucg_fntpgm_uint8_t ucg_font_courB24_mf[];
const ucg_fntpgm_uint8_t ucg_font_courB24_mn[];
const ucg_fntpgm_uint8_t ucg_font_courB24_mr[];
const ucg_fntpgm_uint8_t ucg_font_courB24_tf[];
const ucg_fntpgm_uint8_t ucg_font_courB24_tn[];
const ucg_fntpgm_uint8_t ucg_font_courB24_tr[];
const ucg_fntpgm_uint8_t ucg_font_courR08_mf[];
const ucg_fntpgm_uint8_t ucg_font_courR08_mr[];
const ucg_fntpgm_uint8_t ucg_font_courR08_tf[];
const ucg_fntpgm_uint8_t ucg_font_courR08_tr[];
const ucg_fntpgm_uint8_t ucg_font_courR10_mf[];
const ucg_fntpgm_uint8_t ucg_font_courR10_mr[];
const ucg_fntpgm_uint8_t ucg_font_courR10_tf[];
const ucg_fntpgm_uint8_t ucg_font_courR10_tr[];
const ucg_fntpgm_uint8_t ucg_font_courR12_mf[];
const ucg_fntpgm_uint8_t ucg_font_courR12_mr[];
const ucg_fntpgm_uint8_t ucg_font_courR12_tf[];
const ucg_fntpgm_uint8_t ucg_font_courR12_tr[];
const ucg_fntpgm_uint8_t ucg_font_courR14_mf[];
const ucg_fntpgm_uint8_t ucg_font_courR14_mr[];
const ucg_fntpgm_uint8_t ucg_font_courR14_tf[];
const ucg_fntpgm_uint8_t ucg_font_courR14_tr[];
const ucg_fntpgm_uint8_t ucg_font_courR18_mf[];
const ucg_fntpgm_uint8_t ucg_font_courR18_mr[];
const ucg_fntpgm_uint8_t ucg_font_courR18_tf[];
const ucg_fntpgm_uint8_t ucg_font_courR18_tr[];
const ucg_fntpgm_uint8_t ucg_font_courR24_mf[];
const ucg_fntpgm_uint8_t ucg_font_courR24_mn[];
const ucg_fntpgm_uint8_t ucg_font_courR24_mr[];
const ucg_fntpgm_uint8_t ucg_font_courR24_tf[];
const ucg_fntpgm_uint8_t ucg_font_courR24_tn[];
const ucg_fntpgm_uint8_t ucg_font_courR24_tr[];
const ucg_fntpgm_uint8_t ucg_font_cu12_67_75[];
const ucg_fntpgm_uint8_t ucg_font_cu12_75[];
const ucg_fntpgm_uint8_t ucg_font_cu12_hf[];
const ucg_fntpgm_uint8_t ucg_font_cu12_mf[];
const ucg_fntpgm_uint8_t ucg_font_cu12_tf[];
const ucg_fntpgm_uint8_t ucg_font_cursor_tf[];
const ucg_fntpgm_uint8_t ucg_font_cursor_tr[];
const ucg_fntpgm_uint8_t ucg_font_fixed_v0_hf[];
const ucg_fntpgm_uint8_t ucg_font_fixed_v0_hn[];
const ucg_fntpgm_uint8_t ucg_font_fixed_v0_hr[];
const ucg_fntpgm_uint8_t ucg_font_fixed_v0_mr[];
const ucg_fntpgm_uint8_t ucg_font_fixed_v0_tf[];
const ucg_fntpgm_uint8_t ucg_font_fixed_v0_tn[];
const ucg_fntpgm_uint8_t ucg_font_fixed_v0_tr[];
const ucg_fntpgm_uint8_t ucg_font_freedoomr10_tr[];
const ucg_fntpgm_uint8_t ucg_font_freedoomr25_tn[];
const ucg_fntpgm_uint8_t ucg_font_helvB08_hf[];
const ucg_fntpgm_uint8_t ucg_font_helvB08_hr[];
const ucg_fntpgm_uint8_t ucg_font_helvB08_tf[];
const ucg_fntpgm_uint8_t ucg_font_helvB08_tr[];
const ucg_fntpgm_uint8_t ucg_font_helvB10_hf[];
const ucg_fntpgm_uint8_t ucg_font_helvB10_hr[];
const ucg_fntpgm_uint8_t ucg_font_helvB10_tf[];
const ucg_fntpgm_uint8_t ucg_font_helvB10_tr[];
const ucg_fntpgm_uint8_t ucg_font_helvB12_hf[];
const ucg_fntpgm_uint8_t ucg_font_helvB12_hr[];
const ucg_fntpgm_uint8_t ucg_font_helvB12_tf[];
const ucg_fntpgm_uint8_t ucg_font_helvB12_tr[];
const ucg_fntpgm_uint8_t ucg_font_helvB14_hf[];
const ucg_fntpgm_uint8_t ucg_font_helvB14_hr[];
const ucg_fntpgm_uint8_t ucg_font_helvB14_tf[];
const ucg_fntpgm_uint8_t ucg_font_helvB14_tr[];
const ucg_fntpgm_uint8_t ucg_font_helvB18_hf[];
const ucg_fntpgm_uint8_t ucg_font_helvB18_hr[];
const ucg_fntpgm_uint8_t ucg_font_helvB18_tf[];
const ucg_fntpgm_uint8_t ucg_font_helvB18_tr[];
const ucg_fntpgm_uint8_t ucg_font_helvB24_hf[];
const ucg_fntpgm_uint8_t ucg_font_helvB24_hn[];
const ucg_fntpgm_uint8_t ucg_font_helvB24_hr[];
const ucg_fntpgm_uint8_t ucg_font_helvB24_tf[];
const ucg_fntpgm_uint8_t ucg_font_helvB24_tn[];
const ucg_fntpgm_uint8_t ucg_font_helvB24_tr[];
const ucg_fntpgm_uint8_t ucg_font_helvR08_hf[];
const ucg_fntpgm_uint8_t ucg_font_helvR08_hr[];
const ucg_fntpgm_uint8_t ucg_font_helvR08_tf[];
const ucg_fntpgm_uint8_t ucg_font_helvR08_tr[];
const ucg_fntpgm_uint8_t ucg_font_helvR10_hf[];
const ucg_fntpgm_uint8_t ucg_font_helvR10_hr[];
const ucg_fntpgm_uint8_t ucg_font_helvR10_tf[];
const ucg_fntpgm_uint8_t ucg_font_helvR10_tr[];
const ucg_fntpgm_uint8_t ucg_font_helvR12_hf[];
const ucg_fntpgm_uint8_t ucg_font_helvR12_hr[];
const ucg_fntpgm_uint8_t ucg_font_helvR12_tf[];
const ucg_fntpgm_uint8_t ucg_font_helvR12_tr[];
const ucg_fntpgm_uint8_t ucg_font_helvR14_hf[];
const ucg_fntpgm_uint8_t ucg_font_helvR14_hr[];
const ucg_fntpgm_uint8_t ucg_font_helvR14_tf[];
const ucg_fntpgm_uint8_t ucg_font_helvR14_tr[];
const ucg_fntpgm_uint8_t ucg_font_helvR18_hf[];
const ucg_fntpgm_uint8_t ucg_font_helvR18_hr[];
const ucg_fntpgm_uint8_t ucg_font_helvR18_tf[];
const ucg_fntpgm_uint8_t ucg_font_helvR18_tr[];
const ucg_fntpgm_uint8_t ucg_font_helvR24_hf[];
const ucg_fntpgm_uint8_t ucg_font_helvR24_hn[];
const ucg_fntpgm_uint8_t ucg_font_helvR24_hr[];
const ucg_fntpgm_uint8_t ucg_font_helvR24_tf[];
const ucg_fntpgm_uint8_t ucg_font_helvR24_tn[];
const ucg_fntpgm_uint8_t ucg_font_helvR24_tr[];
const ucg_fntpgm_uint8_t ucg_font_lucasfont_alternate_hf[];
const ucg_fntpgm_uint8_t ucg_font_lucasfont_alternate_hn[];
const ucg_fntpgm_uint8_t ucg_font_lucasfont_alternate_hr[];
const ucg_fntpgm_uint8_t ucg_font_lucasfont_alternate_tf[];
const ucg_fntpgm_uint8_t ucg_font_lucasfont_alternate_tn[];
const ucg_fntpgm_uint8_t ucg_font_lucasfont_alternate_tr[];
const ucg_fntpgm_uint8_t ucg_font_m2icon_5[];
const ucg_fntpgm_uint8_t ucg_font_m2icon_7[];
const ucg_fntpgm_uint8_t ucg_font_m2icon_9[];
const ucg_fntpgm_uint8_t ucg_font_micro_mf[];
const ucg_fntpgm_uint8_t ucg_font_micro_tf[];
const ucg_fntpgm_uint8_t ucg_font_ncenB08_hf[];
const ucg_fntpgm_uint8_t ucg_font_ncenB08_hr[];
const ucg_fntpgm_uint8_t ucg_font_ncenB08_tf[];
const ucg_fntpgm_uint8_t ucg_font_ncenB08_tr[];
const ucg_fntpgm_uint8_t ucg_font_ncenB10_hf[];
const ucg_fntpgm_uint8_t ucg_font_ncenB10_hr[];
const ucg_fntpgm_uint8_t ucg_font_ncenB10_tf[];
const ucg_fntpgm_uint8_t ucg_font_ncenB10_tr[];
const ucg_fntpgm_uint8_t ucg_font_ncenB12_hf[];
const ucg_fntpgm_uint8_t ucg_font_ncenB12_hr[];
const ucg_fntpgm_uint8_t ucg_font_ncenB12_tf[];
const ucg_fntpgm_uint8_t ucg_font_ncenB12_tr[];
const ucg_fntpgm_uint8_t ucg_font_ncenB14_hf[];
const ucg_fntpgm_uint8_t ucg_font_ncenB14_hr[];
const ucg_fntpgm_uint8_t ucg_font_ncenB14_tf[];
const ucg_fntpgm_uint8_t ucg_font_ncenB14_tr[];
const ucg_fntpgm_uint8_t ucg_font_ncenB18_hf[];
const ucg_fntpgm_uint8_t ucg_font_ncenB18_hr[];
const ucg_fntpgm_uint8_t ucg_font_ncenB18_tf[];
const ucg_fntpgm_uint8_t ucg_font_ncenB18_tr[];
const ucg_fntpgm_uint8_t ucg_font_ncenB24_hf[];
const ucg_fntpgm_uint8_t ucg_font_ncenB24_hn[];
const ucg_fntpgm_uint8_t ucg_font_ncenB24_hr[];
const ucg_fntpgm_uint8_t ucg_font_ncenB24_tf[];
const ucg_fntpgm_uint8_t ucg_font_ncenB24_tn[];
const ucg_fntpgm_uint8_t ucg_font_ncenB24_tr[];
const ucg_fntpgm_uint8_t ucg_font_ncenR08_hf[];
const ucg_fntpgm_uint8_t ucg_font_ncenR08_hr[];
const ucg_fntpgm_uint8_t ucg_font_ncenR08_tf[];
const ucg_fntpgm_uint8_t ucg_font_ncenR08_tr[];
const ucg_fntpgm_uint8_t ucg_font_ncenR10_hf[];
const ucg_fntpgm_uint8_t ucg_font_ncenR10_hr[];
const ucg_fntpgm_uint8_t ucg_font_ncenR10_tf[];
const ucg_fntpgm_uint8_t ucg_font_ncenR10_tr[];
const ucg_fntpgm_uint8_t ucg_font_ncenR12_hf[];
const ucg_fntpgm_uint8_t ucg_font_ncenR12_hr[];
const ucg_fntpgm_uint8_t ucg_font_ncenR12_tf[];
const ucg_fntpgm_uint8_t ucg_font_ncenR12_tr[];
const ucg_fntpgm_uint8_t ucg_font_ncenR14_hf[];
const ucg_fntpgm_uint8_t ucg_font_ncenR14_hr[];
const ucg_fntpgm_uint8_t ucg_font_ncenR14_tf[];
const ucg_fntpgm_uint8_t ucg_font_ncenR14_tr[];
const ucg_fntpgm_uint8_t ucg_font_ncenR18_hf[];
const ucg_fntpgm_uint8_t ucg_font_ncenR18_hr[];
const ucg_fntpgm_uint8_t ucg_font_ncenR18_tf[];
const ucg_fntpgm_uint8_t ucg_font_ncenR18_tr[];
const ucg_fntpgm_uint8_t ucg_font_ncenR24_hf[];
const ucg_fntpgm_uint8_t ucg_font_ncenR24_hn[];
const ucg_fntpgm_uint8_t ucg_font_ncenR24_hr[];
const ucg_fntpgm_uint8_t ucg_font_ncenR24_tf[];
const ucg_fntpgm_uint8_t ucg_font_ncenR24_tn[];
const ucg_fntpgm_uint8_t ucg_font_ncenR24_tr[];
const ucg_fntpgm_uint8_t ucg_font_orgv01_hf[];
const ucg_fntpgm_uint8_t ucg_font_orgv01_hn[];
const ucg_fntpgm_uint8_t ucg_font_orgv01_hr[];
const ucg_fntpgm_uint8_t ucg_font_orgv01_tf[];
const ucg_fntpgm_uint8_t ucg_font_orgv01_tn[];
const ucg_fntpgm_uint8_t ucg_font_orgv01_tr[];
const ucg_fntpgm_uint8_t ucg_font_p01type_hf[];
const ucg_fntpgm_uint8_t ucg_font_p01type_hn[];
const ucg_fntpgm_uint8_t ucg_font_p01type_hr[];
const ucg_fntpgm_uint8_t ucg_font_p01type_tf[];
const ucg_fntpgm_uint8_t ucg_font_p01type_tn[];
const ucg_fntpgm_uint8_t ucg_font_p01type_tr[];
const ucg_fntpgm_uint8_t ucg_font_pixelle_micro_hf[];
const ucg_fntpgm_uint8_t ucg_font_pixelle_micro_hn[];
const ucg_fntpgm_uint8_t ucg_font_pixelle_micro_hr[];
const ucg_fntpgm_uint8_t ucg_font_pixelle_micro_tf[];
const ucg_fntpgm_uint8_t ucg_font_pixelle_micro_tn[];
const ucg_fntpgm_uint8_t ucg_font_pixelle_micro_tr[];
const ucg_fntpgm_uint8_t ucg_font_profont10_8f[];
const ucg_fntpgm_uint8_t ucg_font_profont10_8r[];
const ucg_fntpgm_uint8_t ucg_font_profont10_mf[];
const ucg_fntpgm_uint8_t ucg_font_profont10_mr[];
const ucg_fntpgm_uint8_t ucg_font_profont11_8f[];
const ucg_fntpgm_uint8_t ucg_font_profont11_8r[];
const ucg_fntpgm_uint8_t ucg_font_profont11_mf[];
const ucg_fntpgm_uint8_t ucg_font_profont11_mr[];
const ucg_fntpgm_uint8_t ucg_font_profont12_8f[];
const ucg_fntpgm_uint8_t ucg_font_profont12_8r[];
const ucg_fntpgm_uint8_t ucg_font_profont12_mf[];
const ucg_fntpgm_uint8_t ucg_font_profont12_mr[];
const ucg_fntpgm_uint8_t ucg_font_profont15_8f[];
const ucg_fntpgm_uint8_t ucg_font_profont15_8r[];
const ucg_fntpgm_uint8_t ucg_font_profont15_mf[];
const ucg_fntpgm_uint8_t ucg_font_profont15_mr[];
const ucg_fntpgm_uint8_t ucg_font_profont17_8f[];
const ucg_fntpgm_uint8_t ucg_font_profont17_8r[];
const ucg_fntpgm_uint8_t ucg_font_profont17_mf[];
const ucg_fntpgm_uint8_t ucg_font_profont17_mr[];
const ucg_fntpgm_uint8_t ucg_font_profont22_8f[];
const ucg_fntpgm_uint8_t ucg_font_profont22_8n[];
const ucg_fntpgm_uint8_t ucg_font_profont22_8r[];
const ucg_fntpgm_uint8_t ucg_font_profont22_mf[];
const ucg_fntpgm_uint8_t ucg_font_profont22_mn[];
const ucg_fntpgm_uint8_t ucg_font_profont22_mr[];
const ucg_fntpgm_uint8_t ucg_font_profont29_8f[];
const ucg_fntpgm_uint8_t ucg_font_profont29_8n[];
const ucg_fntpgm_uint8_t ucg_font_profont29_8r[];
const ucg_fntpgm_uint8_t ucg_font_profont29_mf[];
const ucg_fntpgm_uint8_t ucg_font_profont29_mn[];
const ucg_fntpgm_uint8_t ucg_font_profont29_mr[];
const ucg_fntpgm_uint8_t ucg_font_robot_de_niro_hf[];
const ucg_fntpgm_uint8_t ucg_font_robot_de_niro_hn[];
const ucg_fntpgm_uint8_t ucg_font_robot_de_niro_hr[];
const ucg_fntpgm_uint8_t ucg_font_robot_de_niro_tf[];
const ucg_fntpgm_uint8_t ucg_font_robot_de_niro_tn[];
const ucg_fntpgm_uint8_t ucg_font_robot_de_niro_tr[];
const ucg_fntpgm_uint8_t ucg_font_symb08_tf[];
const ucg_fntpgm_uint8_t ucg_font_symb08_tr[];
const ucg_fntpgm_uint8_t ucg_font_symb10_tf[];
const ucg_fntpgm_uint8_t ucg_font_symb10_tr[];
const ucg_fntpgm_uint8_t ucg_font_symb12_tf[];
const ucg_fntpgm_uint8_t ucg_font_symb12_tr[];
const ucg_fntpgm_uint8_t ucg_font_symb14_tf[];
const ucg_fntpgm_uint8_t ucg_font_symb14_tr[];
const ucg_fntpgm_uint8_t ucg_font_symb18_tf[];
const ucg_fntpgm_uint8_t ucg_font_symb18_tr[];
const ucg_fntpgm_uint8_t ucg_font_symb24_tf[];
const ucg_fntpgm_uint8_t ucg_font_symb24_tr[];
const ucg_fntpgm_uint8_t ucg_font_timB08_hf[];
const ucg_fntpgm_uint8_t ucg_font_timB08_hr[];
const ucg_fntpgm_uint8_t ucg_font_timB08_tf[];
const ucg_fntpgm_uint8_t ucg_font_timB08_tr[];
const ucg_fntpgm_uint8_t ucg_font_timB10_hf[];
const ucg_fntpgm_uint8_t ucg_font_timB10_hr[];
const ucg_fntpgm_uint8_t ucg_font_timB10_tf[];
const ucg_fntpgm_uint8_t ucg_font_timB10_tr[];
const ucg_fntpgm_uint8_t ucg_font_timB12_hf[];
const ucg_fntpgm_uint8_t ucg_font_timB12_hr[];
const ucg_fntpgm_uint8_t ucg_font_timB12_tf[];
const ucg_fntpgm_uint8_t ucg_font_timB12_tr[];
const ucg_fntpgm_uint8_t ucg_font_timB14_hf[];
const ucg_fntpgm_uint8_t ucg_font_timB14_hr[];
const ucg_fntpgm_uint8_t ucg_font_timB14_tf[];
const ucg_fntpgm_uint8_t ucg_font_timB14_tr[];
const ucg_fntpgm_uint8_t ucg_font_timB18_hf[];
const ucg_fntpgm_uint8_t ucg_font_timB18_hr[];
const ucg_fntpgm_uint8_t ucg_font_timB18_tf[];
const ucg_fntpgm_uint8_t ucg_font_timB18_tr[];
const ucg_fntpgm_uint8_t ucg_font_timB24_hf[];
const ucg_fntpgm_uint8_t ucg_font_timB24_hn[];
const ucg_fntpgm_uint8_t ucg_font_timB24_hr[];
const ucg_fntpgm_uint8_t ucg_font_timB24_tf[];
const ucg_fntpgm_uint8_t ucg_font_timB24_tn[];
const ucg_fntpgm_uint8_t ucg_font_timB24_tr[];
const ucg_fntpgm_uint8_t ucg_font_timR08_hf[];
const ucg_fntpgm_uint8_t ucg_font_timR08_hr[];
const ucg_fntpgm_uint8_t ucg_font_timR08_tf[];
const ucg_fntpgm_uint8_t ucg_font_timR08_tr[];
const ucg_fntpgm_uint8_t ucg_font_timR10_hf[];
const ucg_fntpgm_uint8_t ucg_font_timR10_hr[];
const ucg_fntpgm_uint8_t ucg_font_timR10_tf[];
const ucg_fntpgm_uint8_t ucg_font_timR10_tr[];
const ucg_fntpgm_uint8_t ucg_font_timR12_hf[];
const ucg_fntpgm_uint8_t ucg_font_timR12_hr[];
const ucg_fntpgm_uint8_t ucg_font_timR12_tf[];
const ucg_fntpgm_uint8_t ucg_font_timR12_tr[];
const ucg_fntpgm_uint8_t ucg_font_timR14_hf[];
const ucg_fntpgm_uint8_t ucg_font_timR14_hr[];
const ucg_fntpgm_uint8_t ucg_font_timR14_tf[];
const ucg_fntpgm_uint8_t ucg_font_timR14_tr[];
const ucg_fntpgm_uint8_t ucg_font_timR18_hf[];
const ucg_fntpgm_uint8_t ucg_font_timR18_hr[];
const ucg_fntpgm_uint8_t ucg_font_timR18_tf[];
const ucg_fntpgm_uint8_t ucg_font_timR18_tr[];
const ucg_fntpgm_uint8_t ucg_font_timR24_hf[];
const ucg_fntpgm_uint8_t ucg_font_timR24_hn[];
const ucg_fntpgm_uint8_t ucg_font_timR24_hr[];
const ucg_fntpgm_uint8_t ucg_font_timR24_tf[];
const ucg_fntpgm_uint8_t ucg_font_timR24_tn[];
const ucg_fntpgm_uint8_t ucg_font_timR24_tr[];
const ucg_fntpgm_uint8_t ucg_font_tpssb_hf[];
const ucg_fntpgm_uint8_t ucg_font_tpssb_hn[];
const ucg_fntpgm_uint8_t ucg_font_tpssb_hr[];
const ucg_fntpgm_uint8_t ucg_font_tpssb_tf[];
const ucg_fntpgm_uint8_t ucg_font_tpssb_tn[];
const ucg_fntpgm_uint8_t ucg_font_tpssb_tr[];
const ucg_fntpgm_uint8_t ucg_font_tpss_hf[];
const ucg_fntpgm_uint8_t ucg_font_tpss_hn[];
const ucg_fntpgm_uint8_t ucg_font_tpss_hr[];
const ucg_fntpgm_uint8_t ucg_font_tpss_tf[];
const ucg_fntpgm_uint8_t ucg_font_tpss_tn[];
const ucg_fntpgm_uint8_t ucg_font_tpss_tr[];
const ucg_fntpgm_uint8_t ucg_font_trixel_square_hf[];
const ucg_fntpgm_uint8_t ucg_font_trixel_square_hn[];
const ucg_fntpgm_uint8_t ucg_font_trixel_square_hr[];
const ucg_fntpgm_uint8_t ucg_font_trixel_square_tf[];
const ucg_fntpgm_uint8_t ucg_font_trixel_square_tn[];
const ucg_fntpgm_uint8_t ucg_font_trixel_square_tr[];
const ucg_fntpgm_uint8_t ucg_font_u8glib_4_hf[];
const ucg_fntpgm_uint8_t ucg_font_u8glib_4_hr[];
const ucg_fntpgm_uint8_t ucg_font_u8glib_4_tf[];
const ucg_fntpgm_uint8_t ucg_font_u8glib_4_tr[];
const ucg_fntpgm_uint8_t ucg_font_unifont_0_8[];
const ucg_fntpgm_uint8_t ucg_font_unifont_12_13[];
const ucg_fntpgm_uint8_t ucg_font_unifont_18_19[];
const ucg_fntpgm_uint8_t ucg_font_unifont_2_3[];
const ucg_fntpgm_uint8_t ucg_font_unifont_4_5[];
const ucg_fntpgm_uint8_t ucg_font_unifont_67_75[];
const ucg_fntpgm_uint8_t ucg_font_unifont_72_73[];
const ucg_fntpgm_uint8_t ucg_font_unifont_78_79[];
const ucg_fntpgm_uint8_t ucg_font_unifont_8_9[];
const ucg_fntpgm_uint8_t ucg_font_unifont_mf[];
const ucg_fntpgm_uint8_t ucg_font_unifont_mr[];
const ucg_fntpgm_uint8_t ucg_font_unifont_tf[];
const ucg_fntpgm_uint8_t ucg_font_unifont_tr[];
const ucg_fntpgm_uint8_t ucg_font_fub11_hf[];
const ucg_fntpgm_uint8_t ucg_font_fub11_hn[];
const ucg_fntpgm_uint8_t ucg_font_fub11_hr[];
const ucg_fntpgm_uint8_t ucg_font_fub11_tf[];
const ucg_fntpgm_uint8_t ucg_font_fub11_tn[];
const ucg_fntpgm_uint8_t ucg_font_fub11_tr[];
const ucg_fntpgm_uint8_t ucg_font_fub14_hf[];
const ucg_fntpgm_uint8_t ucg_font_fub14_hn[];
const ucg_fntpgm_uint8_t ucg_font_fub14_hr[];
const ucg_fntpgm_uint8_t ucg_font_fub14_tf[];
const ucg_fntpgm_uint8_t ucg_font_fub14_tn[];
const ucg_fntpgm_uint8_t ucg_font_fub14_tr[];
const ucg_fntpgm_uint8_t ucg_font_fub17_hf[];
const ucg_fntpgm_uint8_t ucg_font_fub17_hn[];
const ucg_fntpgm_uint8_t ucg_font_fub17_hr[];
const ucg_fntpgm_uint8_t ucg_font_fub17_tf[];
const ucg_fntpgm_uint8_t ucg_font_fub17_tn[];
const ucg_fntpgm_uint8_t ucg_font_fub17_tr[];
const ucg_fntpgm_uint8_t ucg_font_fub20_hf[];
const ucg_fntpgm_uint8_t ucg_font_fub20_hn[];
const ucg_fntpgm_uint8_t ucg_font_fub20_hr[];
const ucg_fntpgm_uint8_t ucg_font_fub20_tf[];
const ucg_fntpgm_uint8_t ucg_font_fub20_tn[];
const ucg_fntpgm_uint8_t ucg_font_fub20_tr[];
const ucg_fntpgm_uint8_t ucg_font_fub25_hf[];
const ucg_fntpgm_uint8_t ucg_font_fub25_hn[];
const ucg_fntpgm_uint8_t ucg_font_fub25_hr[];
const ucg_fntpgm_uint8_t ucg_font_fub25_tf[];
const ucg_fntpgm_uint8_t ucg_font_fub25_tn[];
const ucg_fntpgm_uint8_t ucg_font_fub25_tr[];
const ucg_fntpgm_uint8_t ucg_font_fub30_hf[];
const ucg_fntpgm_uint8_t ucg_font_fub30_hn[];
const ucg_fntpgm_uint8_t ucg_font_fub30_hr[];
const ucg_fntpgm_uint8_t ucg_font_fub30_tf[];
const ucg_fntpgm_uint8_t ucg_font_fub30_tn[];
const ucg_fntpgm_uint8_t ucg_font_fub30_tr[];
const ucg_fntpgm_uint8_t ucg_font_fub35_hf[];
const ucg_fntpgm_uint8_t ucg_font_fub35_hn[];
const ucg_fntpgm_uint8_t ucg_font_fub35_hr[];
const ucg_fntpgm_uint8_t ucg_font_fub35_tf[];
const ucg_fntpgm_uint8_t ucg_font_fub35_tn[];
const ucg_fntpgm_uint8_t ucg_font_fub35_tr[];
const ucg_fntpgm_uint8_t ucg_font_fub42_hf[];
const ucg_fntpgm_uint8_t ucg_font_fub42_hn[];
const ucg_fntpgm_uint8_t ucg_font_fub42_hr[];
const ucg_fntpgm_uint8_t ucg_font_fub42_tf[];
const ucg_fntpgm_uint8_t ucg_font_fub42_tn[];
const ucg_fntpgm_uint8_t ucg_font_fub42_tr[];
const ucg_fntpgm_uint8_t ucg_font_fub49_hn[];
const ucg_fntpgm_uint8_t ucg_font_fub49_tn[];
const ucg_fntpgm_uint8_t ucg_font_fur11_hf[];
const ucg_fntpgm_uint8_t ucg_font_fur11_hn[];
const ucg_fntpgm_uint8_t ucg_font_fur11_hr[];
const ucg_fntpgm_uint8_t ucg_font_fur11_tf[];
const ucg_fntpgm_uint8_t ucg_font_fur11_tn[];
const ucg_fntpgm_uint8_t ucg_font_fur11_tr[];
const ucg_fntpgm_uint8_t ucg_font_fur14_hf[];
const ucg_fntpgm_uint8_t ucg_font_fur14_hn[];
const ucg_fntpgm_uint8_t ucg_font_fur14_hr[];
const ucg_fntpgm_uint8_t ucg_font_fur14_tf[];
const ucg_fntpgm_uint8_t ucg_font_fur14_tn[];
const ucg_fntpgm_uint8_t ucg_font_fur14_tr[];
const ucg_fntpgm_uint8_t ucg_font_fur17_hf[];
const ucg_fntpgm_uint8_t ucg_font_fur17_hn[];
const ucg_fntpgm_uint8_t ucg_font_fur17_hr[];
const ucg_fntpgm_uint8_t ucg_font_fur17_tf[];
const ucg_fntpgm_uint8_t ucg_font_fur17_tn[];
const ucg_fntpgm_uint8_t ucg_font_fur17_tr[];
const ucg_fntpgm_uint8_t ucg_font_fur20_hf[];
const ucg_fntpgm_uint8_t ucg_font_fur20_hn[];
const ucg_fntpgm_uint8_t ucg_font_fur20_hr[];
const ucg_fntpgm_uint8_t ucg_font_fur20_tf[];
const ucg_fntpgm_uint8_t ucg_font_fur20_tn[];
const ucg_fntpgm_uint8_t ucg_font_fur20_tr[];
const ucg_fntpgm_uint8_t ucg_font_fur25_hf[];
const ucg_fntpgm_uint8_t ucg_font_fur25_hn[];
const ucg_fntpgm_uint8_t ucg_font_fur25_hr[];
const ucg_fntpgm_uint8_t ucg_font_fur25_tf[];
const ucg_fntpgm_uint8_t ucg_font_fur25_tn[];
const ucg_fntpgm_uint8_t ucg_font_fur25_tr[];
const ucg_fntpgm_uint8_t ucg_font_fur30_hf[];
const ucg_fntpgm_uint8_t ucg_font_fur30_hn[];
const ucg_fntpgm_uint8_t ucg_font_fur30_hr[];
const ucg_fntpgm_uint8_t ucg_font_fur30_tf[];
const ucg_fntpgm_uint8_t ucg_font_fur30_tn[];
const ucg_fntpgm_uint8_t ucg_font_fur30_tr[];
const ucg_fntpgm_uint8_t ucg_font_fur35_hf[];
const ucg_fntpgm_uint8_t ucg_font_fur35_hn[];
const ucg_fntpgm_uint8_t ucg_font_fur35_hr[];
const ucg_fntpgm_uint8_t ucg_font_fur35_tf[];
const ucg_fntpgm_uint8_t ucg_font_fur35_tn[];
const ucg_fntpgm_uint8_t ucg_font_fur35_tr[];
const ucg_fntpgm_uint8_t ucg_font_fur42_hf[];
const ucg_fntpgm_uint8_t ucg_font_fur42_hn[];
const ucg_fntpgm_uint8_t ucg_font_fur42_hr[];
const ucg_fntpgm_uint8_t ucg_font_fur42_tf[];
const ucg_fntpgm_uint8_t ucg_font_fur42_tn[];
const ucg_fntpgm_uint8_t ucg_font_fur42_tr[];
const ucg_fntpgm_uint8_t ucg_font_fur49_hn[];
const ucg_fntpgm_uint8_t ucg_font_fur49_tn[];
const ucg_fntpgm_uint8_t ucg_font_fur49_tr[];
const ucg_fntpgm_uint8_t ucg_font_inb16_mf[];
const ucg_fntpgm_uint8_t ucg_font_inb16_mn[];
const ucg_fntpgm_uint8_t ucg_font_inb16_mr[];
const ucg_fntpgm_uint8_t ucg_font_inb16_tf[];
const ucg_fntpgm_uint8_t ucg_font_inb16_tn[];
const ucg_fntpgm_uint8_t ucg_font_inb16_tr[];
const ucg_fntpgm_uint8_t ucg_font_inb19_mf[];
const ucg_fntpgm_uint8_t ucg_font_inb19_mn[];
const ucg_fntpgm_uint8_t ucg_font_inb19_mr[];
const ucg_fntpgm_uint8_t ucg_font_inb19_tf[];
const ucg_fntpgm_uint8_t ucg_font_inb19_tn[];
const ucg_fntpgm_uint8_t ucg_font_inb19_tr[];
const ucg_fntpgm_uint8_t ucg_font_inb21_mf[];
const ucg_fntpgm_uint8_t ucg_font_inb21_mn[];
const ucg_fntpgm_uint8_t ucg_font_inb21_mr[];
const ucg_fntpgm_uint8_t ucg_font_inb21_tf[];
const ucg_fntpgm_uint8_t ucg_font_inb21_tn[];
const ucg_fntpgm_uint8_t ucg_font_inb21_tr[];
const ucg_fntpgm_uint8_t ucg_font_inb24_mf[];
const ucg_fntpgm_uint8_t ucg_font_inb24_mn[];
const ucg_fntpgm_uint8_t ucg_font_inb24_mr[];
const ucg_fntpgm_uint8_t ucg_font_inb24_tf[];
const ucg_fntpgm_uint8_t ucg_font_inb24_tn[];
const ucg_fntpgm_uint8_t ucg_font_inb24_tr[];
const ucg_fntpgm_uint8_t ucg_font_inb27_mf[];
const ucg_fntpgm_uint8_t ucg_font_inb27_mn[];
const ucg_fntpgm_uint8_t ucg_font_inb27_mr[];
const ucg_fntpgm_uint8_t ucg_font_inb27_tf[];
const ucg_fntpgm_uint8_t ucg_font_inb27_tn[];
const ucg_fntpgm_uint8_t ucg_font_inb27_tr[];
const ucg_fntpgm_uint8_t ucg_font_inb30_mf[];
const ucg_fntpgm_uint8_t ucg_font_inb30_mn[];
const ucg_fntpgm_uint8_t ucg_font_inb30_mr[];
const ucg_fntpgm_uint8_t ucg_font_inb30_tf[];
const ucg_fntpgm_uint8_t ucg_font_inb30_tn[];
const ucg_fntpgm_uint8_t ucg_font_inb30_tr[];
const ucg_fntpgm_uint8_t ucg_font_inb33_mf[];
const ucg_fntpgm_uint8_t ucg_font_inb33_mn[];
const ucg_fntpgm_uint8_t ucg_font_inb33_mr[];
const ucg_fntpgm_uint8_t ucg_font_inb33_tf[];
const ucg_fntpgm_uint8_t ucg_font_inb33_tn[];
const ucg_fntpgm_uint8_t ucg_font_inb33_tr[];
const ucg_fntpgm_uint8_t ucg_font_inb38_mf[];
const ucg_fntpgm_uint8_t ucg_font_inb38_mn[];
const ucg_fntpgm_uint8_t ucg_font_inb38_mr[];
const ucg_fntpgm_uint8_t ucg_font_inb38_tf[];
const ucg_fntpgm_uint8_t ucg_font_inb38_tn[];
const ucg_fntpgm_uint8_t ucg_font_inb38_tr[];
const ucg_fntpgm_uint8_t ucg_font_inb42_mf[];
const ucg_fntpgm_uint8_t ucg_font_inb42_mn[];
const ucg_fntpgm_uint8_t ucg_font_inb42_mr[];
const ucg_fntpgm_uint8_t ucg_font_inb42_tf[];
const ucg_fntpgm_uint8_t ucg_font_inb42_tn[];
const ucg_fntpgm_uint8_t ucg_font_inb42_tr[];
const ucg_fntpgm_uint8_t ucg_font_inb46_mf[];
const ucg_fntpgm_uint8_t ucg_font_inb46_mn[];
const ucg_fntpgm_uint8_t ucg_font_inb46_mr[];
const ucg_fntpgm_uint8_t ucg_font_inb46_tf[];
const ucg_fntpgm_uint8_t ucg_font_inb46_tn[];
const ucg_fntpgm_uint8_t ucg_font_inb46_tr[];
const ucg_fntpgm_uint8_t ucg_font_inb49_mf[];
const ucg_fntpgm_uint8_t ucg_font_inb49_mn[];
const ucg_fntpgm_uint8_t ucg_font_inb49_mr[];
const ucg_fntpgm_uint8_t ucg_font_inb49_tf[];
const ucg_fntpgm_uint8_t ucg_font_inb49_tn[];
const ucg_fntpgm_uint8_t ucg_font_inb49_tr[];
const ucg_fntpgm_uint8_t ucg_font_inb53_mf[];
const ucg_fntpgm_uint8_t ucg_font_inb53_mn[];
const ucg_fntpgm_uint8_t ucg_font_inb53_mr[];
const ucg_fntpgm_uint8_t ucg_font_inb53_tf[];
const ucg_fntpgm_uint8_t ucg_font_inb53_tn[];
const ucg_fntpgm_uint8_t ucg_font_inb53_tr[];
const ucg_fntpgm_uint8_t ucg_font_inb57_mf[];
const ucg_fntpgm_uint8_t ucg_font_inb57_mn[];
const ucg_fntpgm_uint8_t ucg_font_inb57_mr[];
const ucg_fntpgm_uint8_t ucg_font_inb57_tf[];
const ucg_fntpgm_uint8_t ucg_font_inb57_tn[];
const ucg_fntpgm_uint8_t ucg_font_inb57_tr[];
const ucg_fntpgm_uint8_t ucg_font_inb63_mn[];
const ucg_fntpgm_uint8_t ucg_font_inb63_tf[];
const ucg_fntpgm_uint8_t ucg_font_inb63_tn[];
const ucg_fntpgm_uint8_t ucg_font_inb63_tr[];
const ucg_fntpgm_uint8_t ucg_font_inr16_mf[];
const ucg_fntpgm_uint8_t ucg_font_inr16_mn[];
const ucg_fntpgm_uint8_t ucg_font_inr16_mr[];
const ucg_fntpgm_uint8_t ucg_font_inr16_tf[];
const ucg_fntpgm_uint8_t ucg_font_inr16_tn[];
const ucg_fntpgm_uint8_t ucg_font_inr16_tr[];
const ucg_fntpgm_uint8_t ucg_font_inr19_mf[];
const ucg_fntpgm_uint8_t ucg_font_inr19_mn[];
const ucg_fntpgm_uint8_t ucg_font_inr19_mr[];
const ucg_fntpgm_uint8_t ucg_font_inr19_tf[];
const ucg_fntpgm_uint8_t ucg_font_inr19_tn[];
const ucg_fntpgm_uint8_t ucg_font_inr19_tr[];
const ucg_fntpgm_uint8_t ucg_font_inr21_mf[];
const ucg_fntpgm_uint8_t ucg_font_inr21_mn[];
const ucg_fntpgm_uint8_t ucg_font_inr21_mr[];
const ucg_fntpgm_uint8_t ucg_font_inr21_tf[];
const ucg_fntpgm_uint8_t ucg_font_inr21_tn[];
const ucg_fntpgm_uint8_t ucg_font_inr21_tr[];
const ucg_fntpgm_uint8_t ucg_font_inr24_mf[];
const ucg_fntpgm_uint8_t ucg_font_inr24_mn[];
const ucg_fntpgm_uint8_t ucg_font_inr24_mr[];
const ucg_fntpgm_uint8_t ucg_font_inr24_tf[];
const ucg_fntpgm_uint8_t ucg_font_inr24_tn[];
const ucg_fntpgm_uint8_t ucg_font_inr24_tr[];
const ucg_fntpgm_uint8_t ucg_font_inr27_mf[];
const ucg_fntpgm_uint8_t ucg_font_inr27_mn[];
const ucg_fntpgm_uint8_t ucg_font_inr27_mr[];
const ucg_fntpgm_uint8_t ucg_font_inr27_tf[];
const ucg_fntpgm_uint8_t ucg_font_inr27_tn[];
const ucg_fntpgm_uint8_t ucg_font_inr27_tr[];
const ucg_fntpgm_uint8_t ucg_font_inr30_mf[];
const ucg_fntpgm_uint8_t ucg_font_inr30_mn[];
const ucg_fntpgm_uint8_t ucg_font_inr30_mr[];
const ucg_fntpgm_uint8_t ucg_font_inr30_tf[];
const ucg_fntpgm_uint8_t ucg_font_inr30_tn[];
const ucg_fntpgm_uint8_t ucg_font_inr30_tr[];
const ucg_fntpgm_uint8_t ucg_font_inr33_mf[];
const ucg_fntpgm_uint8_t ucg_font_inr33_mn[];
const ucg_fntpgm_uint8_t ucg_font_inr33_mr[];
const ucg_fntpgm_uint8_t ucg_font_inr33_tf[];
const ucg_fntpgm_uint8_t ucg_font_inr33_tn[];
const ucg_fntpgm_uint8_t ucg_font_inr33_tr[];
const ucg_fntpgm_uint8_t ucg_font_inr38_mf[];
const ucg_fntpgm_uint8_t ucg_font_inr38_mn[];
const ucg_fntpgm_uint8_t ucg_font_inr38_mr[];
const ucg_fntpgm_uint8_t ucg_font_inr38_tf[];
const ucg_fntpgm_uint8_t ucg_font_inr38_tn[];
const ucg_fntpgm_uint8_t ucg_font_inr38_tr[];
const ucg_fntpgm_uint8_t ucg_font_inr42_mf[];
const ucg_fntpgm_uint8_t ucg_font_inr42_mn[];
const ucg_fntpgm_uint8_t ucg_font_inr42_mr[];
const ucg_fntpgm_uint8_t ucg_font_inr42_tf[];
const ucg_fntpgm_uint8_t ucg_font_inr42_tn[];
const ucg_fntpgm_uint8_t ucg_font_inr42_tr[];
const ucg_fntpgm_uint8_t ucg_font_inr46_mf[];
const ucg_fntpgm_uint8_t ucg_font_inr46_mn[];
const ucg_fntpgm_uint8_t ucg_font_inr46_mr[];
const ucg_fntpgm_uint8_t ucg_font_inr46_tf[];
const ucg_fntpgm_uint8_t ucg_font_inr46_tn[];
const ucg_fntpgm_uint8_t ucg_font_inr46_tr[];
const ucg_fntpgm_uint8_t ucg_font_inr49_mf[];
const ucg_fntpgm_uint8_t ucg_font_inr49_mn[];
const ucg_fntpgm_uint8_t ucg_font_inr49_mr[];
const ucg_fntpgm_uint8_t ucg_font_inr49_tf[];
const ucg_fntpgm_uint8_t ucg_font_inr49_tn[];
const ucg_fntpgm_uint8_t ucg_font_inr49_tr[];
const ucg_fntpgm_uint8_t ucg_font_inr53_mf[];
const ucg_fntpgm_uint8_t ucg_font_inr53_mn[];
const ucg_fntpgm_uint8_t ucg_font_inr53_mr[];
const ucg_fntpgm_uint8_t ucg_font_inr53_tf[];
const ucg_fntpgm_uint8_t ucg_font_inr53_tn[];
const ucg_fntpgm_uint8_t ucg_font_inr53_tr[];
const ucg_fntpgm_uint8_t ucg_font_inr57_mn[];
const ucg_fntpgm_uint8_t ucg_font_inr57_tf[];
const ucg_fntpgm_uint8_t ucg_font_inr57_tn[];
const ucg_fntpgm_uint8_t ucg_font_inr57_tr[];
const ucg_fntpgm_uint8_t ucg_font_inr62_mn[];
const ucg_fntpgm_uint8_t ucg_font_inr62_tn[];
const ucg_fntpgm_uint8_t ucg_font_inr62_tr[];
const ucg_fntpgm_uint8_t ucg_font_logisoso16_hf[];
const ucg_fntpgm_uint8_t ucg_font_logisoso16_hn[];
const ucg_fntpgm_uint8_t ucg_font_logisoso16_hr[];
const ucg_fntpgm_uint8_t ucg_font_logisoso16_tf[];
const ucg_fntpgm_uint8_t ucg_font_logisoso16_tn[];
const ucg_fntpgm_uint8_t ucg_font_logisoso16_tr[];
const ucg_fntpgm_uint8_t ucg_font_logisoso18_hf[];
const ucg_fntpgm_uint8_t ucg_font_logisoso18_hn[];
const ucg_fntpgm_uint8_t ucg_font_logisoso18_hr[];
const ucg_fntpgm_uint8_t ucg_font_logisoso18_tf[];
const ucg_fntpgm_uint8_t ucg_font_logisoso18_tn[];
const ucg_fntpgm_uint8_t ucg_font_logisoso18_tr[];
const ucg_fntpgm_uint8_t ucg_font_logisoso20_hf[];
const ucg_fntpgm_uint8_t ucg_font_logisoso20_hn[];
const ucg_fntpgm_uint8_t ucg_font_logisoso20_hr[];
const ucg_fntpgm_uint8_t ucg_font_logisoso20_tf[];
const ucg_fntpgm_uint8_t ucg_font_logisoso20_tn[];
const ucg_fntpgm_uint8_t ucg_font_logisoso20_tr[];
const ucg_fntpgm_uint8_t ucg_font_logisoso22_hf[];
const ucg_fntpgm_uint8_t ucg_font_logisoso22_hn[];
const ucg_fntpgm_uint8_t ucg_font_logisoso22_hr[];
const ucg_fntpgm_uint8_t ucg_font_logisoso22_tf[];
const ucg_fntpgm_uint8_t ucg_font_logisoso22_tn[];
const ucg_fntpgm_uint8_t ucg_font_logisoso22_tr[];
const ucg_fntpgm_uint8_t ucg_font_logisoso24_hf[];
const ucg_fntpgm_uint8_t ucg_font_logisoso24_hn[];
const ucg_fntpgm_uint8_t ucg_font_logisoso24_hr[];
const ucg_fntpgm_uint8_t ucg_font_logisoso24_tf[];
const ucg_fntpgm_uint8_t ucg_font_logisoso24_tn[];
const ucg_fntpgm_uint8_t ucg_font_logisoso24_tr[];
const ucg_fntpgm_uint8_t ucg_font_logisoso26_hf[];
const ucg_fntpgm_uint8_t ucg_font_logisoso26_hn[];
const ucg_fntpgm_uint8_t ucg_font_logisoso26_hr[];
const ucg_fntpgm_uint8_t ucg_font_logisoso26_tf[];
const ucg_fntpgm_uint8_t ucg_font_logisoso26_tn[];
const ucg_fntpgm_uint8_t ucg_font_logisoso26_tr[];
const ucg_fntpgm_uint8_t ucg_font_logisoso28_hf[];
const ucg_fntpgm_uint8_t ucg_font_logisoso28_hn[];
const ucg_fntpgm_uint8_t ucg_font_logisoso28_hr[];
const ucg_fntpgm_uint8_t ucg_font_logisoso28_tf[];
const ucg_fntpgm_uint8_t ucg_font_logisoso28_tn[];
const ucg_fntpgm_uint8_t ucg_font_logisoso28_tr[];
const ucg_fntpgm_uint8_t ucg_font_logisoso30_tf[];
const ucg_fntpgm_uint8_t ucg_font_logisoso30_tn[];
const ucg_fntpgm_uint8_t ucg_font_logisoso30_tr[];
const ucg_fntpgm_uint8_t ucg_font_logisoso32_tf[];
const ucg_fntpgm_uint8_t ucg_font_logisoso32_tn[];
const ucg_fntpgm_uint8_t ucg_font_logisoso32_tr[];
const ucg_fntpgm_uint8_t ucg_font_logisoso34_tf[];
const ucg_fntpgm_uint8_t ucg_font_logisoso34_tn[];
const ucg_fntpgm_uint8_t ucg_font_logisoso34_tr[];
const ucg_fntpgm_uint8_t ucg_font_logisoso38_tf[];
const ucg_fntpgm_uint8_t ucg_font_logisoso38_tn[];
const ucg_fntpgm_uint8_t ucg_font_logisoso38_tr[];
const ucg_fntpgm_uint8_t ucg_font_logisoso42_tf[];
const ucg_fntpgm_uint8_t ucg_font_logisoso42_tn[];
const ucg_fntpgm_uint8_t ucg_font_logisoso42_tr[];
const ucg_fntpgm_uint8_t ucg_font_logisoso46_tf[];
const ucg_fntpgm_uint8_t ucg_font_logisoso46_tn[];
const ucg_fntpgm_uint8_t ucg_font_logisoso46_tr[];
const ucg_fntpgm_uint8_t ucg_font_logisoso50_tf[];
const ucg_fntpgm_uint8_t ucg_font_logisoso50_tn[];
const ucg_fntpgm_uint8_t ucg_font_logisoso50_tr[];
const ucg_fntpgm_uint8_t ucg_font_logisoso54_tf[];
const ucg_fntpgm_uint8_t ucg_font_logisoso54_tn[];
const ucg_fntpgm_uint8_t ucg_font_logisoso54_tr[];
const ucg_fntpgm_uint8_t ucg_font_logisoso58_tf[];
const ucg_fntpgm_uint8_t ucg_font_logisoso58_tn[];
const ucg_fntpgm_uint8_t ucg_font_logisoso58_tr[];
const ucg_fntpgm_uint8_t ucg_font_logisoso62_tn[];
const ucg_fntpgm_uint8_t ucg_font_osb18_hf[];
const ucg_fntpgm_uint8_t ucg_font_osb18_hn[];
const ucg_fntpgm_uint8_t ucg_font_osb18_hr[];
const ucg_fntpgm_uint8_t ucg_font_osb18_tf[];
const ucg_fntpgm_uint8_t ucg_font_osb18_tn[];
const ucg_fntpgm_uint8_t ucg_font_osb18_tr[];
const ucg_fntpgm_uint8_t ucg_font_osb21_hf[];
const ucg_fntpgm_uint8_t ucg_font_osb21_hn[];
const ucg_fntpgm_uint8_t ucg_font_osb21_hr[];
const ucg_fntpgm_uint8_t ucg_font_osb21_tf[];
const ucg_fntpgm_uint8_t ucg_font_osb21_tn[];
const ucg_fntpgm_uint8_t ucg_font_osb21_tr[];
const ucg_fntpgm_uint8_t ucg_font_osb26_hf[];
const ucg_fntpgm_uint8_t ucg_font_osb26_hn[];
const ucg_fntpgm_uint8_t ucg_font_osb26_hr[];
const ucg_fntpgm_uint8_t ucg_font_osb26_tf[];
const ucg_fntpgm_uint8_t ucg_font_osb26_tn[];
const ucg_fntpgm_uint8_t ucg_font_osb26_tr[];
const ucg_fntpgm_uint8_t ucg_font_osb29_hf[];
const ucg_fntpgm_uint8_t ucg_font_osb29_hn[];
const ucg_fntpgm_uint8_t ucg_font_osb29_hr[];
const ucg_fntpgm_uint8_t ucg_font_osb29_tf[];
const ucg_fntpgm_uint8_t ucg_font_osb29_tn[];
const ucg_fntpgm_uint8_t ucg_font_osb29_tr[];
const ucg_fntpgm_uint8_t ucg_font_osb35_hf[];
const ucg_fntpgm_uint8_t ucg_font_osb35_hn[];
const ucg_fntpgm_uint8_t ucg_font_osb35_hr[];
const ucg_fntpgm_uint8_t ucg_font_osb35_tf[];
const ucg_fntpgm_uint8_t ucg_font_osb35_tn[];
const ucg_fntpgm_uint8_t ucg_font_osb35_tr[];
const ucg_fntpgm_uint8_t ucg_font_osb41_hf[];
const ucg_fntpgm_uint8_t ucg_font_osb41_hn[];
const ucg_fntpgm_uint8_t ucg_font_osb41_hr[];
const ucg_fntpgm_uint8_t ucg_font_osb41_tf[];
const ucg_fntpgm_uint8_t ucg_font_osb41_tn[];
const ucg_fntpgm_uint8_t ucg_font_osb41_tr[];
const ucg_fntpgm_uint8_t ucg_font_osr18_hf[];
const ucg_fntpgm_uint8_t ucg_font_osr18_hn[];
const ucg_fntpgm_uint8_t ucg_font_osr18_hr[];
const ucg_fntpgm_uint8_t ucg_font_osr18_tf[];
const ucg_fntpgm_uint8_t ucg_font_osr18_tn[];
const ucg_fntpgm_uint8_t ucg_font_osr18_tr[];
const ucg_fntpgm_uint8_t ucg_font_osr21_hf[];
const ucg_fntpgm_uint8_t ucg_font_osr21_hn[];
const ucg_fntpgm_uint8_t ucg_font_osr21_hr[];
const ucg_fntpgm_uint8_t ucg_font_osr21_tf[];
const ucg_fntpgm_uint8_t ucg_font_osr21_tn[];
const ucg_fntpgm_uint8_t ucg_font_osr21_tr[];
const ucg_fntpgm_uint8_t ucg_font_osr26_hf[];
const ucg_fntpgm_uint8_t ucg_font_osr26_hn[];
const ucg_fntpgm_uint8_t ucg_font_osr26_hr[];
const ucg_fntpgm_uint8_t ucg_font_osr26_tf[];
const ucg_fntpgm_uint8_t ucg_font_osr26_tn[];
const ucg_fntpgm_uint8_t ucg_font_osr26_tr[];
const ucg_fntpgm_uint8_t ucg_font_osr29_hf[];
const ucg_fntpgm_uint8_t ucg_font_osr29_hn[];
const ucg_fntpgm_uint8_t ucg_font_osr29_hr[];
const ucg_fntpgm_uint8_t ucg_font_osr29_tf[];
const ucg_fntpgm_uint8_t ucg_font_osr29_tn[];
const ucg_fntpgm_uint8_t ucg_font_osr29_tr[];
const ucg_fntpgm_uint8_t ucg_font_osr35_hf[];
const ucg_fntpgm_uint8_t ucg_font_osr35_hn[];
const ucg_fntpgm_uint8_t ucg_font_osr35_hr[];
const ucg_fntpgm_uint8_t ucg_font_osr35_tf[];
const ucg_fntpgm_uint8_t ucg_font_osr35_tn[];
const ucg_fntpgm_uint8_t ucg_font_osr35_tr[];
const ucg_fntpgm_uint8_t ucg_font_osr41_hf[];
const ucg_fntpgm_uint8_t ucg_font_osr41_hn[];
const ucg_fntpgm_uint8_t ucg_font_osr41_hr[];
const ucg_fntpgm_uint8_t ucg_font_osr41_tf[];
const ucg_fntpgm_uint8_t ucg_font_osr41_tn[];
const ucg_fntpgm_uint8_t ucg_font_osr41_tr[];

#endif



#ifdef __cplusplus
}
#endif


#endif /* _UCG_H */
