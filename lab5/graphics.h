#ifndef GRAPHICS_H
#define GRAPHICS_H
#include <lcom/lcf.h>
//Standard BIOS Services
#define BIOS_VIDEO_CARD 0x10
#define BIOS_PC_CONF  0x11
#define BIOS_MEMORY_CONF  0x12
#define BIOS_KEYBOARD 0x16



//VBE function call results
#define VBE_CALL_SUCCESS 0x00
#define VBE_ERROR_CALL 0x01
#define VBE_ERROR_NOTSUPPORTED 0x02 // Function is not supported in current HW() configuration
#define VBE_ERROR_INVALID_VM 0x03 //Function is Invalid in current Video Mode


//Graphics Mode                   Color Model =        | Bits per pixel(R:G:B)
#define GM_1024x768   0x105 //    Indexed              | 8
#define GM_640x480    0x110 //    Direct color         | 15((1:)5:5:5)
#define GM_800x600    0x115 //    Direct color         | 24(8:8:8)
#define GM_1280x1024  0x11A //    Direct color         | 16(5:6:5)
#define GM_1152x864   0x14C //    Direct color         | 32((8:)8:8:8)

#define VBE_CALL 0x4F02 // Use with a function [VBE_CALL < 2 | ST ]

//Functions
#define SET_VBE_MODE 0x02 // BX argument [0, 8] -> mode number | [9- 13] -> 0 
//             [14] = (0 - Use windowed frame buffer model; 1 - User linear/flat fame buffer model) 
//             [15] -> (0 - Clear display memory; 1 - don't clear display memory)

#define VBE_FUNCTION_SET_VBE 0x4F02

typedef struct{
  xpm_image_t image;
  char* arr;
} Sprite;



int activateGraphicsMode(uint16_t mode);

int activateTextMode();

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);

int vg_draw_pixel(uint16_t x, uint16_t y, uint32_t color);

/**
 * Draw pixmap
 * 
*/
int draw_pm(xpm_map_t xpm, uint16_t x, uint16_t y);
Sprite readSprite(xpm_map_t xpm);
int drawSprite(uint16_t x, uint16_t y, Sprite sprite);

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);
int (vg_draw_vline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);

/**
 *  Copy double buffer to the video memory 
 */
void applyChanges();

#endif
