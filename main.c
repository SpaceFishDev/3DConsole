#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <time.h>
#define SDL_MAIN_HANDLED
#include"include/SDL.h"
const camera_focal_length = 28; 
const w = 200;
const h = 100;
typedef struct
{
    int r;
    int g;
    int b;
} pixel;

 pixel* Buffer;


SDL_Renderer* renderer;
SDL_Window* window;

void put_pixel(pixel pix, int x, int y)
{
    if(x >= w){
        return;
    }
    if(y >= h){
        return;
    }
    Buffer[x + (y*w)] = pix;
}

void draw_vertex(int x, int y, int z)
{
    int xproj = (camera_focal_length*x) / (camera_focal_length + z);
    int yproj = (camera_focal_length*y) / (camera_focal_length + z);
    put_pixel((pixel){200,100,200}, xproj, yproj);
}


void draw(int SDL)
{
    int x = 0;
    int y = 0;
    for(int i = 0; i < w*h; ++i)
    {
        if(i%w == 0 && i != 0)
        {
            if(!SDL)
                printf("\n");
            x = 0;
            y++;
        }
        pixel p = Buffer[i];
        char* s = " .`-'~_:;^\",/+<*#@$"; 
        int gray = (p.r + p.g + p.b) / 3;

        if (gray < 0) {
            gray = 0;
        } else if (gray > 255) {
            gray = 255;
        }

        int index = gray * strlen(s) / 256;  
        char c = s[index];
        if(!SDL)
            printf("%c", c);
        else if(SDL && (p.r + p.g + p.b) > 0)
        {
            SDL_SetRenderDrawColor(renderer,p.r,p.g,p.b,255);
            SDL_RenderDrawPoint(renderer,x,y);
        }
        ++x;
    }
}


void draw_line(int x0, int y0, int x1, int y1)
{
    float dx = abs(x1 - x0);
    float sx = x0 < x1 ? 1 : -1;
    float dy = -abs(y1-y0);
    float sy = y0 < y1 ? 1 : -1;
    float error = dx + dy;
    srand(time(NULL));
    while(1)
    {
        if(x0 == x1 && y0 == y1) break;
        float e2 = 2 * error;
        put_pixel((pixel){255, 255, 255}, x0, y0);
        if(e2 >= dy)
        {
            if(x0 == x1) break;
            error = error + dy;
            x0 = x0 + sx;
        }
        if(e2 <= dx)
        {
            if(y0 == y1) break;
            error = error + dx;
            y0 = y0 + sy;
        }
    }
}

typedef struct
{
    int x, y,z;
} vert;

typedef struct
{   
    int a;
    int b;
} connect;

void draw_vertices(vert* x, int sz2, connect* connections)
{
    size_t i = 0;
    while(i != sz2)
    {
        int xproj = (camera_focal_length*x[connections[i].a].x) / (camera_focal_length + x[connections[i].a].z);
        int yproj = (camera_focal_length*x[connections[i].a].y) / (camera_focal_length + x[connections[i].a].z);
        int xproj1 = (camera_focal_length*x[connections[i].b].x) / (camera_focal_length +x[connections[i].b].z);
        int yproj1 = (camera_focal_length*x[connections[i].b].y) / (camera_focal_length + x[connections[i].b].z);
        draw_line(xproj, yproj, xproj1, yproj1);
        i++;
    }
}

void draw_prism(int x, int sx, int y, int sy, int z, int sz)
{
    vert cube[] = {
        {x, y,z}, 
        {x, y + sy,z}, 
        {x + sx, y, z},  
        {x + sx, y + sy,z},
        {x, y,z + sz}, 
        {x, y + sy,z + sz}, 
        {x + sx, y,z + sz},  
        {x + sx, y + sy,z + sz},
    };
    connect cube_c[] = {
        {0,1},
        {0,2},
        {1,3},
        {2,3},
        {4,5},
        {4,6},
        {5,7},
        {6,7},
        {0, 4},
        {1, 5},
        {2, 6},
        {3, 7},
    };
    draw_vertices(cube, 12, cube_c);
}


int main()
{
    SDL_CreateWindowAndRenderer(w*8,h*4,0,&window,&renderer);
    Buffer = malloc(sizeof(pixel)*h*w);
    if (Buffer == NULL) {
        printf("Failed to allocate memory for Buffer\n");
        exit(1);
    }
    int i = 0;
    for(; i != h*w; ++i)
    {
        Buffer[i].r = 0;
        Buffer[i].g = 0;
        Buffer[i].b = 0;
    }
    draw_prism(90,80,90,80,20,10);
    draw_prism(90 + 100,40,90,40,20,5);
    draw(0);
    while(1)
    {
        SDL_SetRenderDrawColor(renderer, 0,0,0,255);
        SDL_RenderClear(renderer);
        SDL_Event ev;
        while(SDL_PollEvent(&ev))
        {
            if(ev.type == SDL_QUIT)
            {
                return 0;
            }
        }
        draw(1);
        SDL_RenderPresent(renderer);
    }
}