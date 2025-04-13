#include <game.h>
#include <stdio.h>

int game_init(Game *game, GEWindow *window) {
    char *log;
    unsigned int seed;
    GEStdShader *shaders[1];
    
    game->last_time = ge_window_ms(window);
    ge_window_depth_test(window, 0);
    
    shaders[0] = &game->stdshader;
    
    if((log = ge_loader_load_shader(&game->shader, "shaders/vertex.vert",
                                    "shaders/fragment.frag")) != NULL){
        puts(log);
        return 1;
    }
    
    ge_shader_use(&game->shader);
    
    if(ge_stdshader_init(&game->stdshader, &game->shader)){
        puts("Failed to init stdshader");
        return 1;
    }
    
    if(ge_image_init(&game->font_image, "assets/font.png")){
        puts("Failed to init font image");
        return 1;
    }
    
    if(ge_texture_init(&game->font_texture, &game->font_image, 0, 0)){
        puts("Failed to init font texture");
        return 1;
    }
    
    if(ge_font_init(&game->font, &game->font_image, 0, 3, 0, 1, 1,
                    GE_F_ASCII)){
        puts("Failed to init font");
        return 1;
    }
    
    if(ge_text_init(&game->fps, &game->font, &game->font_texture,
                    &game->stdshader, "FPS: 0", 2)){
        puts("Failed to init fps text");
        return 1;
    }
    if(ge_loader_model_renderable(&game->fps_renderable,
                                  &GE_TEXT_GET_MODEL(&game->fps),
                                  &game->stdshader)){
        puts("Failed to create fps renderable");
        return 1;
    }
    if(ge_entity_init(&game->fps_entity, &game->fps_renderable)){
        puts("Failed to create fps entity");
        return 1;
    }
    
    ge_entity_set_position(&game->fps_entity, -16, -16, 0);
    ge_entity_update(&game->fps_entity);
    
    if(ge_scene_init(&game->scene, &game->fps_entity, 1, shaders, 1, 0)){
        puts("Failed to init scene");
        return 1;
    }
    
    if(ge_camera_init(&game->camera)){
        puts("Failed to init camera");
        return 1;
    }
    
    GE_CAMERA_ORTHO2D(&game->camera, 0, 0, 480, 360);
    
    if(title_init(&game->title, &game->font, &game->font_texture, shaders)){
        puts("Failed to create title screen");
        return 1;
    }
    
    if(ge_image_init(&game->tileset, "assets/tiles.png")){
        puts("Failed to init tileset image");
        return 1;
    }
    if(ge_texture_init(&game->tileset_texture, &game->tileset, 0, 0)){
        puts("Failed to init player texture");
        return 1;
    }
    
    if(ge_image_init(&game->player, "assets/player.png")){
        puts("Failed to init player image");
        return 1;
    }
    
    if(ge_texture_init(&game->player_texture, &game->player, 0, 0)){
        puts("Failed to init player texture");
        return 1;
    }
    
    seed = ge_window_ms(window);
    if(!seed) seed = 1;
    printf("Seed: %u\n", seed);
    
    if(playing_init(&game->playing, &game->font, &game->font_texture,
                    &game->tileset, &game->tileset_texture,
                    &game->player_texture, shaders, seed)){
        puts("Failed to create playing screen");
        return 1;
    }
    
    if(game_over_init(&game->game_over, &game->font, &game->font_texture,
                      shaders)){
        puts("Failed to create game over screen");
        return 1;
    }
    
    if(shop_init(&game->shop, &game->font, &game->font_texture, shaders)){
        puts("Failed to create shop screen");
        return 1;
    }
    
    game->screen = S_TITLE;
    game->tick_ms = 0;
    game->starve_tick_ms = 0;
    
    return 0;
}

void game_resize(Game *game, GEWindow *window, int w, int h) {
    ge_window_view(window, w, h);
    GE_CAMERA_ORTHO2D(&game->camera, 0, 0, w, h);
    
    title_resize(&game->title, w, h);
    playing_resize(&game->playing, w, h);
    shop_resize(&game->shop, w, h);
    game_over_resize(&game->game_over, w, h);
}

void game_click(Game *game, GEWindow *window, int x, int y) {
    unsigned int seed;
    switch(game->screen){
        case S_TITLE:
            if(title_on_play(&game->title, x, y)){
                /* Reset the game */
                seed = ge_window_ms(window);
                if(!seed) seed = 1;
                printf("Seed: %u\n", seed);
                playing_reset(&game->playing, seed);
                game->starve_tick_duration = STARVE_TICK_DURATION;
                
                game->screen = S_PLAYING;
                game->tick_ms = 0;
                game->starve_tick_ms = 0;
            }
            if(title_on_shop(&game->title, x, y)){
                game->screen = S_SHOP;
            }
            break;
        case S_PLAYING:
            playing_dig(&game->playing);
            break;
        case S_SHOP:
            if(shop_on_go_to_title(&game->shop, x, y)){
                game->screen= S_TITLE;
                break;
            }
            shop_click(&game->shop, &game->playing, x, y);
            break;
        case S_GAME_OVER:
            if(game_over_on_retry(&game->game_over, x, y)){
                /* Reset the game */
                seed = ge_window_ms(window);
                if(!seed) seed = 1;
                printf("Seed: %u\n", seed);
                playing_reset(&game->playing, seed);
                game->starve_tick_duration = STARVE_TICK_DURATION;
                
                game->screen = S_PLAYING;
                game->tick_ms = 0;
            }
            if(game_over_on_go_to_title(&game->game_over, x, y)){
                game->screen = S_TITLE;
            }
            break;
        default:
            puts("Something went wrong!");
            game->screen = S_TITLE;
    }
}

void game_keypress(Game *game, GEWindow *window, GEKey key) {
    (void)window;
    if(key == GE_K_SPACE && game->screen == S_PLAYING){
        playing_dig(&game->playing);
    }
}

void game_render(Game *game, GEWindow *window) {
    char fps_buffer[20];
    int score = 0;
    int depth = 0;
    while((game->new_time = ge_window_ms(window))-game->last_time < 16);
    game->delta_time = game->new_time-game->last_time;
    game->last_time = game->new_time;
    game->delta = game->delta_time/(float)1000;
    
    if(!game->delta_time) game->delta_time = 1;
    
    game->tick_ms += game->delta_time;
    game->starve_tick_ms += game->delta_time;
    
    ge_window_clear(window, 0, 0, 0, 1);
    
    sprintf(fps_buffer, "FPS: %lu\n", 1000/game->delta_time);
    ge_text_update(&game->fps, fps_buffer);
    
    ge_shader_use(&game->shader);
    
    ge_camera_use(&game->camera, &game->stdshader);
    
    switch(game->screen){
        case S_TITLE:
            title_render(&game->title);
            break;
        case S_PLAYING:
            while(game->tick_ms >= TICK_DURATION){
                playing_update_lava(&game->playing);
                game->tick_ms -= TICK_DURATION;
            }
            while(game->starve_tick_ms >= game->starve_tick_duration){
                playing_starve(&game->playing);
                game->starve_tick_ms -= game->starve_tick_duration;
                if(game->starve_tick_duration > MIN_STARVE_TICK_DURATION){
                    game->starve_tick_duration -= 5;
                    printf("Starve tick duration: %lu\n",
                           game->starve_tick_duration);
                }
            }
            playing_move_player(&game->playing, window, game->delta);
            playing_render(&game->playing);
            if(playing_game_over(&game->playing)){
                /* Calculate the score */
                playing_calculate_score(&game->playing, &score, &depth);
                game_over_set_score(&game->game_over, depth, score);
                game->shop.money += score;
                /* Set the high score */
                title_new_score(&game->title, score, depth);
                /* The player died, go to the game over screen */
                game->screen = S_GAME_OVER;
            }
            break;
        case S_SHOP:
            shop_render(&game->shop);
            break;
        case S_GAME_OVER:
            game_over_render(&game->game_over);
            break;
        default:
            puts("Something went wrong!");
            game->screen = S_TITLE;
    }
    
    ge_scene_render(&game->scene);
    
    ge_scene_update(&game->scene);
}

void game_free(Game *game) {
    (void)game;
    ge_shader_free(&game->shader);
    ge_stdshader_free(&game->stdshader);
    ge_font_free(&game->font);
    ge_image_free(&game->font_image);
    ge_texture_free(&game->font_texture);
    ge_image_free(&game->tileset);
    ge_texture_free(&game->tileset_texture);
    ge_image_free(&game->player);
    ge_texture_free(&game->player_texture);
    ge_text_free(&game->fps);
    ge_renderable_free(&game->fps_renderable);
    ge_scene_free(&game->scene);
    ge_camera_free(&game->camera);
    title_free(&game->title);
    playing_free(&game->playing);
    shop_free(&game->shop);
    game_over_free(&game->game_over);
}

