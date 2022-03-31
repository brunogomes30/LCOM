#include "graphics.h"
#include <lcom/lcf.h>
#include <machine/int86.h>
#include "xpm.h"
static phys_bytes phys;
static char *video_mem;
static char *double_buffer;
static unsigned x_res;
static unsigned y_res;
static unsigned bitspp; //Bits per pixel

struct minix_mem_range mr;
unsigned int vram_base; /* VRAM's physical addresss */
unsigned int vram_size; /* VRAM's size, but you can use
				    the frame-buffer size, instead */

int activateGraphicsMode(uint16_t mode) {
  struct reg86 r;
  vbe_mode_info_t vbeInfo;
  memset(&r, 0, sizeof(r));
  //Get info of mode
  if (vbe_get_mode_info(mode, &vbeInfo) != OK) {
    printf("Can't get info of selected mode(%d)", mode);
    return 1;
  }
  x_res = vbeInfo.XResolution;
  y_res = vbeInfo.YResolution;
  bitspp = vbeInfo.BitsPerPixel;
  phys = vbeInfo.PhysBasePtr;
  
  //printf("Mode = %d\n", mode);
  r.ax = VBE_FUNCTION_SET_VBE;
  r.bx = BIT(14) | mode;
  r.intno = BIOS_VIDEO_CARD;
  if (sys_int86(&r) != OK) {
    printf("Error activating graphics Mode\n");
    return 1;
  }

  /* Allow memory mapping */
  vram_base = phys;
  vram_size = y_res * x_res * (bitspp / 8);
  mr.mr_base = (phys_bytes) vram_base;
  mr.mr_limit = mr.mr_base + vram_size;
  unsigned ans;
  if (OK != (ans = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
    panic("sys_privctl (ADD_MEM) failed: %d\n", ans);

  /* Map memory */
  video_mem = vm_map_phys(SELF, (void *) mr.mr_base, vram_size);
  double_buffer = (char*) malloc(x_res * y_res * (bitspp / 8));
  return 0;
}

int activateTextMode() {
  return vg_exit();
}

int vg_draw_pixel(uint16_t x, uint16_t y, uint32_t color) {
  if (x < x_res && y < y_res) {
    char *vram = double_buffer + (y * x_res + x) * (bitspp / 8);
    int nBytes = bitspp/8;
    while(nBytes-- > 0){
      *(vram++) = color & 0xFF;
      color = color >> 8;
    }
    return 0;
  }
  else {
    return 1;
  }
}


Sprite readSprite(xpm_map_t  xpm){
  Sprite s;
  enum xpm_image_type type = XPM_INDEXED;
  s.arr = (char *) xpm_load(xpm, type, &(s.image));
  if(s.arr == NULL){
    printf("NULL\n");
  }
  //s.arr = read_xpm(map, &(s.width), &(s.height));
  return s;
}

int draw_pm(xpm_map_t xpm, uint16_t x, uint16_t y){
  
  
  
  Sprite sprite = readSprite(xpm);

  drawSprite(x, y, sprite);

  return 0;
}

int drawSprite(uint16_t x, uint16_t y, Sprite sprite){
  for(int iy = 0; iy < sprite.image.height; iy++){
    for(int ix = 0; ix < sprite.image.width; ix++){
      if(sprite.arr[ix + iy*sprite.image.width] != 0){
        vg_draw_pixel(x + ix, y + iy, sprite.arr[ix + iy*sprite.image.width]);
      }
    }
  }
  return 0;
}

int(vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
  for (unsigned ix = 0; ix < len; ix++) {
    if (vg_draw_pixel(x + ix, y, color) != OK) {
      return 1;
    }
  }
  return 0;
}

int vg_draw_vline(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
  for (unsigned iy = 0; iy < len; iy++) {
    if (vg_draw_pixel(x, y + iy, color) != OK) {
      return 1;
    }
  }
  return 0;
}

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color){
  vg_draw_hline(x, y, width, color);
  vg_draw_hline(x, y + height, width, color);
  vg_draw_vline(x , y , height, color);
  vg_draw_vline(x + width, y ,height, color);

  return 0;
}


void applyChanges(){
  memcpy(video_mem, double_buffer, y_res * x_res * (bitspp / 8));
}

