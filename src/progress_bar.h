#ifndef PROGRESS_BAR_H
#define PROGRESS_BAR_H

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

typedef struct {
    GEImage image;
    GESprite sprite;
    GETexture texture;
    GERenderable renderable;
    GEEntity entity;
    int value;
    int r1, g1, b1;
    int r2, g2, b2;
} ProgressBar;

int progress_bar_init(ProgressBar *bar, GEStdShader *shader, int w, int h,
                      int value, int max, int r1, int g1, int b1, int r2,
                      int g2, int b2, float scale);

int progressbar_update(ProgressBar *bar, int value, int max);

void progressbar_free(ProgressBar *bar);

#endif

