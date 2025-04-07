#ifndef GAME_OVER_H
#define GAME_OVER_H

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

#include <text.h>

typedef struct {
    Text game_over;
    Text score;
    Text retry;
    Text title;
    
    GEEntity entities[4];
    
    int text_x[4];
    int text_y[4];
    
    int w, h;
    
    GEScene scene;
} GameOver;

int game_over_init(GameOver *game_over, GEFont *font, GETexture *font_texture,
                   GEStdShader **shaders);

int game_over_set_score(GameOver *game_over, int depth, int score);

void game_over_resize(GameOver *game_over, int w, int h);

void game_over_render(GameOver *game_over);

int game_over_on_retry(GameOver *game_over, int x, int y);

int game_over_on_go_to_title(GameOver *game_over, int x, int y);

void game_over_free(GameOver *game_over);

#endif

