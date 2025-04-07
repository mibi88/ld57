#ifndef PLAYING_H
#define PLAYING_H

#define MAP_WIDTH 64
#define MAP_HEIGHT 512

#include <mibiengine2/base/window.h>
#include <mibiengine2/base/image.h>
#include <mibiengine2/base/texture.h>
#include <mibiengine2/base/shader.h>
#include <mibiengine2/renderer/scene.h>
#include <mibiengine2/renderer/loader.h>
#include <mibiengine2/renderer/stdshader.h>
#include <mibiengine2/render2d/sprite.h>
#include <mibiengine2/render2d/font.h>
#include <mibiengine2/render2d/text.h>
#include <mibiengine2/render2d/tilemap.h>

#include <terrain.h>
#include <progress_bar.h>
#include <text.h>

typedef struct {
    Terrain terrain;
    
    GETilemap map;
    GERenderable map_renderable;
    GEEntity *map_entity;
    
    GESprite player_sprite;
    GERenderable player_renderable;
    GEEntity *player;
    
    GEEntity entities[4];
    
    GEEntity *food_bar;
    
    ProgressBar food_bar_data;
    
    Text food_text;
    
    GEScene scene;
    int depth;
    int w, h;
    
    GEVec2 player_pos;
    GEVec2 velocity;
    float map_y;
    
    GEKey keys[3];
    
    float speed;
    float max;
    float jump_force;
    float gravity;
    
    int food;
    
    int copper_food, gold_food, platinum_food, diamond_food;
    int dig_hunger;
    int dig_radius;
    int hunger;
    
    int scrolled;
    float max_y;
    int score;
    
    unsigned int copper, gold, platinum, diamond;
} Playing;

int playing_init(Playing *playing, GEFont *font, GETexture *font_texture,
                 GEImage *tileset, GETexture *tileset_texture,
                 GETexture *player_texture, GEStdShader **shaders,
                 unsigned int seed);

void playing_screen_to_map(Playing *playing, int sx, int sy, float *x,
                           float *y);

void playing_starve(Playing *playing);

void playing_dig(Playing *playing);

void playing_move_player(Playing *playing, GEWindow *window, float delta);

void playing_update_lava(Playing *playing);

void playing_calculate_score(Playing *playing, int *score, int *depth);

int playing_reset(Playing *playing, unsigned int seed);

int playing_game_over(Playing *playing);

void playing_render(Playing *playing);

void playing_resize(Playing *playing, int w, int h);

void playing_free(Playing *playing);

#endif

