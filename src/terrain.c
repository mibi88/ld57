#include <terrain.h>

#include <stdlib.h>
#include <string.h>

unsigned int xorshift(unsigned int *seed) {
    /* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
    *seed ^= *seed<<13;
    *seed ^= *seed>>17;
    *seed ^= *seed<<5;
    return *seed;
}

int terrain_init(Terrain *terrain, int w, int h, unsigned int seed) {
    terrain->tiles = malloc(w*h*sizeof(unsigned short int));
    if(terrain->tiles == NULL) return 1;
    terrain->w = w;
    terrain->h = h;
    terrain->seed = seed;
    return 0;
}

void terrain_dig(Terrain *terrain, int sx, int sy, int radius, int tile,
                 void on_tile(int x, int y, int tile, void *data),
                 void *data) {
    /* Using Jesko's Method of the midpoint circle algorithm */
    int t1 = radius>>4;
    int t2;
    int x = radius;
    int y = 0;
    int i;
    while(x >= y){
        for(i=sx-x;i<=sx+x;i++){
            if(i >= 0 && i < terrain->w){
                if(sy+y >= 0 && sy+y < terrain->h){
                    if(on_tile) on_tile(sy+y, i,
                                        terrain->tiles[(sy+y)*terrain->w+i],
                                        data);
                    terrain->tiles[(sy+y)*terrain->w+i] = tile;
                }
                if(sy-y >= 0 && sy-y < terrain->h){
                    if(on_tile) on_tile(sy-y, i,
                                        terrain->tiles[(sy-y)*terrain->w+i],
                                        data);
                    terrain->tiles[(sy-y)*terrain->w+i] = tile;
                }
            }
        }
        for(i=sx-y;i<=sx+y;i++){
            if(i >= 0 && i < terrain->w){
                if(sy+x >= 0 && sy+x < terrain->h){
                    if(on_tile) on_tile(sy+x, i,
                                        terrain->tiles[(sy+x)*terrain->w+i],
                                        data);
                    terrain->tiles[(sy+x)*terrain->w+i] = tile;
                }
                if(sy-x >= 0 && sy-x < terrain->h){
                    if(on_tile) on_tile(sy-x, i,
                                        terrain->tiles[(sy-x)*terrain->w+i],
                                        data);
                    terrain->tiles[(sy-x)*terrain->w+i] = tile;
                }
            }
        }
        y++;
        t1 += y;
        t2 = t1-x;
        if(t2 >= 0){
            t1 = t2;
            x--;
        }
    }
}

void terrain_generate_128_lines(Terrain *terrain) {
    unsigned int i;
    memmove(terrain->tiles, terrain->tiles+128*terrain->w,
            terrain->w*(terrain->h-128)*sizeof(unsigned short int));
    for(i=0;i<xorshift(&terrain->seed)%5;i++){
        /* Add holes */
        terrain_dig(terrain, xorshift(&terrain->seed)%terrain->w,
                    terrain->h-128+xorshift(&terrain->seed)%128,
                    xorshift(&terrain->seed)%4, 0, NULL, NULL);
    }
    for(i=0;i<xorshift(&terrain->seed)%4;i++){
        /* Add lava pools */
        terrain_dig(terrain, xorshift(&terrain->seed)%terrain->w,
                    terrain->h-128+xorshift(&terrain->seed)%128,
                    xorshift(&terrain->seed)%4, 2, NULL, NULL);
    }
    for(i=0;i<xorshift(&terrain->seed)%6;i++){
        /* Add copper */
        terrain_dig(terrain, xorshift(&terrain->seed)%terrain->w,
                    terrain->h-128+xorshift(&terrain->seed)%128,
                    xorshift(&terrain->seed)%3, 3, NULL, NULL);
    }
    for(i=0;i<xorshift(&terrain->seed)%5;i++){
        /* Add gold */
        terrain_dig(terrain, xorshift(&terrain->seed)%terrain->w,
                    terrain->h-128+xorshift(&terrain->seed)%128,
                    xorshift(&terrain->seed)%3, 4, NULL, NULL);
    }
    for(i=0;i<xorshift(&terrain->seed)%4;i++){
        /* Add platinum */
        terrain_dig(terrain, xorshift(&terrain->seed)%terrain->w,
                    terrain->h-128+xorshift(&terrain->seed)%128,
                    xorshift(&terrain->seed)%3, 5, NULL, NULL);
    }
    for(i=0;i<xorshift(&terrain->seed)%3;i++){
        /* Add diamond */
        terrain_dig(terrain, xorshift(&terrain->seed)%terrain->w,
                    terrain->h-128+xorshift(&terrain->seed)%128,
                    xorshift(&terrain->seed)%2, 6, NULL, NULL);
    }
}

void terrain_gen(Terrain *terrain) {
    int i;
    for(i=0;i<terrain->w*terrain->h;i++) terrain->tiles[i] = 1;
    for(i=0;i<terrain->h/128+1;i++) terrain_generate_128_lines(terrain);
}

void terrain_move_up(Terrain *terrain, int y) {
    if(y <= 0 || y >= terrain->h) return;
    memmove(terrain->tiles, terrain->tiles+(y*terrain->w),
            terrain->w*(terrain->h-y));
}

void terrain_free(Terrain *terrain) {
    free(terrain->tiles);
    terrain->tiles = NULL;
}

