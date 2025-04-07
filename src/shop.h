#ifndef SHOP_H
#define SHOP_H

#define UPGRADE_NUM 4
#define MIN_ENTITY_NUM 4

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

#include <playing.h>
#include <text.h>

typedef struct {
    Text title;
    Text go_to_title;
    Text nothing_available;
    Text money_text;
    
    Text upgrades[UPGRADE_NUM];
    Text price_text[UPGRADE_NUM];
    Text level_text[UPGRADE_NUM];
    
    void (*buy[UPGRADE_NUM])(void *_shop, Playing *playing, int level,
                             int *price);
    
    int upgrade_level[UPGRADE_NUM];
    
    int upgrade_max[UPGRADE_NUM];
    
    int price[UPGRADE_NUM];
    
    GEEntity entities[MIN_ENTITY_NUM+UPGRADE_NUM*3];
    
    GEScene scene;
    
    int money;
    
    int w, h;
    
    int current_y[3];
    
    int text_x[MIN_ENTITY_NUM+UPGRADE_NUM*3];
    int text_y[MIN_ENTITY_NUM+UPGRADE_NUM*3];
} Shop;

int shop_init(Shop *shop, GEFont *font, GETexture *font_texture,
              GEStdShader **shaders);

void shop_render(Shop *shop);

int shop_on_go_to_title(Shop *shop, int x, int y);

void shop_click(Shop *shop, Playing *playing, int x, int y);

void shop_resize(Shop *shop, int w, int h);

void shop_free(Shop *shop);

#endif

