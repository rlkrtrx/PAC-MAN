#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ncurses.h>
#include <linmath/linmath.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <QDBMP/qdbmp.h>

#include "define.h"
#include "ngin.h"
#include "tb.h"
#include "blk.h"
#include "tile.h"
#include "map.h"
#include "col.h"
#include "spr.h"
#include "ghost.h"
#include "sct.h"

int main()
{
  sct prg = Sct();
  run(&prg);
  return 0;
}
