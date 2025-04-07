#ifndef TEXT_H
#define TEXT_H

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

typedef struct {
    GEText text;
    GERenderable renderable;
    GEEntity entity;
} Text;

int text_init(Text *text, GEFont *font, GETexture *font_texture,
               GEStdShader *shader, char *str, float scale);

void text_free(Text *text);

#endif

