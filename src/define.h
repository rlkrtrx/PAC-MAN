#ifndef __DEFINE__
#define __DEFINE__

#define MENU_TILES 2
#define WINDOW_WIDTH TLSZ*28
#define WINDOW_HEIGHT TLSZ*(31+MENU_TILES)
#define TEXT_SF 2

#define WINDOW_NAME "PACMAN"

#define TLSZ 22.0f 
#define PI 3.141592653

#define PX_TILE_WIDTH 8
#define PX_TILE_HEIGHT 8

#define NONE 0
#define UP 1
#define RIGHT 2
#define DOWN 3
#define LEFT 4

#define U -1
#define R 1
#define D 1
#define L -1

int print_direction(int dir)
{
    switch(dir)
    {
        case UP:
            printf("UP");
        break;
        case RIGHT:
            printf("RIGHT");
        break;
        case DOWN:
            printf("DOWN");
        break;
        case LEFT:
            printf("LEFT");
        break;
        default:
        return 0;
        break;
    }
    return 1;
}

#endif
