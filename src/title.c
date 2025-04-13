#include <title.h>
#include <stdio.h>

int title_init(Title *title, GEFont *font, GETexture *font_texture,
               GEStdShader **shaders) {
    if(text_init(&title->title, font, font_texture, shaders[0], "GOLD HUNGER",
                 2)){
        puts("Failed to create title text");
        return 1;
    }
    if(text_init(&title->play, font, font_texture, shaders[0], "Play", 2)){
        puts("Failed to create play text");
        return 1;
    }
    if(text_init(&title->shop, font, font_texture, shaders[0], "Upgrades", 2)){
        puts("Failed to create shop text");
        return 1;
    }
    if(text_init(&title->credits, font, font_texture, shaders[0],
                 "2025 Mibi88 - Made with <3 for the Ludum Dare 57 (jam)", 1)){
        puts("Failed to create credits text");
        return 1;
    }
    if(text_init(&title->hiscore_text, font, font_texture, shaders[0],
                 "HISCORE: 0 - MAX.DEPTH: 0", 2)){
        puts("Failed to create hiscore text");
        return 1;
    }
    
    title->hiscore = 0;
    title->max_depth = 0;
    
    ge_entity_set_extra_data(&title->title.entity, (void*)1);
    ge_entity_set_extra_data(&title->play.entity, (void*)2);
    ge_entity_set_extra_data(&title->shop.entity, (void*)3);
    ge_entity_set_extra_data(&title->credits.entity, (void*)4);
    ge_entity_set_extra_data(&title->hiscore_text.entity, (void*)5);
    
    title->entities[0] = title->title.entity;
    title->entities[1] = title->play.entity;
    title->entities[2] = title->shop.entity;
    title->entities[3] = title->credits.entity;
    title->entities[4] = title->hiscore_text.entity;
    
    if(ge_scene_init(&title->scene, title->entities, 5, shaders, 1, 0)){
        puts("Failed to init title scene");
        return 1;
    }
    return 0;
}

void title_new_score(Title *title, int score, int depth) {
    char buffer[60];
    if(score > title->hiscore) title->hiscore = score;
    if(depth > title->max_depth) title->max_depth = depth;
    sprintf(buffer, "HISCORE: %d - MAX.DEPTH: %d", title->hiscore,
            title->max_depth);
    ge_text_update(&title->hiscore_text.text, buffer);
}

void title_update_entities(GEEntity *entity, void *data) {
    Title *title = data;
    switch((size_t)entity->extra){
        case 1:
            ge_entity_set_position(entity, -(title->w-title->title.text.w*
                                             title->title.text.size)/2,
                                   -96, 0);
            ge_entity_update(entity);
            break;
        case 2:
            title->text_x[1] = (title->w-title->play.text.w*
                                title->play.text.size)/2;
            title->text_y[1] = 160;
            ge_entity_set_position(entity, -title->text_x[1],
                                   -title->text_y[1], 0);
            ge_entity_update(entity);
            break;
        case 3:
            title->text_x[2] = (title->w-title->shop.text.w*
                                title->shop.text.size)/2;
            title->text_y[2] = 192;
            ge_entity_set_position(entity, -title->text_x[2],
                                   -title->text_y[2], 0);
            ge_entity_update(entity);
            break;
        case 4:
            ge_entity_set_position(entity,
                                   -(title->w-title->credits.text.w*
                                     title->credits.text.size)/2,
                                   -(title->h-64), 0);
            ge_entity_update(entity);
            break;
        case 5:
            ge_entity_set_position(entity,
                                   -(title->w-title->hiscore_text.text.w*
                                     title->hiscore_text.text.size)/2,
                                   -(title->h-128), 0);
            ge_entity_update(entity);
            break;
    }
}

void title_resize(Title *title, int w, int h) {
    title->w = w;
    title->h = h;
    ge_scene_for_entity(&title->scene, title_update_entities, title);
}

void title_render(Title *title) {
    ge_scene_update(&title->scene);
    ge_scene_render(&title->scene);
}

int title_on_play(Title *title, int x, int y) {
    x -= title->text_x[1]-16;
    y -= title->text_y[1]-8;
    if(x >= 0 && x < title->play.text.w*title->play.text.size &&
       y >= 0 && y < /*title->play.text.h*2*/16){
        return 1;
    }
    return 0;
}

int title_on_shop(Title *title, int x, int y) {
    x -= title->text_x[2]-16;
    y -= title->text_y[2]-8;
    if(x >= 0 && x < title->shop.text.w*title->shop.text.size &&
       y >= 0 && y < /*title->shop.text.h*2*/16){
        return 1;
    }
    return 0;
}

void title_free(Title *title) {
    text_free(&title->title);
    text_free(&title->play);
    text_free(&title->shop);
    text_free(&title->credits);
    text_free(&title->hiscore_text);
    ge_scene_free(&title->scene);
}

