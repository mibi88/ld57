#ifndef TERRAIN_H
#define TERRAIN_H

typedef struct {
    unsigned short int *tiles;
    int w, h;
    unsigned int seed;
} Terrain;

int terrain_init(Terrain *terrain, int w, int h, unsigned int seed);

void terrain_generate_128_lines(Terrain *terrain);

void terrain_gen(Terrain *terrain);

void terrain_move_up(Terrain *terrain, int y);

void terrain_dig(Terrain *terrain, int sx, int sy, int radius, int tile,
                 void on_tile(int x, int y, int tile, void *data),
                 void *data);

void terrain_free(Terrain *terrain);

#endif

