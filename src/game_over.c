#include <game_over.h>
#include <stdio.h>

int game_over_init(GameOver *game_over, GEFont *font, GETexture *font_texture,
                   GEStdShader **shaders) {
    if(text_init(&game_over->game_over, font, font_texture, shaders[0],
                 "GAME OVER", 2)){
        puts("Failed to create game over text");
        return 1;
    }
    if(text_init(&game_over->score, font, font_texture, shaders[0],
                 "Depth: 0 - Score: 0 - +$0", 2)){
        puts("Failed to create score text");
        return 1;
    }
    if(text_init(&game_over->retry, font, font_texture, shaders[0], "Retry",
                 2)){
        puts("Failed to create retry text");
        return 1;
    }
    if(text_init(&game_over->title, font, font_texture, shaders[0],
                 "Go to the title", 2)){
        puts("Failed to create the go to title text");
        return 1;
    }
    
    ge_entity_set_extra_data(&game_over->game_over.entity, (void*)1);
    ge_entity_set_extra_data(&game_over->score.entity, (void*)2);
    ge_entity_set_extra_data(&game_over->retry.entity, (void*)3);
    ge_entity_set_extra_data(&game_over->title.entity, (void*)4);
    
    game_over->entities[0] = game_over->game_over.entity;
    game_over->entities[1] = game_over->score.entity;
    game_over->entities[2] = game_over->retry.entity;
    game_over->entities[3] = game_over->title.entity;
    
    if(ge_scene_init(&game_over->scene, game_over->entities, 4, shaders, 1,
                     0)){
        puts("Failed to init game over scene");
        return 1;
    }
    return 0;
}

void game_over_update_entities(GEEntity *entity, void *data) {
    GameOver *game_over = data;
    switch((size_t)entity->extra){
        case 1:
            ge_entity_set_position(entity,
                                   -(game_over->w-game_over->game_over.text.w*
                                     game_over->game_over.text.size)/2,
                                   -96, 0);
            ge_entity_update(entity);
            break;
        case 2:
            ge_entity_set_position(entity,
                                   -(game_over->w-game_over->score.text.w*
                                     game_over->score.text.size)/2,
                                   -160, 0);
            ge_entity_update(entity);
            break;
        case 3:
            game_over->text_x[2] = (game_over->w-game_over->retry.text.w*
                                    game_over->retry.text.size)/2;
            game_over->text_y[2] = 224;
            ge_entity_set_position(entity, -game_over->text_x[2],
                                   -game_over->text_y[2], 0);
            ge_entity_update(entity);
            break;
        case 4:
            game_over->text_x[3] = (game_over->w-game_over->title.text.w*
                                    game_over->title.text.size)/2;
            game_over->text_y[3] = 256;
            ge_entity_set_position(entity, -game_over->text_x[3],
                                   -game_over->text_y[3], 0);
            ge_entity_update(entity);
            break;
    }
}

int game_over_set_score(GameOver *game_over, int depth, int score) {
    char buffer[60];
    sprintf(buffer, "Depth: %d - Score: %d - +$%d", depth, score, score);
    if(ge_text_update(&game_over->score.text, buffer)){
        puts("Failed to update score text");
        return 1;
    }
    return 0;
}

void game_over_resize(GameOver *game_over, int w, int h) {
    game_over->w = w;
    game_over->h = h;
}

void game_over_render(GameOver *game_over) {
    ge_scene_for_entity(&game_over->scene, game_over_update_entities,
                        game_over);
    ge_scene_update(&game_over->scene);
    ge_scene_render(&game_over->scene);
}

int game_over_on_retry(GameOver *game_over, int x, int y) {
    x -= game_over->text_x[2]-16;
    y -= game_over->text_y[2]-8;
    if(x >= 0 && x < game_over->retry.text.w*game_over->retry.text.size &&
       y >= 0 && y < 16){
        return 1;
    }
    return 0;
}

int game_over_on_go_to_title(GameOver *game_over, int x, int y) {
    x -= game_over->text_x[3]-16;
    y -= game_over->text_y[3]-8;
    if(x >= 0 && x < game_over->title.text.w*game_over->title.text.size &&
       y >= 0 && y < 16){
        return 1;
    }
    return 0;
}

void game_over_free(GameOver *game_over) {
    text_free(&game_over->game_over);
    text_free(&game_over->score);
    text_free(&game_over->retry);
    text_free(&game_over->title);
    ge_scene_free(&game_over->scene);
}
