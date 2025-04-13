#include <text.h>
#include <stdio.h>

int text_init(Text *text, GEFont *font, GETexture *font_texture,
               GEStdShader *shader, char *str, float scale) {
    if(ge_text_init(&text->text, font, font_texture, shader, str, scale)){
        puts("Failed to init text");
        return 1;
    }
    if(ge_loader_model_renderable(&text->renderable,
                                  &GE_TEXT_GET_MODEL(&text->text),
                                  shader)){
        puts("Failed to create text renderable");
        return 1;
    }
    if(ge_entity_init(&text->entity, &text->renderable)){
        puts("Failed to create text entity");
        return 1;
    }
    return 0;
}

void text_free(Text *text) {
    ge_text_free(&text->text);
    ge_renderable_free(&text->renderable);
}

