#ifndef TITLE_H
#define TITLE_H

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
    Text title;
    Text play;
    Text shop;
    Text credits;
    Text hiscore_text;
    
    GEEntity entities[5];
    
    int text_x[5];
    int text_y[5];
    
    int w, h;
    
    int hiscore;
    int max_depth;
    
    GEScene scene;
} Title;

int title_init(Title *title, GEFont *font, GETexture *font_texture,
               GEStdShader **shaders);

void title_new_score(Title *title, int score, int depth);

void title_render(Title *title);

int title_on_play(Title *title, int x, int y);

int title_on_shop(Title *title, int x, int y);

void title_resize(Title *title, int w, int h);

void title_free(Title *title);

#endif

