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

/*ucg_int_t ucg_GetWidth(ucg_t *ucg) {
    return ucg->dimension.w;
}

ucg_int_t ucg_GetHeight(ucg_t *ucg) {
    return ucg->dimension.h;
}*/

ucg_int_t ucg_GetWidth(ucg_t *ucg);
ucg_int_t ucg_GetHeight(ucg_t *ucg);

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

extern const ucg_fntpgm_uint8_t ucg_font_04b_03b[];
extern const ucg_fntpgm_uint8_t ucg_font_04b_03bn[];
extern const ucg_fntpgm_uint8_t ucg_font_04b_03br[];
extern const ucg_fntpgm_uint8_t ucg_font_04b_03[];
extern const ucg_fntpgm_uint8_t ucg_font_04b_03n[];
extern const ucg_fntpgm_uint8_t ucg_font_04b_03r[];
extern const ucg_fntpgm_uint8_t ucg_font_04b_24[];
extern const ucg_fntpgm_uint8_t ucg_font_04b_24n[];
extern const ucg_fntpgm_uint8_t ucg_font_04b_24r[];
extern const ucg_fntpgm_uint8_t ucg_font_10x20_67_75[];
extern const ucg_fntpgm_uint8_t ucg_font_10x20_75r[];
extern const ucg_fntpgm_uint8_t ucg_font_10x20_78_79[];
extern const ucg_fntpgm_uint8_t ucg_font_10x20[];
extern const ucg_fntpgm_uint8_t ucg_font_10x20r[];
extern const ucg_fntpgm_uint8_t ucg_font_4x6[];
extern const ucg_fntpgm_uint8_t ucg_font_4x6r[];
extern const ucg_fntpgm_uint8_t ucg_font_5x7[];
extern const ucg_fntpgm_uint8_t ucg_font_5x7r[];
extern const ucg_fntpgm_uint8_t ucg_font_5x8[];
extern const ucg_fntpgm_uint8_t ucg_font_5x8r[];
extern const ucg_fntpgm_uint8_t ucg_font_6x10[];
extern const ucg_fntpgm_uint8_t ucg_font_6x10r[];
extern const ucg_fntpgm_uint8_t ucg_font_6x12_67_75[];
extern const ucg_fntpgm_uint8_t ucg_font_6x12_75r[];
extern const ucg_fntpgm_uint8_t ucg_font_6x12_78_79[];
extern const ucg_fntpgm_uint8_t ucg_font_6x12[];
extern const ucg_fntpgm_uint8_t ucg_font_6x12r[];
extern const ucg_fntpgm_uint8_t ucg_font_6x13_67_75[];
extern const ucg_fntpgm_uint8_t ucg_font_6x13_75r[];
extern const ucg_fntpgm_uint8_t ucg_font_6x13_78_79[];
extern const ucg_fntpgm_uint8_t ucg_font_6x13B[];
extern const ucg_fntpgm_uint8_t ucg_font_6x13Br[];
extern const ucg_fntpgm_uint8_t ucg_font_6x13[];
extern const ucg_fntpgm_uint8_t ucg_font_6x13O[];
extern const ucg_fntpgm_uint8_t ucg_font_6x13Or[];
extern const ucg_fntpgm_uint8_t ucg_font_6x13r[];
extern const ucg_fntpgm_uint8_t ucg_font_7x13_67_75[];
extern const ucg_fntpgm_uint8_t ucg_font_7x13_75r[];
extern const ucg_fntpgm_uint8_t ucg_font_7x13B[];
extern const ucg_fntpgm_uint8_t ucg_font_7x13Br[];
extern const ucg_fntpgm_uint8_t ucg_font_7x13[];
extern const ucg_fntpgm_uint8_t ucg_font_7x13O[];
extern const ucg_fntpgm_uint8_t ucg_font_7x13Or[];
extern const ucg_fntpgm_uint8_t ucg_font_7x13r[];
extern const ucg_fntpgm_uint8_t ucg_font_7x14B[];
extern const ucg_fntpgm_uint8_t ucg_font_7x14Br[];
extern const ucg_fntpgm_uint8_t ucg_font_7x14[];
extern const ucg_fntpgm_uint8_t ucg_font_7x14r[];
extern const ucg_fntpgm_uint8_t ucg_font_8x13_67_75[];
extern const ucg_fntpgm_uint8_t ucg_font_8x13_75r[];
extern const ucg_fntpgm_uint8_t ucg_font_8x13B[];
extern const ucg_fntpgm_uint8_t ucg_font_8x13Br[];
extern const ucg_fntpgm_uint8_t ucg_font_8x13[];
extern const ucg_fntpgm_uint8_t ucg_font_8x13O[];
extern const ucg_fntpgm_uint8_t ucg_font_8x13Or[];
extern const ucg_fntpgm_uint8_t ucg_font_8x13r[];
extern const ucg_fntpgm_uint8_t ucg_font_9x15_67_75[];
extern const ucg_fntpgm_uint8_t ucg_font_9x15_75r[];
extern const ucg_fntpgm_uint8_t ucg_font_9x15_78_79[];
extern const ucg_fntpgm_uint8_t ucg_font_9x15B[];
extern const ucg_fntpgm_uint8_t ucg_font_9x15Br[];
extern const ucg_fntpgm_uint8_t ucg_font_9x15[];
extern const ucg_fntpgm_uint8_t ucg_font_9x15r[];
extern const ucg_fntpgm_uint8_t ucg_font_9x18_67_75[];
extern const ucg_fntpgm_uint8_t ucg_font_9x18_75r[];
extern const ucg_fntpgm_uint8_t ucg_font_9x18_78_79[];
extern const ucg_fntpgm_uint8_t ucg_font_9x18B[];
extern const ucg_fntpgm_uint8_t ucg_font_9x18Br[];
extern const ucg_fntpgm_uint8_t ucg_font_9x18[];
extern const ucg_fntpgm_uint8_t ucg_font_9x18r[];
extern const ucg_fntpgm_uint8_t ucg_font_baby[];
extern const ucg_fntpgm_uint8_t ucg_font_babyn[];
extern const ucg_fntpgm_uint8_t ucg_font_babyr[];
extern const ucg_fntpgm_uint8_t ucg_font_blipfest_07[];
extern const ucg_fntpgm_uint8_t ucg_font_blipfest_07n[];
extern const ucg_fntpgm_uint8_t ucg_font_blipfest_07r[];
extern const ucg_fntpgm_uint8_t ucg_font_chikita[];
extern const ucg_fntpgm_uint8_t ucg_font_chikitan[];
extern const ucg_fntpgm_uint8_t ucg_font_chikitar[];
extern const ucg_fntpgm_uint8_t ucg_font_courB08[];
extern const ucg_fntpgm_uint8_t ucg_font_courB08r[];
extern const ucg_fntpgm_uint8_t ucg_font_courB10[];
extern const ucg_fntpgm_uint8_t ucg_font_courB10r[];
extern const ucg_fntpgm_uint8_t ucg_font_courB12[];
extern const ucg_fntpgm_uint8_t ucg_font_courB12r[];
extern const ucg_fntpgm_uint8_t ucg_font_courB14[];
extern const ucg_fntpgm_uint8_t ucg_font_courB14r[];
extern const ucg_fntpgm_uint8_t ucg_font_courB18[];
extern const ucg_fntpgm_uint8_t ucg_font_courB18r[];
extern const ucg_fntpgm_uint8_t ucg_font_courB24[];
extern const ucg_fntpgm_uint8_t ucg_font_courB24r[];
extern const ucg_fntpgm_uint8_t ucg_font_courB24n[];
extern const ucg_fntpgm_uint8_t ucg_font_courR08[];
extern const ucg_fntpgm_uint8_t ucg_font_courR08r[];
extern const ucg_fntpgm_uint8_t ucg_font_courR10[];
extern const ucg_fntpgm_uint8_t ucg_font_courR10r[];
extern const ucg_fntpgm_uint8_t ucg_font_courR12[];
extern const ucg_fntpgm_uint8_t ucg_font_courR12r[];
extern const ucg_fntpgm_uint8_t ucg_font_courR14[];
extern const ucg_fntpgm_uint8_t ucg_font_courR14r[];
extern const ucg_fntpgm_uint8_t ucg_font_courR18[];
extern const ucg_fntpgm_uint8_t ucg_font_courR18r[];
extern const ucg_fntpgm_uint8_t ucg_font_courR24[];
extern const ucg_fntpgm_uint8_t ucg_font_courR24n[];
extern const ucg_fntpgm_uint8_t ucg_font_courR24r[];
extern const ucg_fntpgm_uint8_t ucg_font_cu12_67_75[];
extern const ucg_fntpgm_uint8_t ucg_font_cu12_75r[];
extern const ucg_fntpgm_uint8_t ucg_font_cu12[];
extern const ucg_fntpgm_uint8_t ucg_font_cursor[];
extern const ucg_fntpgm_uint8_t ucg_font_cursorr[];
extern const ucg_fntpgm_uint8_t ucg_font_fixed_v0[];
extern const ucg_fntpgm_uint8_t ucg_font_fixed_v0n[];
extern const ucg_fntpgm_uint8_t ucg_font_fixed_v0r[];
extern const ucg_fntpgm_uint8_t ucg_font_freedoomr10r[];
extern const ucg_fntpgm_uint8_t ucg_font_freedoomr25n[];
extern const ucg_fntpgm_uint8_t ucg_font_helvB08[];
extern const ucg_fntpgm_uint8_t ucg_font_helvB08r[];
extern const ucg_fntpgm_uint8_t ucg_font_helvB10[];
extern const ucg_fntpgm_uint8_t ucg_font_helvB10r[];
extern const ucg_fntpgm_uint8_t ucg_font_helvB12[];
extern const ucg_fntpgm_uint8_t ucg_font_helvB12r[];
extern const ucg_fntpgm_uint8_t ucg_font_helvB14[];
extern const ucg_fntpgm_uint8_t ucg_font_helvB14r[];
extern const ucg_fntpgm_uint8_t ucg_font_helvB18[];
extern const ucg_fntpgm_uint8_t ucg_font_helvB18r[];
extern const ucg_fntpgm_uint8_t ucg_font_helvB24[];
extern const ucg_fntpgm_uint8_t ucg_font_helvB24n[];
extern const ucg_fntpgm_uint8_t ucg_font_helvB24r[];
extern const ucg_fntpgm_uint8_t ucg_font_helvR08[];
extern const ucg_fntpgm_uint8_t ucg_font_helvR08r[];
extern const ucg_fntpgm_uint8_t ucg_font_helvR10[];
extern const ucg_fntpgm_uint8_t ucg_font_helvR10r[];
extern const ucg_fntpgm_uint8_t ucg_font_helvR12[];
extern const ucg_fntpgm_uint8_t ucg_font_helvR12r[];
extern const ucg_fntpgm_uint8_t ucg_font_helvR14[];
extern const ucg_fntpgm_uint8_t ucg_font_helvR14r[];
extern const ucg_fntpgm_uint8_t ucg_font_helvR18[];
extern const ucg_fntpgm_uint8_t ucg_font_helvR18r[];
extern const ucg_fntpgm_uint8_t ucg_font_helvR24[];
extern const ucg_fntpgm_uint8_t ucg_font_helvR24n[];
extern const ucg_fntpgm_uint8_t ucg_font_helvR24r[];
extern const ucg_fntpgm_uint8_t ucg_font_lucasfont_alternate[];
extern const ucg_fntpgm_uint8_t ucg_font_lucasfont_alternaten[];
extern const ucg_fntpgm_uint8_t ucg_font_lucasfont_alternater[];
extern const ucg_fntpgm_uint8_t ucg_font_m2icon_5[];
extern const ucg_fntpgm_uint8_t ucg_font_m2icon_7[];
extern const ucg_fntpgm_uint8_t ucg_font_m2icon_9[];
extern const ucg_fntpgm_uint8_t ucg_font_micro[];
extern const ucg_fntpgm_uint8_t ucg_font_ncenB08[];
extern const ucg_fntpgm_uint8_t ucg_font_ncenB08r[];
extern const ucg_fntpgm_uint8_t ucg_font_ncenB10[];
extern const ucg_fntpgm_uint8_t ucg_font_ncenB10r[];
extern const ucg_fntpgm_uint8_t ucg_font_ncenB12[];
extern const ucg_fntpgm_uint8_t ucg_font_ncenB12r[];
extern const ucg_fntpgm_uint8_t ucg_font_ncenB14[];
extern const ucg_fntpgm_uint8_t ucg_font_ncenB14r[];
extern const ucg_fntpgm_uint8_t ucg_font_ncenB18[];
extern const ucg_fntpgm_uint8_t ucg_font_ncenB18r[];
extern const ucg_fntpgm_uint8_t ucg_font_ncenB24[];
extern const ucg_fntpgm_uint8_t ucg_font_ncenB24n[];
extern const ucg_fntpgm_uint8_t ucg_font_ncenB24r[];
extern const ucg_fntpgm_uint8_t ucg_font_ncenR08[];
extern const ucg_fntpgm_uint8_t ucg_font_ncenR08r[];
extern const ucg_fntpgm_uint8_t ucg_font_ncenR10[];
extern const ucg_fntpgm_uint8_t ucg_font_ncenR10r[];
extern const ucg_fntpgm_uint8_t ucg_font_ncenR12[];
extern const ucg_fntpgm_uint8_t ucg_font_ncenR12r[];
extern const ucg_fntpgm_uint8_t ucg_font_ncenR14[];
extern const ucg_fntpgm_uint8_t ucg_font_ncenR14r[];
extern const ucg_fntpgm_uint8_t ucg_font_ncenR18[];
extern const ucg_fntpgm_uint8_t ucg_font_ncenR18r[];
extern const ucg_fntpgm_uint8_t ucg_font_ncenR24[];
extern const ucg_fntpgm_uint8_t ucg_font_ncenR24n[];
extern const ucg_fntpgm_uint8_t ucg_font_ncenR24r[];
extern const ucg_fntpgm_uint8_t ucg_font_orgv01[];
extern const ucg_fntpgm_uint8_t ucg_font_orgv01n[];
extern const ucg_fntpgm_uint8_t ucg_font_orgv01r[];
extern const ucg_fntpgm_uint8_t ucg_font_p01type[];
extern const ucg_fntpgm_uint8_t ucg_font_p01typen[];
extern const ucg_fntpgm_uint8_t ucg_font_p01typer[];
extern const ucg_fntpgm_uint8_t ucg_font_pixelle_micro[];
extern const ucg_fntpgm_uint8_t ucg_font_pixelle_micron[];
extern const ucg_fntpgm_uint8_t ucg_font_pixelle_micror[];
extern const ucg_fntpgm_uint8_t ucg_font_profont10[];
extern const ucg_fntpgm_uint8_t ucg_font_profont10r[];
extern const ucg_fntpgm_uint8_t ucg_font_profont11[];
extern const ucg_fntpgm_uint8_t ucg_font_profont11r[];
extern const ucg_fntpgm_uint8_t ucg_font_profont12[];
extern const ucg_fntpgm_uint8_t ucg_font_profont12r[];
extern const ucg_fntpgm_uint8_t ucg_font_profont15[];
extern const ucg_fntpgm_uint8_t ucg_font_profont15r[];
extern const ucg_fntpgm_uint8_t ucg_font_profont17[];
extern const ucg_fntpgm_uint8_t ucg_font_profont17r[];
extern const ucg_fntpgm_uint8_t ucg_font_profont22[];
extern const ucg_fntpgm_uint8_t ucg_font_profont22n[];
extern const ucg_fntpgm_uint8_t ucg_font_profont22r[];
extern const ucg_fntpgm_uint8_t ucg_font_profont29[];
extern const ucg_fntpgm_uint8_t ucg_font_profont29n[];
extern const ucg_fntpgm_uint8_t ucg_font_profont29r[];
extern const ucg_fntpgm_uint8_t ucg_font_robot_de_niro[];
extern const ucg_fntpgm_uint8_t ucg_font_robot_de_niron[];
extern const ucg_fntpgm_uint8_t ucg_font_robot_de_niror[];
extern const ucg_fntpgm_uint8_t ucg_font_symb08[];
extern const ucg_fntpgm_uint8_t ucg_font_symb08r[];
extern const ucg_fntpgm_uint8_t ucg_font_symb10[];
extern const ucg_fntpgm_uint8_t ucg_font_symb10r[];
extern const ucg_fntpgm_uint8_t ucg_font_symb12[];
extern const ucg_fntpgm_uint8_t ucg_font_symb12r[];
extern const ucg_fntpgm_uint8_t ucg_font_symb14[];
extern const ucg_fntpgm_uint8_t ucg_font_symb14r[];
extern const ucg_fntpgm_uint8_t ucg_font_symb18[];
extern const ucg_fntpgm_uint8_t ucg_font_symb18r[];
extern const ucg_fntpgm_uint8_t ucg_font_symb24[];
extern const ucg_fntpgm_uint8_t ucg_font_symb24r[];
extern const ucg_fntpgm_uint8_t ucg_font_timB08[];
extern const ucg_fntpgm_uint8_t ucg_font_timB08r[];
extern const ucg_fntpgm_uint8_t ucg_font_timB10[];
extern const ucg_fntpgm_uint8_t ucg_font_timB10r[];
extern const ucg_fntpgm_uint8_t ucg_font_timB12[];
extern const ucg_fntpgm_uint8_t ucg_font_timB12r[];
extern const ucg_fntpgm_uint8_t ucg_font_timB14[];
extern const ucg_fntpgm_uint8_t ucg_font_timB14r[];
extern const ucg_fntpgm_uint8_t ucg_font_timB18[];
extern const ucg_fntpgm_uint8_t ucg_font_timB18r[];
extern const ucg_fntpgm_uint8_t ucg_font_timB24[];
extern const ucg_fntpgm_uint8_t ucg_font_timB24n[];
extern const ucg_fntpgm_uint8_t ucg_font_timB24r[];
extern const ucg_fntpgm_uint8_t ucg_font_timR08[];
extern const ucg_fntpgm_uint8_t ucg_font_timR08r[];
extern const ucg_fntpgm_uint8_t ucg_font_timR10[];
extern const ucg_fntpgm_uint8_t ucg_font_timR10r[];
extern const ucg_fntpgm_uint8_t ucg_font_timR12[];
extern const ucg_fntpgm_uint8_t ucg_font_timR12r[];
extern const ucg_fntpgm_uint8_t ucg_font_timR14[];
extern const ucg_fntpgm_uint8_t ucg_font_timR14r[];
extern const ucg_fntpgm_uint8_t ucg_font_timR18[];
extern const ucg_fntpgm_uint8_t ucg_font_timR18r[];
extern const ucg_fntpgm_uint8_t ucg_font_timR24[];
extern const ucg_fntpgm_uint8_t ucg_font_timR24n[];
extern const ucg_fntpgm_uint8_t ucg_font_timR24r[];
extern const ucg_fntpgm_uint8_t ucg_font_tpssb[];
extern const ucg_fntpgm_uint8_t ucg_font_tpssbn[];
extern const ucg_fntpgm_uint8_t ucg_font_tpssbr[];
extern const ucg_fntpgm_uint8_t ucg_font_tpss[];
extern const ucg_fntpgm_uint8_t ucg_font_tpssn[];
extern const ucg_fntpgm_uint8_t ucg_font_tpssr[];
extern const ucg_fntpgm_uint8_t ucg_font_trixel_square[];
extern const ucg_fntpgm_uint8_t ucg_font_trixel_squaren[];
extern const ucg_fntpgm_uint8_t ucg_font_trixel_squarer[];
extern const ucg_fntpgm_uint8_t ucg_font_u8glib_4[];
extern const ucg_fntpgm_uint8_t ucg_font_u8glib_4r[];
extern const ucg_fntpgm_uint8_t ucg_font_unifont_0_8[];
extern const ucg_fntpgm_uint8_t ucg_font_unifont_12_13[];
extern const ucg_fntpgm_uint8_t ucg_font_unifont_18_19[];
extern const ucg_fntpgm_uint8_t ucg_font_unifont_2_3[];
extern const ucg_fntpgm_uint8_t ucg_font_unifont_4_5[];
extern const ucg_fntpgm_uint8_t ucg_font_unifont_67_75[];
extern const ucg_fntpgm_uint8_t ucg_font_unifont_72_73[];
extern const ucg_fntpgm_uint8_t ucg_font_unifont_75r[];
extern const ucg_fntpgm_uint8_t ucg_font_unifont_76[];
extern const ucg_fntpgm_uint8_t ucg_font_unifont_77[];
extern const ucg_fntpgm_uint8_t ucg_font_unifont_78_79[];
extern const ucg_fntpgm_uint8_t ucg_font_unifont_86[];
extern const ucg_fntpgm_uint8_t ucg_font_unifont_8_9[];
extern const ucg_fntpgm_uint8_t ucg_font_unifont[];
extern const ucg_fntpgm_uint8_t ucg_font_unifontr[];
extern const ucg_fntpgm_uint8_t ucg_font_fub11[];
extern const ucg_fntpgm_uint8_t ucg_font_fub11n[];
extern const ucg_fntpgm_uint8_t ucg_font_fub11r[];
extern const ucg_fntpgm_uint8_t ucg_font_fub14[];
extern const ucg_fntpgm_uint8_t ucg_font_fub14n[];
extern const ucg_fntpgm_uint8_t ucg_font_fub14r[];
extern const ucg_fntpgm_uint8_t ucg_font_fub17[];
extern const ucg_fntpgm_uint8_t ucg_font_fub17n[];
extern const ucg_fntpgm_uint8_t ucg_font_fub17r[];
extern const ucg_fntpgm_uint8_t ucg_font_fub20[];
extern const ucg_fntpgm_uint8_t ucg_font_fub20n[];
extern const ucg_fntpgm_uint8_t ucg_font_fub20r[];
extern const ucg_fntpgm_uint8_t ucg_font_fub25[];
extern const ucg_fntpgm_uint8_t ucg_font_fub25n[];
extern const ucg_fntpgm_uint8_t ucg_font_fub25r[];
extern const ucg_fntpgm_uint8_t ucg_font_fub30[];
extern const ucg_fntpgm_uint8_t ucg_font_fub30n[];
extern const ucg_fntpgm_uint8_t ucg_font_fub30r[];
extern const ucg_fntpgm_uint8_t ucg_font_fub35n[];
extern const ucg_fntpgm_uint8_t ucg_font_fub42n[];
extern const ucg_fntpgm_uint8_t ucg_font_fub49n[];
extern const ucg_fntpgm_uint8_t ucg_font_fur11[];
extern const ucg_fntpgm_uint8_t ucg_font_fur11n[];
extern const ucg_fntpgm_uint8_t ucg_font_fur11r[];
extern const ucg_fntpgm_uint8_t ucg_font_fur14[];
extern const ucg_fntpgm_uint8_t ucg_font_fur14n[];
extern const ucg_fntpgm_uint8_t ucg_font_fur14r[];
extern const ucg_fntpgm_uint8_t ucg_font_fur17[];
extern const ucg_fntpgm_uint8_t ucg_font_fur17n[];
extern const ucg_fntpgm_uint8_t ucg_font_fur17r[];
extern const ucg_fntpgm_uint8_t ucg_font_fur20[];
extern const ucg_fntpgm_uint8_t ucg_font_fur20n[];
extern const ucg_fntpgm_uint8_t ucg_font_fur20r[];
extern const ucg_fntpgm_uint8_t ucg_font_fur25[];
extern const ucg_fntpgm_uint8_t ucg_font_fur25n[];
extern const ucg_fntpgm_uint8_t ucg_font_fur25r[];
extern const ucg_fntpgm_uint8_t ucg_font_fur30[];
extern const ucg_fntpgm_uint8_t ucg_font_fur30n[];
extern const ucg_fntpgm_uint8_t ucg_font_fur30r[];
extern const ucg_fntpgm_uint8_t ucg_font_fur35n[];
extern const ucg_fntpgm_uint8_t ucg_font_fur42n[];
extern const ucg_fntpgm_uint8_t ucg_font_fur49n[];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso16[];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso16n[];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso16r[];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso18[];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso18n[];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso18r[];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso20[];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso20n[];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso20r[];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso22[];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso22n[];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso22r[];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso24[];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso24n[];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso24r[];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso26[];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso26n[];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso26r[];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso28[];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso28n[];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso28r[];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso30[];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso30n[];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso30r[];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso32[];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso32n[];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso32r[];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso34[];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso34n[];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso34r[];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso38[];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso38n[];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso38r[];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso42[];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso42n[];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso42r[];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso46n[];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso46r[];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso50n[];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso50r[];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso54n[];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso58n[];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso62n[];
extern const ucg_fntpgm_uint8_t ucg_font_osb18[];
extern const ucg_fntpgm_uint8_t ucg_font_osb18n[];
extern const ucg_fntpgm_uint8_t ucg_font_osb18r[];
extern const ucg_fntpgm_uint8_t ucg_font_osb21[];
extern const ucg_fntpgm_uint8_t ucg_font_osb21n[];
extern const ucg_fntpgm_uint8_t ucg_font_osb21r[];
extern const ucg_fntpgm_uint8_t ucg_font_osb26[];
extern const ucg_fntpgm_uint8_t ucg_font_osb26n[];
extern const ucg_fntpgm_uint8_t ucg_font_osb26r[];
extern const ucg_fntpgm_uint8_t ucg_font_osb29[];
extern const ucg_fntpgm_uint8_t ucg_font_osb29n[];
extern const ucg_fntpgm_uint8_t ucg_font_osb29r[];
extern const ucg_fntpgm_uint8_t ucg_font_osb35[];
extern const ucg_fntpgm_uint8_t ucg_font_osb35n[];
extern const ucg_fntpgm_uint8_t ucg_font_osb35r[];
extern const ucg_fntpgm_uint8_t ucg_font_osr18[];
extern const ucg_fntpgm_uint8_t ucg_font_osr18n[];
extern const ucg_fntpgm_uint8_t ucg_font_osr18r[];
extern const ucg_fntpgm_uint8_t ucg_font_osr21[];
extern const ucg_fntpgm_uint8_t ucg_font_osr21n[];
extern const ucg_fntpgm_uint8_t ucg_font_osr21r[];
extern const ucg_fntpgm_uint8_t ucg_font_osr26[];
extern const ucg_fntpgm_uint8_t ucg_font_osr26n[];
extern const ucg_fntpgm_uint8_t ucg_font_osr26r[];
extern const ucg_fntpgm_uint8_t ucg_font_osr29[];
extern const ucg_fntpgm_uint8_t ucg_font_osr29n[];
extern const ucg_fntpgm_uint8_t ucg_font_osr29r[];
extern const ucg_fntpgm_uint8_t ucg_font_osr35[];
extern const ucg_fntpgm_uint8_t ucg_font_osr35n[];
extern const ucg_fntpgm_uint8_t ucg_font_osr35r[];

#else

extern const ucg_fntpgm_uint8_t ucg_font_04b_03b_hf[902];
extern const ucg_fntpgm_uint8_t ucg_font_04b_03b_hn[179];
extern const ucg_fntpgm_uint8_t ucg_font_04b_03b_hr[896];
extern const ucg_fntpgm_uint8_t ucg_font_04b_03b_tf[732];
extern const ucg_fntpgm_uint8_t ucg_font_04b_03b_tn[143];
extern const ucg_fntpgm_uint8_t ucg_font_04b_03b_tr[728];
extern const ucg_fntpgm_uint8_t ucg_font_04b_03_hf[898];
extern const ucg_fntpgm_uint8_t ucg_font_04b_03_hn[168];
extern const ucg_fntpgm_uint8_t ucg_font_04b_03_hr[893];
extern const ucg_fntpgm_uint8_t ucg_font_04b_03_tf[765];
extern const ucg_fntpgm_uint8_t ucg_font_04b_03_tn[158];
extern const ucg_fntpgm_uint8_t ucg_font_04b_03_tr[761];
extern const ucg_fntpgm_uint8_t ucg_font_04b_24_hf[824];
extern const ucg_fntpgm_uint8_t ucg_font_04b_24_hn[158];
extern const ucg_fntpgm_uint8_t ucg_font_04b_24_hr[768];
extern const ucg_fntpgm_uint8_t ucg_font_04b_24_tf[759];
extern const ucg_fntpgm_uint8_t ucg_font_04b_24_tn[133];
extern const ucg_fntpgm_uint8_t ucg_font_04b_24_tr[707];
extern const ucg_fntpgm_uint8_t ucg_font_10x20_67_75[4131];
extern const ucg_fntpgm_uint8_t ucg_font_10x20_78_79[2449];
extern const ucg_fntpgm_uint8_t ucg_font_10x20_mf[4198];
extern const ucg_fntpgm_uint8_t ucg_font_10x20_mr[1961];
extern const ucg_fntpgm_uint8_t ucg_font_10x20_tf[3058];
extern const ucg_fntpgm_uint8_t ucg_font_10x20_tr[1417];
extern const ucg_fntpgm_uint8_t ucg_font_4x6_mf[1593];
extern const ucg_fntpgm_uint8_t ucg_font_4x6_mr[786];
extern const ucg_fntpgm_uint8_t ucg_font_4x6_tf[1469];
extern const ucg_fntpgm_uint8_t ucg_font_4x6_tr[726];
extern const ucg_fntpgm_uint8_t ucg_font_5x7_8f[2233];
extern const ucg_fntpgm_uint8_t ucg_font_5x7_8r[1085];
extern const ucg_fntpgm_uint8_t ucg_font_5x7_mf[1910];
extern const ucg_fntpgm_uint8_t ucg_font_5x7_mr[937];
extern const ucg_fntpgm_uint8_t ucg_font_5x7_tf[1669];
extern const ucg_fntpgm_uint8_t ucg_font_5x7_tr[833];
extern const ucg_fntpgm_uint8_t ucg_font_5x8_8f[2279];
extern const ucg_fntpgm_uint8_t ucg_font_5x8_8r[1102];
extern const ucg_fntpgm_uint8_t ucg_font_5x8_mf[2007];
extern const ucg_fntpgm_uint8_t ucg_font_5x8_mr[987];
extern const ucg_fntpgm_uint8_t ucg_font_5x8_tf[1778];
extern const ucg_fntpgm_uint8_t ucg_font_5x8_tr[877];
extern const ucg_fntpgm_uint8_t ucg_font_6x10_mf[2414];
extern const ucg_fntpgm_uint8_t ucg_font_6x10_mr[1162];
extern const ucg_fntpgm_uint8_t ucg_font_6x10_tf[2009];
extern const ucg_fntpgm_uint8_t ucg_font_6x10_tr[971];
extern const ucg_fntpgm_uint8_t ucg_font_6x12_67_75[2747];
extern const ucg_fntpgm_uint8_t ucg_font_6x12_75[490];
extern const ucg_fntpgm_uint8_t ucg_font_6x12_78_79[2780];
extern const ucg_fntpgm_uint8_t ucg_font_6x12_mf[2501];
extern const ucg_fntpgm_uint8_t ucg_font_6x12_mr[1185];
extern const ucg_fntpgm_uint8_t ucg_font_6x12_tf[2017];
extern const ucg_fntpgm_uint8_t ucg_font_6x12_tr[963];
extern const ucg_fntpgm_uint8_t ucg_font_6x13_67_75[2762];
extern const ucg_fntpgm_uint8_t ucg_font_6x13_78_79[1924];
extern const ucg_fntpgm_uint8_t ucg_font_6x13B_tf[2250];
extern const ucg_fntpgm_uint8_t ucg_font_6x13B_tr[1058];
extern const ucg_fntpgm_uint8_t ucg_font_6x13_mf[2735];
extern const ucg_fntpgm_uint8_t ucg_font_6x13_mr[1299];
extern const ucg_fntpgm_uint8_t ucg_font_6x13O_tf[2523];
extern const ucg_fntpgm_uint8_t ucg_font_6x13O_tr[1211];
extern const ucg_fntpgm_uint8_t ucg_font_6x13_tf[2169];
extern const ucg_fntpgm_uint8_t ucg_font_6x13_tr[1048];
extern const ucg_fntpgm_uint8_t ucg_font_7x13_67_75[2790];
extern const ucg_fntpgm_uint8_t ucg_font_7x13_78_79[271];
extern const ucg_fntpgm_uint8_t ucg_font_7x13B_tf[2253];
extern const ucg_fntpgm_uint8_t ucg_font_7x13B_tr[1075];
extern const ucg_fntpgm_uint8_t ucg_font_7x13_mf[2994];
extern const ucg_fntpgm_uint8_t ucg_font_7x13_mr[1414];
extern const ucg_fntpgm_uint8_t ucg_font_7x13O_tf[2687];
extern const ucg_fntpgm_uint8_t ucg_font_7x13O_tr[1259];
extern const ucg_fntpgm_uint8_t ucg_font_7x13_tf[2331];
extern const ucg_fntpgm_uint8_t ucg_font_7x13_tr[1103];
extern const ucg_fntpgm_uint8_t ucg_font_7x14B_mf[3124];
extern const ucg_fntpgm_uint8_t ucg_font_7x14B_mr[1469];
extern const ucg_fntpgm_uint8_t ucg_font_7x14B_tf[2317];
extern const ucg_fntpgm_uint8_t ucg_font_7x14B_tr[1085];
extern const ucg_fntpgm_uint8_t ucg_font_7x14_mf[3201];
extern const ucg_fntpgm_uint8_t ucg_font_7x14_mr[1522];
extern const ucg_fntpgm_uint8_t ucg_font_7x14_tf[2428];
extern const ucg_fntpgm_uint8_t ucg_font_7x14_tr[1146];
extern const ucg_fntpgm_uint8_t ucg_font_8x13_67_75[3274];
extern const ucg_fntpgm_uint8_t ucg_font_8x13B_mf[3233];
extern const ucg_fntpgm_uint8_t ucg_font_8x13B_mr[1551];
extern const ucg_fntpgm_uint8_t ucg_font_8x13B_tf[2439];
extern const ucg_fntpgm_uint8_t ucg_font_8x13B_tr[1153];
extern const ucg_fntpgm_uint8_t ucg_font_8x13_mf[3153];
extern const ucg_fntpgm_uint8_t ucg_font_8x13_mr[1519];
extern const ucg_fntpgm_uint8_t ucg_font_8x13O_mf[3210];
extern const ucg_fntpgm_uint8_t ucg_font_8x13O_mr[1558];
extern const ucg_fntpgm_uint8_t ucg_font_8x13O_tf[2738];
extern const ucg_fntpgm_uint8_t ucg_font_8x13O_tr[1292];
extern const ucg_fntpgm_uint8_t ucg_font_8x13_tf[2387];
extern const ucg_fntpgm_uint8_t ucg_font_8x13_tr[1126];
extern const ucg_fntpgm_uint8_t ucg_font_9x15_67_75[3483];
extern const ucg_fntpgm_uint8_t ucg_font_9x15_78_79[3637];
extern const ucg_fntpgm_uint8_t ucg_font_9x15B_mf[3559];
extern const ucg_fntpgm_uint8_t ucg_font_9x15B_mr[1665];
extern const ucg_fntpgm_uint8_t ucg_font_9x15B_tf[2749];
extern const ucg_fntpgm_uint8_t ucg_font_9x15B_tr[1271];
extern const ucg_fntpgm_uint8_t ucg_font_9x15_mf[3546];
extern const ucg_fntpgm_uint8_t ucg_font_9x15_mr[1674];
extern const ucg_fntpgm_uint8_t ucg_font_9x15_tf[2649];
extern const ucg_fntpgm_uint8_t ucg_font_9x15_tr[1242];
extern const ucg_fntpgm_uint8_t ucg_font_9x18_67_75[3534];
extern const ucg_fntpgm_uint8_t ucg_font_9x18_78_79[3138];
extern const ucg_fntpgm_uint8_t ucg_font_9x18B_mf[3602];
extern const ucg_fntpgm_uint8_t ucg_font_9x18B_mr[1661];
extern const ucg_fntpgm_uint8_t ucg_font_9x18B_tf[2613];
extern const ucg_fntpgm_uint8_t ucg_font_9x18B_tr[1203];
extern const ucg_fntpgm_uint8_t ucg_font_9x18_mf[3616];
extern const ucg_fntpgm_uint8_t ucg_font_9x18_mr[1666];
extern const ucg_fntpgm_uint8_t ucg_font_9x18_tf[2690];
extern const ucg_fntpgm_uint8_t ucg_font_9x18_tr[1250];
extern const ucg_fntpgm_uint8_t ucg_font_amstrad_cpc_8f[2465];
extern const ucg_fntpgm_uint8_t ucg_font_amstrad_cpc_8r[1222];
extern const ucg_fntpgm_uint8_t ucg_font_baby_hf[2171];
extern const ucg_fntpgm_uint8_t ucg_font_baby_hn[178];
extern const ucg_fntpgm_uint8_t ucg_font_baby_hr[974];
extern const ucg_fntpgm_uint8_t ucg_font_baby_tf[1776];
extern const ucg_fntpgm_uint8_t ucg_font_baby_tn[152];
extern const ucg_fntpgm_uint8_t ucg_font_baby_tr[792];
extern const ucg_fntpgm_uint8_t ucg_font_blipfest_07_hf[635];
extern const ucg_fntpgm_uint8_t ucg_font_blipfest_07_hn[141];
extern const ucg_fntpgm_uint8_t ucg_font_blipfest_07_hr[635];
extern const ucg_fntpgm_uint8_t ucg_font_blipfest_07_tf[533];
extern const ucg_fntpgm_uint8_t ucg_font_blipfest_07_tn[116];
extern const ucg_fntpgm_uint8_t ucg_font_blipfest_07_tr[533];
extern const ucg_fntpgm_uint8_t ucg_font_chikita_hf[2256];
extern const ucg_fntpgm_uint8_t ucg_font_chikita_hn[185];
extern const ucg_fntpgm_uint8_t ucg_font_chikita_hr[1023];
extern const ucg_fntpgm_uint8_t ucg_font_chikita_tf[1865];
extern const ucg_fntpgm_uint8_t ucg_font_chikita_tn[166];
extern const ucg_fntpgm_uint8_t ucg_font_chikita_tr[854];
extern const ucg_fntpgm_uint8_t ucg_font_courB08_mf[2763];
extern const ucg_fntpgm_uint8_t ucg_font_courB08_mr[1242];
extern const ucg_fntpgm_uint8_t ucg_font_courB08_tf[2167];
extern const ucg_fntpgm_uint8_t ucg_font_courB08_tr[986];
extern const ucg_fntpgm_uint8_t ucg_font_courB10_mf[3816];
extern const ucg_fntpgm_uint8_t ucg_font_courB10_mr[1627];
extern const ucg_fntpgm_uint8_t ucg_font_courB10_tf[2954];
extern const ucg_fntpgm_uint8_t ucg_font_courB10_tr[1355];
extern const ucg_fntpgm_uint8_t ucg_font_courB12_mf[3953];
extern const ucg_fntpgm_uint8_t ucg_font_courB12_mr[1748];
extern const ucg_fntpgm_uint8_t ucg_font_courB12_tf[3312];
extern const ucg_fntpgm_uint8_t ucg_font_courB12_tr[1538];
extern const ucg_fntpgm_uint8_t ucg_font_courB14_mf[4384];
extern const ucg_fntpgm_uint8_t ucg_font_courB14_mr[2003];
extern const ucg_fntpgm_uint8_t ucg_font_courB14_tf[3759];
extern const ucg_fntpgm_uint8_t ucg_font_courB14_tr[1747];
extern const ucg_fntpgm_uint8_t ucg_font_courB18_mf[6145];
extern const ucg_fntpgm_uint8_t ucg_font_courB18_mn[442];
extern const ucg_fntpgm_uint8_t ucg_font_courB18_mr[2550];
extern const ucg_fntpgm_uint8_t ucg_font_courB18_tf[4851];
extern const ucg_fntpgm_uint8_t ucg_font_courB18_tn[353];
extern const ucg_fntpgm_uint8_t ucg_font_courB18_tr[2236];
extern const ucg_fntpgm_uint8_t ucg_font_courB24_mf[8013];
extern const ucg_fntpgm_uint8_t ucg_font_courB24_mn[564];
extern const ucg_fntpgm_uint8_t ucg_font_courB24_mr[3602];
extern const ucg_fntpgm_uint8_t ucg_font_courB24_tf[6661];
extern const ucg_fntpgm_uint8_t ucg_font_courB24_tn[460];
extern const ucg_fntpgm_uint8_t ucg_font_courB24_tr[3015];
extern const ucg_fntpgm_uint8_t ucg_font_courR08_mf[2591];
extern const ucg_fntpgm_uint8_t ucg_font_courR08_mr[1092];
extern const ucg_fntpgm_uint8_t ucg_font_courR08_tf[2064];
extern const ucg_fntpgm_uint8_t ucg_font_courR08_tr[989];
extern const ucg_fntpgm_uint8_t ucg_font_courR10_mf[3474];
extern const ucg_fntpgm_uint8_t ucg_font_courR10_mr[1594];
extern const ucg_fntpgm_uint8_t ucg_font_courR10_tf[2914];
extern const ucg_fntpgm_uint8_t ucg_font_courR10_tr[1331];
extern const ucg_fntpgm_uint8_t ucg_font_courR12_mf[3739];
extern const ucg_fntpgm_uint8_t ucg_font_courR12_mr[1704];
extern const ucg_fntpgm_uint8_t ucg_font_courR12_tf[3169];
extern const ucg_fntpgm_uint8_t ucg_font_courR12_tr[1465];
extern const ucg_fntpgm_uint8_t ucg_font_courR14_mf[4052];
extern const ucg_fntpgm_uint8_t ucg_font_courR14_mr[1894];
extern const ucg_fntpgm_uint8_t ucg_font_courR14_tf[3644];
extern const ucg_fntpgm_uint8_t ucg_font_courR14_tr[1674];
extern const ucg_fntpgm_uint8_t ucg_font_courR18_mf[5425];
extern const ucg_fntpgm_uint8_t ucg_font_courR18_mr[2403];
extern const ucg_fntpgm_uint8_t ucg_font_courR18_tf[4544];
extern const ucg_fntpgm_uint8_t ucg_font_courR18_tr[2069];
extern const ucg_fntpgm_uint8_t ucg_font_courR24_mf[7105];
extern const ucg_fntpgm_uint8_t ucg_font_courR24_mn[520];
extern const ucg_fntpgm_uint8_t ucg_font_courR24_mr[3179];
extern const ucg_fntpgm_uint8_t ucg_font_courR24_tf[6078];
extern const ucg_fntpgm_uint8_t ucg_font_courR24_tn[422];
extern const ucg_fntpgm_uint8_t ucg_font_courR24_tr[2750];
extern const ucg_fntpgm_uint8_t ucg_font_cu12_67_75[3705];
extern const ucg_fntpgm_uint8_t ucg_font_cu12_75[936];
extern const ucg_fntpgm_uint8_t ucg_font_cu12_hf[3907];
extern const ucg_fntpgm_uint8_t ucg_font_cu12_mf[4559];
extern const ucg_fntpgm_uint8_t ucg_font_cu12_tf[3356];
extern const ucg_fntpgm_uint8_t ucg_font_cursor_tf[4198];
extern const ucg_fntpgm_uint8_t ucg_font_cursor_tr[390];
extern const ucg_fntpgm_uint8_t ucg_font_fixed_v0_hf[2174];
extern const ucg_fntpgm_uint8_t ucg_font_fixed_v0_hn[202];
extern const ucg_fntpgm_uint8_t ucg_font_fixed_v0_hr[1127];
extern const ucg_fntpgm_uint8_t ucg_font_fixed_v0_mr[1270];
extern const ucg_fntpgm_uint8_t ucg_font_fixed_v0_tf[1810];
extern const ucg_fntpgm_uint8_t ucg_font_fixed_v0_tn[182];
extern const ucg_fntpgm_uint8_t ucg_font_fixed_v0_tr[935];
extern const ucg_fntpgm_uint8_t ucg_font_freedoomr10_tr[812];
extern const ucg_fntpgm_uint8_t ucg_font_freedoomr25_tn[233];
extern const ucg_fntpgm_uint8_t ucg_font_helvB08_hf[2672];
extern const ucg_fntpgm_uint8_t ucg_font_helvB08_hr[1236];
extern const ucg_fntpgm_uint8_t ucg_font_helvB08_tf[2252];
extern const ucg_fntpgm_uint8_t ucg_font_helvB08_tr[1054];
extern const ucg_fntpgm_uint8_t ucg_font_helvB10_hf[3674];
extern const ucg_fntpgm_uint8_t ucg_font_helvB10_hr[1693];
extern const ucg_fntpgm_uint8_t ucg_font_helvB10_tf[2901];
extern const ucg_fntpgm_uint8_t ucg_font_helvB10_tr[1339];
extern const ucg_fntpgm_uint8_t ucg_font_helvB12_hf[4362];
extern const ucg_fntpgm_uint8_t ucg_font_helvB12_hr[1968];
extern const ucg_fntpgm_uint8_t ucg_font_helvB12_tf[3333];
extern const ucg_fntpgm_uint8_t ucg_font_helvB12_tr[1522];
extern const ucg_fntpgm_uint8_t ucg_font_helvB14_hf[4971];
extern const ucg_fntpgm_uint8_t ucg_font_helvB14_hr[2164];
extern const ucg_fntpgm_uint8_t ucg_font_helvB14_tf[3863];
extern const ucg_fntpgm_uint8_t ucg_font_helvB14_tr[1763];
extern const ucg_fntpgm_uint8_t ucg_font_helvB18_hf[6220];
extern const ucg_fntpgm_uint8_t ucg_font_helvB18_hr[2790];
extern const ucg_fntpgm_uint8_t ucg_font_helvB18_tf[4948];
extern const ucg_fntpgm_uint8_t ucg_font_helvB18_tr[2245];
extern const ucg_fntpgm_uint8_t ucg_font_helvB24_hf[8873];
extern const ucg_fntpgm_uint8_t ucg_font_helvB24_hn[608];
extern const ucg_fntpgm_uint8_t ucg_font_helvB24_hr[3981];
extern const ucg_fntpgm_uint8_t ucg_font_helvB24_tf[6638];
extern const ucg_fntpgm_uint8_t ucg_font_helvB24_tn[453];
extern const ucg_fntpgm_uint8_t ucg_font_helvB24_tr[2979];
extern const ucg_fntpgm_uint8_t ucg_font_helvR08_hf[2730];
extern const ucg_fntpgm_uint8_t ucg_font_helvR08_hr[1271];
extern const ucg_fntpgm_uint8_t ucg_font_helvR08_tf[2187];
extern const ucg_fntpgm_uint8_t ucg_font_helvR08_tr[1035];
extern const ucg_fntpgm_uint8_t ucg_font_helvR10_hf[3688];
extern const ucg_fntpgm_uint8_t ucg_font_helvR10_hr[1672];
extern const ucg_fntpgm_uint8_t ucg_font_helvR10_tf[2852];
extern const ucg_fntpgm_uint8_t ucg_font_helvR10_tr[1312];
extern const ucg_fntpgm_uint8_t ucg_font_helvR12_hf[4151];
extern const ucg_fntpgm_uint8_t ucg_font_helvR12_hr[1853];
extern const ucg_fntpgm_uint8_t ucg_font_helvR12_tf[3316];
extern const ucg_fntpgm_uint8_t ucg_font_helvR12_tr[1521];
extern const ucg_fntpgm_uint8_t ucg_font_helvR14_hf[4813];
extern const ucg_fntpgm_uint8_t ucg_font_helvR14_hr[2140];
extern const ucg_fntpgm_uint8_t ucg_font_helvR14_tf[3775];
extern const ucg_fntpgm_uint8_t ucg_font_helvR14_tr[1736];
extern const ucg_fntpgm_uint8_t ucg_font_helvR18_hf[6285];
extern const ucg_fntpgm_uint8_t ucg_font_helvR18_hr[2856];
extern const ucg_fntpgm_uint8_t ucg_font_helvR18_tf[4895];
extern const ucg_fntpgm_uint8_t ucg_font_helvR18_tr[2233];
extern const ucg_fntpgm_uint8_t ucg_font_helvR24_hf[8895];
extern const ucg_fntpgm_uint8_t ucg_font_helvR24_hn[635];
extern const ucg_fntpgm_uint8_t ucg_font_helvR24_hr[4087];
extern const ucg_fntpgm_uint8_t ucg_font_helvR24_tf[6904];
extern const ucg_fntpgm_uint8_t ucg_font_helvR24_tn[486];
extern const ucg_fntpgm_uint8_t ucg_font_helvR24_tr[3166];
extern const ucg_fntpgm_uint8_t ucg_font_lucasfont_alternate_hf[2187];
extern const ucg_fntpgm_uint8_t ucg_font_lucasfont_alternate_hn[218];
extern const ucg_fntpgm_uint8_t ucg_font_lucasfont_alternate_hr[1107];
extern const ucg_fntpgm_uint8_t ucg_font_lucasfont_alternate_tf[1749];
extern const ucg_fntpgm_uint8_t ucg_font_lucasfont_alternate_tn[185];
extern const ucg_fntpgm_uint8_t ucg_font_lucasfont_alternate_tr[902];
extern const ucg_fntpgm_uint8_t ucg_font_m2icon_5[223];
extern const ucg_fntpgm_uint8_t ucg_font_m2icon_7[280];
extern const ucg_fntpgm_uint8_t ucg_font_m2icon_9[345];
extern const ucg_fntpgm_uint8_t ucg_font_micro_mf[776];
extern const ucg_fntpgm_uint8_t ucg_font_micro_tf[705];
extern const ucg_fntpgm_uint8_t ucg_font_ncenB08_hf[2894];
extern const ucg_fntpgm_uint8_t ucg_font_ncenB08_hr[1349];
extern const ucg_fntpgm_uint8_t ucg_font_ncenB08_tf[2510];
extern const ucg_fntpgm_uint8_t ucg_font_ncenB08_tr[1184];
extern const ucg_fntpgm_uint8_t ucg_font_ncenB10_hf[4053];
extern const ucg_fntpgm_uint8_t ucg_font_ncenB10_hr[1842];
extern const ucg_fntpgm_uint8_t ucg_font_ncenB10_tf[3477];
extern const ucg_fntpgm_uint8_t ucg_font_ncenB10_tr[1611];
extern const ucg_fntpgm_uint8_t ucg_font_ncenB12_hf[4484];
extern const ucg_fntpgm_uint8_t ucg_font_ncenB12_hr[2104];
extern const ucg_fntpgm_uint8_t ucg_font_ncenB12_tf[3945];
extern const ucg_fntpgm_uint8_t ucg_font_ncenB12_tr[1830];
extern const ucg_fntpgm_uint8_t ucg_font_ncenB14_hf[5177];
extern const ucg_fntpgm_uint8_t ucg_font_ncenB14_hr[2386];
extern const ucg_fntpgm_uint8_t ucg_font_ncenB14_tf[4543];
extern const ucg_fntpgm_uint8_t ucg_font_ncenB14_tr[2121];
extern const ucg_fntpgm_uint8_t ucg_font_ncenB18_hf[6458];
extern const ucg_fntpgm_uint8_t ucg_font_ncenB18_hr[2963];
extern const ucg_fntpgm_uint8_t ucg_font_ncenB18_tf[5963];
extern const ucg_fntpgm_uint8_t ucg_font_ncenB18_tr[2737];
extern const ucg_fntpgm_uint8_t ucg_font_ncenB24_hf[9410];
extern const ucg_fntpgm_uint8_t ucg_font_ncenB24_hn[684];
extern const ucg_fntpgm_uint8_t ucg_font_ncenB24_hr[4265];
extern const ucg_fntpgm_uint8_t ucg_font_ncenB24_tf[8407];
extern const ucg_fntpgm_uint8_t ucg_font_ncenB24_tn[541];
extern const ucg_fntpgm_uint8_t ucg_font_ncenB24_tr[3915];
extern const ucg_fntpgm_uint8_t ucg_font_ncenR08_hf[2739];
extern const ucg_fntpgm_uint8_t ucg_font_ncenR08_hr[1277];
extern const ucg_fntpgm_uint8_t ucg_font_ncenR08_tf[2324];
extern const ucg_fntpgm_uint8_t ucg_font_ncenR08_tr[1102];
extern const ucg_fntpgm_uint8_t ucg_font_ncenR10_hf[3957];
extern const ucg_fntpgm_uint8_t ucg_font_ncenR10_hr[1817];
extern const ucg_fntpgm_uint8_t ucg_font_ncenR10_tf[3341];
extern const ucg_fntpgm_uint8_t ucg_font_ncenR10_tr[1577];
extern const ucg_fntpgm_uint8_t ucg_font_ncenR12_hf[4243];
extern const ucg_fntpgm_uint8_t ucg_font_ncenR12_hr[1960];
extern const ucg_fntpgm_uint8_t ucg_font_ncenR12_tf[3748];
extern const ucg_fntpgm_uint8_t ucg_font_ncenR12_tr[1754];
extern const ucg_fntpgm_uint8_t ucg_font_ncenR14_hf[4917];
extern const ucg_fntpgm_uint8_t ucg_font_ncenR14_hr[2253];
extern const ucg_fntpgm_uint8_t ucg_font_ncenR14_tf[4431];
extern const ucg_fntpgm_uint8_t ucg_font_ncenR14_tr[2067];
extern const ucg_fntpgm_uint8_t ucg_font_ncenR18_hf[6358];
extern const ucg_fntpgm_uint8_t ucg_font_ncenR18_hr[2950];
extern const ucg_fntpgm_uint8_t ucg_font_ncenR18_tf[5620];
extern const ucg_fntpgm_uint8_t ucg_font_ncenR18_tr[2600];
extern const ucg_fntpgm_uint8_t ucg_font_ncenR24_hf[9116];
extern const ucg_fntpgm_uint8_t ucg_font_ncenR24_hn[658];
extern const ucg_fntpgm_uint8_t ucg_font_ncenR24_hr[4207];
extern const ucg_fntpgm_uint8_t ucg_font_ncenR24_tf[8031];
extern const ucg_fntpgm_uint8_t ucg_font_ncenR24_tn[529];
extern const ucg_fntpgm_uint8_t ucg_font_ncenR24_tr[3736];
extern const ucg_fntpgm_uint8_t ucg_font_orgv01_hf[2227];
extern const ucg_fntpgm_uint8_t ucg_font_orgv01_hn[176];
extern const ucg_fntpgm_uint8_t ucg_font_orgv01_hr[915];
extern const ucg_fntpgm_uint8_t ucg_font_orgv01_tf[1597];
extern const ucg_fntpgm_uint8_t ucg_font_orgv01_tn[136];
extern const ucg_fntpgm_uint8_t ucg_font_orgv01_tr[739];
extern const ucg_fntpgm_uint8_t ucg_font_p01type_hf[902];
extern const ucg_fntpgm_uint8_t ucg_font_p01type_hn[158];
extern const ucg_fntpgm_uint8_t ucg_font_p01type_hr[808];
extern const ucg_fntpgm_uint8_t ucg_font_p01type_tf[759];
extern const ucg_fntpgm_uint8_t ucg_font_p01type_tn[136];
extern const ucg_fntpgm_uint8_t ucg_font_p01type_tr[682];
extern const ucg_fntpgm_uint8_t ucg_font_pixelle_micro_hf[875];
extern const ucg_fntpgm_uint8_t ucg_font_pixelle_micro_hn[165];
extern const ucg_fntpgm_uint8_t ucg_font_pixelle_micro_hr[875];
extern const ucg_fntpgm_uint8_t ucg_font_pixelle_micro_tf[704];
extern const ucg_fntpgm_uint8_t ucg_font_pixelle_micro_tn[137];
extern const ucg_fntpgm_uint8_t ucg_font_pixelle_micro_tr[704];
extern const ucg_fntpgm_uint8_t ucg_font_profont10_8f[3030];
extern const ucg_fntpgm_uint8_t ucg_font_profont10_8r[1296];
extern const ucg_fntpgm_uint8_t ucg_font_profont10_mf[2478];
extern const ucg_fntpgm_uint8_t ucg_font_profont10_mr[1037];
extern const ucg_fntpgm_uint8_t ucg_font_profont11_8f[3070];
extern const ucg_fntpgm_uint8_t ucg_font_profont11_8r[1292];
extern const ucg_fntpgm_uint8_t ucg_font_profont11_mf[2837];
extern const ucg_fntpgm_uint8_t ucg_font_profont11_mr[1195];
extern const ucg_fntpgm_uint8_t ucg_font_profont12_8f[3125];
extern const ucg_fntpgm_uint8_t ucg_font_profont12_8r[1304];
extern const ucg_fntpgm_uint8_t ucg_font_profont12_mf[2920];
extern const ucg_fntpgm_uint8_t ucg_font_profont12_mr[1204];
extern const ucg_fntpgm_uint8_t ucg_font_profont15_8f[3778];
extern const ucg_fntpgm_uint8_t ucg_font_profont15_8r[1590];
extern const ucg_fntpgm_uint8_t ucg_font_profont15_mf[3464];
extern const ucg_fntpgm_uint8_t ucg_font_profont15_mr[1452];
extern const ucg_fntpgm_uint8_t ucg_font_profont17_8f[5109];
extern const ucg_fntpgm_uint8_t ucg_font_profont17_8r[2104];
extern const ucg_fntpgm_uint8_t ucg_font_profont17_mf[4648];
extern const ucg_fntpgm_uint8_t ucg_font_profont17_mr[1913];
extern const ucg_fntpgm_uint8_t ucg_font_profont22_8f[5312];
extern const ucg_fntpgm_uint8_t ucg_font_profont22_8n[421];
extern const ucg_fntpgm_uint8_t ucg_font_profont22_8r[2158];
extern const ucg_fntpgm_uint8_t ucg_font_profont22_mf[5517];
extern const ucg_fntpgm_uint8_t ucg_font_profont22_mn[425];
extern const ucg_fntpgm_uint8_t ucg_font_profont22_mr[2288];
extern const ucg_fntpgm_uint8_t ucg_font_profont29_8f[7335];
extern const ucg_fntpgm_uint8_t ucg_font_profont29_8n[532];
extern const ucg_fntpgm_uint8_t ucg_font_profont29_8r[3017];
extern const ucg_fntpgm_uint8_t ucg_font_profont29_mf[7202];
extern const ucg_fntpgm_uint8_t ucg_font_profont29_mn[514];
extern const ucg_fntpgm_uint8_t ucg_font_profont29_mr[2994];
extern const ucg_fntpgm_uint8_t ucg_font_robot_de_niro_hf[1224];
extern const ucg_fntpgm_uint8_t ucg_font_robot_de_niro_hn[172];
extern const ucg_fntpgm_uint8_t ucg_font_robot_de_niro_hr[980];
extern const ucg_fntpgm_uint8_t ucg_font_robot_de_niro_tf[970];
extern const ucg_fntpgm_uint8_t ucg_font_robot_de_niro_tn[142];
extern const ucg_fntpgm_uint8_t ucg_font_robot_de_niro_tr[757];
extern const ucg_fntpgm_uint8_t ucg_font_symb08_tf[2116];
extern const ucg_fntpgm_uint8_t ucg_font_symb08_tr[1072];
extern const ucg_fntpgm_uint8_t ucg_font_symb10_tf[2785];
extern const ucg_fntpgm_uint8_t ucg_font_symb10_tr[1424];
extern const ucg_fntpgm_uint8_t ucg_font_symb12_tf[3393];
extern const ucg_fntpgm_uint8_t ucg_font_symb12_tr[1741];
extern const ucg_fntpgm_uint8_t ucg_font_symb14_tf[3732];
extern const ucg_fntpgm_uint8_t ucg_font_symb14_tr[1947];
extern const ucg_fntpgm_uint8_t ucg_font_symb18_tf[4703];
extern const ucg_fntpgm_uint8_t ucg_font_symb18_tr[2509];
extern const ucg_fntpgm_uint8_t ucg_font_symb24_tf[6986];
extern const ucg_fntpgm_uint8_t ucg_font_symb24_tr[3690];
extern const ucg_fntpgm_uint8_t ucg_font_timB08_hf[2697];
extern const ucg_fntpgm_uint8_t ucg_font_timB08_hr[1264];
extern const ucg_fntpgm_uint8_t ucg_font_timB08_tf[2300];
extern const ucg_fntpgm_uint8_t ucg_font_timB08_tr[1073];
extern const ucg_fntpgm_uint8_t ucg_font_timB10_hf[3671];
extern const ucg_fntpgm_uint8_t ucg_font_timB10_hr[1674];
extern const ucg_fntpgm_uint8_t ucg_font_timB10_tf[3043];
extern const ucg_fntpgm_uint8_t ucg_font_timB10_tr[1403];
extern const ucg_fntpgm_uint8_t ucg_font_timB12_hf[4091];
extern const ucg_fntpgm_uint8_t ucg_font_timB12_hr[1885];
extern const ucg_fntpgm_uint8_t ucg_font_timB12_tf[3467];
extern const ucg_fntpgm_uint8_t ucg_font_timB12_tr[1620];
extern const ucg_fntpgm_uint8_t ucg_font_timB14_hf[4646];
extern const ucg_fntpgm_uint8_t ucg_font_timB14_hr[2099];
extern const ucg_fntpgm_uint8_t ucg_font_timB14_tf[3981];
extern const ucg_fntpgm_uint8_t ucg_font_timB14_tr[1848];
extern const ucg_fntpgm_uint8_t ucg_font_timB18_hf[5999];
extern const ucg_fntpgm_uint8_t ucg_font_timB18_hr[2752];
extern const ucg_fntpgm_uint8_t ucg_font_timB18_tf[5413];
extern const ucg_fntpgm_uint8_t ucg_font_timB18_tr[2519];
extern const ucg_fntpgm_uint8_t ucg_font_timB24_hf[8548];
extern const ucg_fntpgm_uint8_t ucg_font_timB24_hn[586];
extern const ucg_fntpgm_uint8_t ucg_font_timB24_hr[3905];
extern const ucg_fntpgm_uint8_t ucg_font_timB24_tf[7549];
extern const ucg_fntpgm_uint8_t ucg_font_timB24_tn[491];
extern const ucg_fntpgm_uint8_t ucg_font_timB24_tr[3439];
extern const ucg_fntpgm_uint8_t ucg_font_timR08_hf[2566];
extern const ucg_fntpgm_uint8_t ucg_font_timR08_hr[1191];
extern const ucg_fntpgm_uint8_t ucg_font_timR08_tf[2166];
extern const ucg_fntpgm_uint8_t ucg_font_timR08_tr[1026];
extern const ucg_fntpgm_uint8_t ucg_font_timR10_hf[3470];
extern const ucg_fntpgm_uint8_t ucg_font_timR10_hr[1583];
extern const ucg_fntpgm_uint8_t ucg_font_timR10_tf[2970];
extern const ucg_fntpgm_uint8_t ucg_font_timR10_tr[1365];
extern const ucg_fntpgm_uint8_t ucg_font_timR12_hf[3997];
extern const ucg_fntpgm_uint8_t ucg_font_timR12_hr[1832];
extern const ucg_fntpgm_uint8_t ucg_font_timR12_tf[3452];
extern const ucg_fntpgm_uint8_t ucg_font_timR12_tr[1586];
extern const ucg_fntpgm_uint8_t ucg_font_timR14_hf[4528];
extern const ucg_fntpgm_uint8_t ucg_font_timR14_hr[2077];
extern const ucg_fntpgm_uint8_t ucg_font_timR14_tf[4023];
extern const ucg_fntpgm_uint8_t ucg_font_timR14_tr[1868];
extern const ucg_fntpgm_uint8_t ucg_font_timR18_hf[5896];
extern const ucg_fntpgm_uint8_t ucg_font_timR18_hr[2716];
extern const ucg_fntpgm_uint8_t ucg_font_timR18_tf[5175];
extern const ucg_fntpgm_uint8_t ucg_font_timR18_tr[2407];
extern const ucg_fntpgm_uint8_t ucg_font_timR24_hf[8423];
extern const ucg_fntpgm_uint8_t ucg_font_timR24_hn[563];
extern const ucg_fntpgm_uint8_t ucg_font_timR24_hr[3829];
extern const ucg_fntpgm_uint8_t ucg_font_timR24_tf[7356];
extern const ucg_fntpgm_uint8_t ucg_font_timR24_tn[476];
extern const ucg_fntpgm_uint8_t ucg_font_timR24_tr[3352];
extern const ucg_fntpgm_uint8_t ucg_font_tpssb_hf[2923];
extern const ucg_fntpgm_uint8_t ucg_font_tpssb_hn[246];
extern const ucg_fntpgm_uint8_t ucg_font_tpssb_hr[1477];
extern const ucg_fntpgm_uint8_t ucg_font_tpssb_tf[2093];
extern const ucg_fntpgm_uint8_t ucg_font_tpssb_tn[203];
extern const ucg_fntpgm_uint8_t ucg_font_tpssb_tr[1080];
extern const ucg_fntpgm_uint8_t ucg_font_tpss_hf[2832];
extern const ucg_fntpgm_uint8_t ucg_font_tpss_hn[232];
extern const ucg_fntpgm_uint8_t ucg_font_tpss_hr[1430];
extern const ucg_fntpgm_uint8_t ucg_font_tpss_tf[1997];
extern const ucg_fntpgm_uint8_t ucg_font_tpss_tn[194];
extern const ucg_fntpgm_uint8_t ucg_font_tpss_tr[1033];
extern const ucg_fntpgm_uint8_t ucg_font_trixel_square_hf[992];
extern const ucg_fntpgm_uint8_t ucg_font_trixel_square_hn[160];
extern const ucg_fntpgm_uint8_t ucg_font_trixel_square_hr[872];
extern const ucg_fntpgm_uint8_t ucg_font_trixel_square_tf[766];
extern const ucg_fntpgm_uint8_t ucg_font_trixel_square_tn[133];
extern const ucg_fntpgm_uint8_t ucg_font_trixel_square_tr[678];
extern const ucg_fntpgm_uint8_t ucg_font_u8glib_4_hf[2085];
extern const ucg_fntpgm_uint8_t ucg_font_u8glib_4_hr[804];
extern const ucg_fntpgm_uint8_t ucg_font_u8glib_4_tf[1830];
extern const ucg_fntpgm_uint8_t ucg_font_u8glib_4_tr[673];
extern const ucg_fntpgm_uint8_t ucg_font_unifont_0_8[4406];
extern const ucg_fntpgm_uint8_t ucg_font_unifont_12_13[3782];
extern const ucg_fntpgm_uint8_t ucg_font_unifont_18_19[6756];
extern const ucg_fntpgm_uint8_t ucg_font_unifont_2_3[3977];
extern const ucg_fntpgm_uint8_t ucg_font_unifont_4_5[3417];
extern const ucg_fntpgm_uint8_t ucg_font_unifont_67_75[3619];
extern const ucg_fntpgm_uint8_t ucg_font_unifont_72_73[8411];
extern const ucg_fntpgm_uint8_t ucg_font_unifont_78_79[6737];
extern const ucg_fntpgm_uint8_t ucg_font_unifont_8_9[4092];
extern const ucg_fntpgm_uint8_t ucg_font_unifont_mf[4216];
extern const ucg_fntpgm_uint8_t ucg_font_unifont_mr[1680];
extern const ucg_fntpgm_uint8_t ucg_font_unifont_tf[2656];
extern const ucg_fntpgm_uint8_t ucg_font_unifont_tr[1178];
extern const ucg_fntpgm_uint8_t ucg_font_fub11_hf[4199];
extern const ucg_fntpgm_uint8_t ucg_font_fub11_hn[323];
extern const ucg_fntpgm_uint8_t ucg_font_fub11_hr[1921];
extern const ucg_fntpgm_uint8_t ucg_font_fub11_tf[3235];
extern const ucg_fntpgm_uint8_t ucg_font_fub11_tn[262];
extern const ucg_fntpgm_uint8_t ucg_font_fub11_tr[1515];
extern const ucg_fntpgm_uint8_t ucg_font_fub14_hf[5176];
extern const ucg_fntpgm_uint8_t ucg_font_fub14_hn[392];
extern const ucg_fntpgm_uint8_t ucg_font_fub14_hr[2396];
extern const ucg_fntpgm_uint8_t ucg_font_fub14_tf[4049];
extern const ucg_fntpgm_uint8_t ucg_font_fub14_tn[321];
extern const ucg_fntpgm_uint8_t ucg_font_fub14_tr[1887];
extern const ucg_fntpgm_uint8_t ucg_font_fub17_hf[6093];
extern const ucg_fntpgm_uint8_t ucg_font_fub17_hn[462];
extern const ucg_fntpgm_uint8_t ucg_font_fub17_hr[2809];
extern const ucg_fntpgm_uint8_t ucg_font_fub17_tf[4710];
extern const ucg_fntpgm_uint8_t ucg_font_fub17_tn[368];
extern const ucg_fntpgm_uint8_t ucg_font_fub17_tr[2179];
extern const ucg_fntpgm_uint8_t ucg_font_fub20_hf[7147];
extern const ucg_fntpgm_uint8_t ucg_font_fub20_hn[526];
extern const ucg_fntpgm_uint8_t ucg_font_fub20_hr[3257];
extern const ucg_fntpgm_uint8_t ucg_font_fub20_tf[5434];
extern const ucg_fntpgm_uint8_t ucg_font_fub20_tn[435];
extern const ucg_fntpgm_uint8_t ucg_font_fub20_tr[2510];
extern const ucg_fntpgm_uint8_t ucg_font_fub25_hf[9409];
extern const ucg_fntpgm_uint8_t ucg_font_fub25_hn[670];
extern const ucg_fntpgm_uint8_t ucg_font_fub25_hr[4308];
extern const ucg_fntpgm_uint8_t ucg_font_fub25_tf[7272];
extern const ucg_fntpgm_uint8_t ucg_font_fub25_tn[537];
extern const ucg_fntpgm_uint8_t ucg_font_fub25_tr[3330];
extern const ucg_fntpgm_uint8_t ucg_font_fub30_hf[11193];
extern const ucg_fntpgm_uint8_t ucg_font_fub30_hn[811];
extern const ucg_fntpgm_uint8_t ucg_font_fub30_hr[5172];
extern const ucg_fntpgm_uint8_t ucg_font_fub30_tf[8846];
extern const ucg_fntpgm_uint8_t ucg_font_fub30_tn[661];
extern const ucg_fntpgm_uint8_t ucg_font_fub30_tr[4076];
extern const ucg_fntpgm_uint8_t ucg_font_fub35_hf[13290];
extern const ucg_fntpgm_uint8_t ucg_font_fub35_hn[936];
extern const ucg_fntpgm_uint8_t ucg_font_fub35_hr[6008];
extern const ucg_fntpgm_uint8_t ucg_font_fub35_tf[10279];
extern const ucg_fntpgm_uint8_t ucg_font_fub35_tn[783];
extern const ucg_fntpgm_uint8_t ucg_font_fub35_tr[4720];
extern const ucg_fntpgm_uint8_t ucg_font_fub42_hf[16434];
extern const ucg_fntpgm_uint8_t ucg_font_fub42_hn[1116];
extern const ucg_fntpgm_uint8_t ucg_font_fub42_hr[7372];
extern const ucg_fntpgm_uint8_t ucg_font_fub42_tf[12654];
extern const ucg_fntpgm_uint8_t ucg_font_fub42_tn[979];
extern const ucg_fntpgm_uint8_t ucg_font_fub42_tr[5749];
extern const ucg_fntpgm_uint8_t ucg_font_fub49_hn[1376];
extern const ucg_fntpgm_uint8_t ucg_font_fub49_tn[1125];
extern const ucg_fntpgm_uint8_t ucg_font_fur11_hf[3934];
extern const ucg_fntpgm_uint8_t ucg_font_fur11_hn[310];
extern const ucg_fntpgm_uint8_t ucg_font_fur11_hr[1852];
extern const ucg_fntpgm_uint8_t ucg_font_fur11_tf[3093];
extern const ucg_fntpgm_uint8_t ucg_font_fur11_tn[252];
extern const ucg_fntpgm_uint8_t ucg_font_fur11_tr[1437];
extern const ucg_fntpgm_uint8_t ucg_font_fur14_hf[4864];
extern const ucg_fntpgm_uint8_t ucg_font_fur14_hn[379];
extern const ucg_fntpgm_uint8_t ucg_font_fur14_hr[2265];
extern const ucg_fntpgm_uint8_t ucg_font_fur14_tf[3907];
extern const ucg_fntpgm_uint8_t ucg_font_fur14_tn[320];
extern const ucg_fntpgm_uint8_t ucg_font_fur14_tr[1833];
extern const ucg_fntpgm_uint8_t ucg_font_fur17_hf[5924];
extern const ucg_fntpgm_uint8_t ucg_font_fur17_hn[448];
extern const ucg_fntpgm_uint8_t ucg_font_fur17_hr[2742];
extern const ucg_fntpgm_uint8_t ucg_font_fur17_tf[4606];
extern const ucg_fntpgm_uint8_t ucg_font_fur17_tn[369];
extern const ucg_fntpgm_uint8_t ucg_font_fur17_tr[2152];
extern const ucg_fntpgm_uint8_t ucg_font_fur20_hf[7225];
extern const ucg_fntpgm_uint8_t ucg_font_fur20_hn[516];
extern const ucg_fntpgm_uint8_t ucg_font_fur20_hr[3295];
extern const ucg_fntpgm_uint8_t ucg_font_fur20_tf[5489];
extern const ucg_fntpgm_uint8_t ucg_font_fur20_tn[437];
extern const ucg_fntpgm_uint8_t ucg_font_fur20_tr[2542];
extern const ucg_fntpgm_uint8_t ucg_font_fur25_hf[9374];
extern const ucg_fntpgm_uint8_t ucg_font_fur25_hn[696];
extern const ucg_fntpgm_uint8_t ucg_font_fur25_hr[4307];
extern const ucg_fntpgm_uint8_t ucg_font_fur25_tf[7407];
extern const ucg_fntpgm_uint8_t ucg_font_fur25_tn[549];
extern const ucg_fntpgm_uint8_t ucg_font_fur25_tr[3364];
extern const ucg_fntpgm_uint8_t ucg_font_fur30_hf[11167];
extern const ucg_fntpgm_uint8_t ucg_font_fur30_hn[817];
extern const ucg_fntpgm_uint8_t ucg_font_fur30_hr[5119];
extern const ucg_fntpgm_uint8_t ucg_font_fur30_tf[8669];
extern const ucg_fntpgm_uint8_t ucg_font_fur30_tn[646];
extern const ucg_fntpgm_uint8_t ucg_font_fur30_tr[3964];
extern const ucg_fntpgm_uint8_t ucg_font_fur35_hf[13218];
extern const ucg_fntpgm_uint8_t ucg_font_fur35_hn[961];
extern const ucg_fntpgm_uint8_t ucg_font_fur35_hr[5992];
extern const ucg_fntpgm_uint8_t ucg_font_fur35_tf[10285];
extern const ucg_fntpgm_uint8_t ucg_font_fur35_tn[762];
extern const ucg_fntpgm_uint8_t ucg_font_fur35_tr[4724];
extern const ucg_fntpgm_uint8_t ucg_font_fur42_hf[16705];
extern const ucg_fntpgm_uint8_t ucg_font_fur42_hn[1130];
extern const ucg_fntpgm_uint8_t ucg_font_fur42_hr[7615];
extern const ucg_fntpgm_uint8_t ucg_font_fur42_tf[13020];
extern const ucg_fntpgm_uint8_t ucg_font_fur42_tn[988];
extern const ucg_fntpgm_uint8_t ucg_font_fur42_tr[5936];
extern const ucg_fntpgm_uint8_t ucg_font_fur49_hn[1430];
extern const ucg_fntpgm_uint8_t ucg_font_fur49_tn[1150];
extern const ucg_fntpgm_uint8_t ucg_font_fur49_tr[7302];
extern const ucg_fntpgm_uint8_t ucg_font_inb16_mf[5557];
extern const ucg_fntpgm_uint8_t ucg_font_inb16_mn[438];
extern const ucg_fntpgm_uint8_t ucg_font_inb16_mr[2518];
extern const ucg_fntpgm_uint8_t ucg_font_inb16_tf[4474];
extern const ucg_fntpgm_uint8_t ucg_font_inb16_tn[384];
extern const ucg_fntpgm_uint8_t ucg_font_inb16_tr[2066];
extern const ucg_fntpgm_uint8_t ucg_font_inb19_mf[6518];
extern const ucg_fntpgm_uint8_t ucg_font_inb19_mn[494];
extern const ucg_fntpgm_uint8_t ucg_font_inb19_mr[2917];
extern const ucg_fntpgm_uint8_t ucg_font_inb19_tf[5128];
extern const ucg_fntpgm_uint8_t ucg_font_inb19_tn[424];
extern const ucg_fntpgm_uint8_t ucg_font_inb19_tr[2323];
extern const ucg_fntpgm_uint8_t ucg_font_inb21_mf[7862];
extern const ucg_fntpgm_uint8_t ucg_font_inb21_mn[578];
extern const ucg_fntpgm_uint8_t ucg_font_inb21_mr[3499];
extern const ucg_fntpgm_uint8_t ucg_font_inb21_tf[5951];
extern const ucg_fntpgm_uint8_t ucg_font_inb21_tn[470];
extern const ucg_fntpgm_uint8_t ucg_font_inb21_tr[2709];
extern const ucg_fntpgm_uint8_t ucg_font_inb24_mf[8821];
extern const ucg_fntpgm_uint8_t ucg_font_inb24_mn[633];
extern const ucg_fntpgm_uint8_t ucg_font_inb24_mr[3933];
extern const ucg_fntpgm_uint8_t ucg_font_inb24_tf[6569];
extern const ucg_fntpgm_uint8_t ucg_font_inb24_tn[513];
extern const ucg_fntpgm_uint8_t ucg_font_inb24_tr[2939];
extern const ucg_fntpgm_uint8_t ucg_font_inb27_mf[9792];
extern const ucg_fntpgm_uint8_t ucg_font_inb27_mn[737];
extern const ucg_fntpgm_uint8_t ucg_font_inb27_mr[4408];
extern const ucg_fntpgm_uint8_t ucg_font_inb27_tf[7517];
extern const ucg_fntpgm_uint8_t ucg_font_inb27_tn[568];
extern const ucg_fntpgm_uint8_t ucg_font_inb27_tr[3381];
extern const ucg_fntpgm_uint8_t ucg_font_inb30_mf[10917];
extern const ucg_fntpgm_uint8_t ucg_font_inb30_mn[800];
extern const ucg_fntpgm_uint8_t ucg_font_inb30_mr[4925];
extern const ucg_fntpgm_uint8_t ucg_font_inb30_tf[8725];
extern const ucg_fntpgm_uint8_t ucg_font_inb30_tn[691];
extern const ucg_fntpgm_uint8_t ucg_font_inb30_tr[3933];
extern const ucg_fntpgm_uint8_t ucg_font_inb33_mf[11974];
extern const ucg_fntpgm_uint8_t ucg_font_inb33_mn[875];
extern const ucg_fntpgm_uint8_t ucg_font_inb33_mr[5343];
extern const ucg_fntpgm_uint8_t ucg_font_inb33_tf[9655];
extern const ucg_fntpgm_uint8_t ucg_font_inb33_tn[753];
extern const ucg_fntpgm_uint8_t ucg_font_inb33_tr[4330];
extern const ucg_fntpgm_uint8_t ucg_font_inb38_mf[13706];
extern const ucg_fntpgm_uint8_t ucg_font_inb38_mn[982];
extern const ucg_fntpgm_uint8_t ucg_font_inb38_mr[6058];
extern const ucg_fntpgm_uint8_t ucg_font_inb38_tf[10970];
extern const ucg_fntpgm_uint8_t ucg_font_inb38_tn[861];
extern const ucg_fntpgm_uint8_t ucg_font_inb38_tr[4915];
extern const ucg_fntpgm_uint8_t ucg_font_inb42_mf[15504];
extern const ucg_fntpgm_uint8_t ucg_font_inb42_mn[1077];
extern const ucg_fntpgm_uint8_t ucg_font_inb42_mr[6737];
extern const ucg_fntpgm_uint8_t ucg_font_inb42_tf[12226];
extern const ucg_fntpgm_uint8_t ucg_font_inb42_tn[948];
extern const ucg_fntpgm_uint8_t ucg_font_inb42_tr[5457];
extern const ucg_fntpgm_uint8_t ucg_font_inb46_mf[17383];
extern const ucg_fntpgm_uint8_t ucg_font_inb46_mn[1235];
extern const ucg_fntpgm_uint8_t ucg_font_inb46_mr[7756];
extern const ucg_fntpgm_uint8_t ucg_font_inb46_tf[13826];
extern const ucg_fntpgm_uint8_t ucg_font_inb46_tn[1029];
extern const ucg_fntpgm_uint8_t ucg_font_inb46_tr[6122];
extern const ucg_fntpgm_uint8_t ucg_font_inb49_mf[18618];
extern const ucg_fntpgm_uint8_t ucg_font_inb49_mn[1320];
extern const ucg_fntpgm_uint8_t ucg_font_inb49_mr[8345];
extern const ucg_fntpgm_uint8_t ucg_font_inb49_tf[14289];
extern const ucg_fntpgm_uint8_t ucg_font_inb49_tn[1088];
extern const ucg_fntpgm_uint8_t ucg_font_inb49_tr[6338];
extern const ucg_fntpgm_uint8_t ucg_font_inb53_mf[20771];
extern const ucg_fntpgm_uint8_t ucg_font_inb53_mn[1497];
extern const ucg_fntpgm_uint8_t ucg_font_inb53_mr[9294];
extern const ucg_fntpgm_uint8_t ucg_font_inb53_tf[16648];
extern const ucg_fntpgm_uint8_t ucg_font_inb53_tn[1207];
extern const ucg_fntpgm_uint8_t ucg_font_inb53_tr[7394];
extern const ucg_fntpgm_uint8_t ucg_font_inb57_mf[22780];
extern const ucg_fntpgm_uint8_t ucg_font_inb57_mn[1615];
extern const ucg_fntpgm_uint8_t ucg_font_inb57_mr[10148];
extern const ucg_fntpgm_uint8_t ucg_font_inb57_tf[19079];
extern const ucg_fntpgm_uint8_t ucg_font_inb57_tn[1403];
extern const ucg_fntpgm_uint8_t ucg_font_inb57_tr[8556];
extern const ucg_fntpgm_uint8_t ucg_font_inb63_mn[1721];
extern const ucg_fntpgm_uint8_t ucg_font_inb63_tf[21278];
extern const ucg_fntpgm_uint8_t ucg_font_inb63_tn[1541];
extern const ucg_fntpgm_uint8_t ucg_font_inb63_tr[9494];
extern const ucg_fntpgm_uint8_t ucg_font_inr16_mf[5454];
extern const ucg_fntpgm_uint8_t ucg_font_inr16_mn[420];
extern const ucg_fntpgm_uint8_t ucg_font_inr16_mr[2488];
extern const ucg_fntpgm_uint8_t ucg_font_inr16_tf[4231];
extern const ucg_fntpgm_uint8_t ucg_font_inr16_tn[355];
extern const ucg_fntpgm_uint8_t ucg_font_inr16_tr[1953];
extern const ucg_fntpgm_uint8_t ucg_font_inr19_mf[6471];
extern const ucg_fntpgm_uint8_t ucg_font_inr19_mn[496];
extern const ucg_fntpgm_uint8_t ucg_font_inr19_mr[2912];
extern const ucg_fntpgm_uint8_t ucg_font_inr19_tf[5093];
extern const ucg_fntpgm_uint8_t ucg_font_inr19_tn[419];
extern const ucg_fntpgm_uint8_t ucg_font_inr19_tr[2316];
extern const ucg_fntpgm_uint8_t ucg_font_inr21_mf[7735];
extern const ucg_fntpgm_uint8_t ucg_font_inr21_mn[577];
extern const ucg_fntpgm_uint8_t ucg_font_inr21_mr[3498];
extern const ucg_fntpgm_uint8_t ucg_font_inr21_tf[5795];
extern const ucg_fntpgm_uint8_t ucg_font_inr21_tn[470];
extern const ucg_fntpgm_uint8_t ucg_font_inr21_tr[2624];
extern const ucg_fntpgm_uint8_t ucg_font_inr24_mf[8591];
extern const ucg_fntpgm_uint8_t ucg_font_inr24_mn[625];
extern const ucg_fntpgm_uint8_t ucg_font_inr24_mr[3850];
extern const ucg_fntpgm_uint8_t ucg_font_inr24_tf[6432];
extern const ucg_fntpgm_uint8_t ucg_font_inr24_tn[509];
extern const ucg_fntpgm_uint8_t ucg_font_inr24_tr[2902];
extern const ucg_fntpgm_uint8_t ucg_font_inr27_mf[9606];
extern const ucg_fntpgm_uint8_t ucg_font_inr27_mn[686];
extern const ucg_fntpgm_uint8_t ucg_font_inr27_mr[4295];
extern const ucg_fntpgm_uint8_t ucg_font_inr27_tf[7436];
extern const ucg_fntpgm_uint8_t ucg_font_inr27_tn[560];
extern const ucg_fntpgm_uint8_t ucg_font_inr27_tr[3390];
extern const ucg_fntpgm_uint8_t ucg_font_inr30_mf[10709];
extern const ucg_fntpgm_uint8_t ucg_font_inr30_mn[780];
extern const ucg_fntpgm_uint8_t ucg_font_inr30_mr[4828];
extern const ucg_fntpgm_uint8_t ucg_font_inr30_tf[8821];
extern const ucg_fntpgm_uint8_t ucg_font_inr30_tn[682];
extern const ucg_fntpgm_uint8_t ucg_font_inr30_tr[3981];
extern const ucg_fntpgm_uint8_t ucg_font_inr33_mf[11537];
extern const ucg_fntpgm_uint8_t ucg_font_inr33_mn[842];
extern const ucg_fntpgm_uint8_t ucg_font_inr33_mr[5140];
extern const ucg_fntpgm_uint8_t ucg_font_inr33_tf[9095];
extern const ucg_fntpgm_uint8_t ucg_font_inr33_tn[716];
extern const ucg_fntpgm_uint8_t ucg_font_inr33_tr[4149];
extern const ucg_fntpgm_uint8_t ucg_font_inr38_mf[13734];
extern const ucg_fntpgm_uint8_t ucg_font_inr38_mn[980];
extern const ucg_fntpgm_uint8_t ucg_font_inr38_mr[6174];
extern const ucg_fntpgm_uint8_t ucg_font_inr38_tf[11055];
extern const ucg_fntpgm_uint8_t ucg_font_inr38_tn[835];
extern const ucg_fntpgm_uint8_t ucg_font_inr38_tr[4974];
extern const ucg_fntpgm_uint8_t ucg_font_inr42_mf[15342];
extern const ucg_fntpgm_uint8_t ucg_font_inr42_mn[1070];
extern const ucg_fntpgm_uint8_t ucg_font_inr42_mr[6763];
extern const ucg_fntpgm_uint8_t ucg_font_inr42_tf[12024];
extern const ucg_fntpgm_uint8_t ucg_font_inr42_tn[919];
extern const ucg_fntpgm_uint8_t ucg_font_inr42_tr[5402];
extern const ucg_fntpgm_uint8_t ucg_font_inr46_mf[17651];
extern const ucg_fntpgm_uint8_t ucg_font_inr46_mn[1257];
extern const ucg_fntpgm_uint8_t ucg_font_inr46_mr[7906];
extern const ucg_fntpgm_uint8_t ucg_font_inr46_tf[13341];
extern const ucg_fntpgm_uint8_t ucg_font_inr46_tn[1017];
extern const ucg_fntpgm_uint8_t ucg_font_inr46_tr[6013];
extern const ucg_fntpgm_uint8_t ucg_font_inr49_mf[18771];
extern const ucg_fntpgm_uint8_t ucg_font_inr49_mn[1298];
extern const ucg_fntpgm_uint8_t ucg_font_inr49_mr[8400];
extern const ucg_fntpgm_uint8_t ucg_font_inr49_tf[14405];
extern const ucg_fntpgm_uint8_t ucg_font_inr49_tn[1076];
extern const ucg_fntpgm_uint8_t ucg_font_inr49_tr[6416];
extern const ucg_fntpgm_uint8_t ucg_font_inr53_mf[20565];
extern const ucg_fntpgm_uint8_t ucg_font_inr53_mn[1448];
extern const ucg_fntpgm_uint8_t ucg_font_inr53_mr[9271];
extern const ucg_fntpgm_uint8_t ucg_font_inr53_tf[16554];
extern const ucg_fntpgm_uint8_t ucg_font_inr53_tn[1185];
extern const ucg_fntpgm_uint8_t ucg_font_inr53_tr[7431];
extern const ucg_fntpgm_uint8_t ucg_font_inr57_mn[1592];
extern const ucg_fntpgm_uint8_t ucg_font_inr57_tf[18257];
extern const ucg_fntpgm_uint8_t ucg_font_inr57_tn[1375];
extern const ucg_fntpgm_uint8_t ucg_font_inr57_tr[8409];
extern const ucg_fntpgm_uint8_t ucg_font_inr62_mn[1698];
extern const ucg_fntpgm_uint8_t ucg_font_inr62_tn[1471];
extern const ucg_fntpgm_uint8_t ucg_font_inr62_tr[8978];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso16_hf[4513];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso16_hn[388];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso16_hr[2284];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso16_tf[3044];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso16_tn[279];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso16_tr[1547];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso18_hf[5338];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso18_hn[430];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso18_hr[2690];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso18_tf[3707];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso18_tn[327];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso18_tr[1948];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso20_hf[5886];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso20_hn[473];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso20_hr[2997];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso20_tf[4134];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso20_tn[374];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso20_tr[2202];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso22_hf[6303];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso22_hn[503];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso22_hr[3192];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso22_tf[4224];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso22_tn[375];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso22_tr[2258];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso24_hf[6556];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso24_hn[544];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso24_hr[3397];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso24_tf[4420];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso24_tn[412];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso24_tr[2387];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso26_hf[7453];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso26_hn[582];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso26_hr[3763];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso26_tf[4902];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso26_tn[430];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso26_tr[2624];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso28_hf[7664];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso28_hn[585];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso28_hr[3882];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso28_tf[4867];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso28_tn[421];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso28_tr[2618];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso30_tf[5344];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso30_tn[474];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso30_tr[2879];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso32_tf[5602];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso32_tn[513];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso32_tr[3031];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso34_tf[6163];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso34_tn[530];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso34_tr[3321];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso38_tf[6806];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso38_tn[605];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso38_tr[3689];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso42_tf[8160];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso42_tn[714];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso42_tr[4554];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso46_tf[8594];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso46_tn[775];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso46_tr[4837];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso50_tf[9656];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso50_tn[847];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso50_tr[5444];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso54_tf[10058];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso54_tn[871];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso54_tr[5659];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso58_tf[10897];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso58_tn[971];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso58_tr[6160];
extern const ucg_fntpgm_uint8_t ucg_font_logisoso62_tn[975];
extern const ucg_fntpgm_uint8_t ucg_font_osb18_hf[6817];
extern const ucg_fntpgm_uint8_t ucg_font_osb18_hn[532];
extern const ucg_fntpgm_uint8_t ucg_font_osb18_hr[3143];
extern const ucg_fntpgm_uint8_t ucg_font_osb18_tf[5811];
extern const ucg_fntpgm_uint8_t ucg_font_osb18_tn[484];
extern const ucg_fntpgm_uint8_t ucg_font_osb18_tr[2719];
extern const ucg_fntpgm_uint8_t ucg_font_osb21_hf[8008];
extern const ucg_fntpgm_uint8_t ucg_font_osb21_hn[601];
extern const ucg_fntpgm_uint8_t ucg_font_osb21_hr[3658];
extern const ucg_fntpgm_uint8_t ucg_font_osb21_tf[6908];
extern const ucg_fntpgm_uint8_t ucg_font_osb21_tn[557];
extern const ucg_fntpgm_uint8_t ucg_font_osb21_tr[3229];
extern const ucg_fntpgm_uint8_t ucg_font_osb26_hf[9809];
extern const ucg_fntpgm_uint8_t ucg_font_osb26_hn[705];
extern const ucg_fntpgm_uint8_t ucg_font_osb26_hr[4459];
extern const ucg_fntpgm_uint8_t ucg_font_osb26_tf[8734];
extern const ucg_fntpgm_uint8_t ucg_font_osb26_tn[664];
extern const ucg_fntpgm_uint8_t ucg_font_osb26_tr[4073];
extern const ucg_fntpgm_uint8_t ucg_font_osb29_hf[11018];
extern const ucg_fntpgm_uint8_t ucg_font_osb29_hn[854];
extern const ucg_fntpgm_uint8_t ucg_font_osb29_hr[5090];
extern const ucg_fntpgm_uint8_t ucg_font_osb29_tf[9920];
extern const ucg_fntpgm_uint8_t ucg_font_osb29_tn[740];
extern const ucg_fntpgm_uint8_t ucg_font_osb29_tr[4591];
extern const ucg_fntpgm_uint8_t ucg_font_osb35_hf[14076];
extern const ucg_fntpgm_uint8_t ucg_font_osb35_hn[1027];
extern const ucg_fntpgm_uint8_t ucg_font_osb35_hr[6442];
extern const ucg_fntpgm_uint8_t ucg_font_osb35_tf[12408];
extern const ucg_fntpgm_uint8_t ucg_font_osb35_tn[968];
extern const ucg_fntpgm_uint8_t ucg_font_osb35_tr[5818];
extern const ucg_fntpgm_uint8_t ucg_font_osb41_hf[16811];
extern const ucg_fntpgm_uint8_t ucg_font_osb41_hn[1190];
extern const ucg_fntpgm_uint8_t ucg_font_osb41_hr[7690];
extern const ucg_fntpgm_uint8_t ucg_font_osb41_tf[15011];
extern const ucg_fntpgm_uint8_t ucg_font_osb41_tn[1155];
extern const ucg_fntpgm_uint8_t ucg_font_osb41_tr[6946];
extern const ucg_fntpgm_uint8_t ucg_font_osr18_hf[6723];
extern const ucg_fntpgm_uint8_t ucg_font_osr18_hn[508];
extern const ucg_fntpgm_uint8_t ucg_font_osr18_hr[3110];
extern const ucg_fntpgm_uint8_t ucg_font_osr18_tf[5935];
extern const ucg_fntpgm_uint8_t ucg_font_osr18_tn[436];
extern const ucg_fntpgm_uint8_t ucg_font_osr18_tr[2792];
extern const ucg_fntpgm_uint8_t ucg_font_osr21_hf[7990];
extern const ucg_fntpgm_uint8_t ucg_font_osr21_hn[586];
extern const ucg_fntpgm_uint8_t ucg_font_osr21_hr[3702];
extern const ucg_fntpgm_uint8_t ucg_font_osr21_tf[6842];
extern const ucg_fntpgm_uint8_t ucg_font_osr21_tn[523];
extern const ucg_fntpgm_uint8_t ucg_font_osr21_tr[3229];
extern const ucg_fntpgm_uint8_t ucg_font_osr26_hf[9863];
extern const ucg_fntpgm_uint8_t ucg_font_osr26_hn[757];
extern const ucg_fntpgm_uint8_t ucg_font_osr26_hr[4551];
extern const ucg_fntpgm_uint8_t ucg_font_osr26_tf[8793];
extern const ucg_fntpgm_uint8_t ucg_font_osr26_tn[633];
extern const ucg_fntpgm_uint8_t ucg_font_osr26_tr[4127];
extern const ucg_fntpgm_uint8_t ucg_font_osr29_hf[11023];
extern const ucg_fntpgm_uint8_t ucg_font_osr29_hn[835];
extern const ucg_fntpgm_uint8_t ucg_font_osr29_hr[5125];
extern const ucg_fntpgm_uint8_t ucg_font_osr29_tf[10211];
extern const ucg_fntpgm_uint8_t ucg_font_osr29_tn[743];
extern const ucg_fntpgm_uint8_t ucg_font_osr29_tr[4753];
extern const ucg_fntpgm_uint8_t ucg_font_osr35_hf[13668];
extern const ucg_fntpgm_uint8_t ucg_font_osr35_hn[997];
extern const ucg_fntpgm_uint8_t ucg_font_osr35_hr[6269];
extern const ucg_fntpgm_uint8_t ucg_font_osr35_tf[12329];
extern const ucg_fntpgm_uint8_t ucg_font_osr35_tn[905];
extern const ucg_fntpgm_uint8_t ucg_font_osr35_tr[5757];
extern const ucg_fntpgm_uint8_t ucg_font_osr41_hf[16644];
extern const ucg_fntpgm_uint8_t ucg_font_osr41_hn[1203];
extern const ucg_fntpgm_uint8_t ucg_font_osr41_hr[7593];
extern const ucg_fntpgm_uint8_t ucg_font_osr41_tf[14889];
extern const ucg_fntpgm_uint8_t ucg_font_osr41_tn[1081];
extern const ucg_fntpgm_uint8_t ucg_font_osr41_tr[6910];

#endif



#ifdef __cplusplus
}
#endif


#endif /* _UCG_H */
