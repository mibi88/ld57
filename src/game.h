#ifndef GAME_H
#define GAME_H

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

#include <title.h>
#include <playing.h>
#include <shop.h>
#include <game_over.h>

#define TICK_DURATION 1000
#define STARVE_TICK_DURATION 500
#define MIN_STARVE_TICK_DURATION 75

enum {
    S_TITLE,
    S_PLAYING,
    S_SHOP,
    S_GAME_OVER,
    S_AMOUNT
};

typedef struct {
    GEShader shader;
    GEStdShader stdshader;
    
    GEFont font;
    GEImage font_image;
    GETexture font_texture;
    
    GEImage tileset;
    GETexture tileset_texture;
    
    GEImage player;
    GETexture player_texture;
    
    GEText fps;
    GERenderable fps_renderable;
    GEEntity fps_entity;
    
    GEScene scene;
    GECamera camera;
    
    Title title;
    Playing playing;
    Shop shop;
    GameOver game_over;
    
    unsigned long last_time;
    unsigned long new_time;
    unsigned long delta_time;
    float delta;
    
    unsigned long tick_ms;
    unsigned long starve_tick_ms;
    
    unsigned long starve_tick_duration;
    
    unsigned char screen;
} Game;

int game_init(Game *game, GEWindow *window);

void game_resize(Game *game, GEWindow *window, int w, int h);

void game_click(Game *game, GEWindow *window, int x, int y);

void game_keypress(Game *game, GEWindow *window, GEKey key);

void game_render(Game *game, GEWindow *window);

void game_free(Game *game);

#endif

