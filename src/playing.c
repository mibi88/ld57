#include <playing.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>

int playing_init(Playing *playing, GEFont *font, GETexture *font_texture,
                 GEImage *tileset, GETexture *tileset_texture,
                 GETexture *player_texture, GEStdShader **shaders,
                 unsigned int seed) {
    float x, y;
    (void)font;
    (void)font_texture;
    playing->map_entity = playing->entities;
    playing->player = playing->entities+1;
    playing->food_bar = playing->entities+2;
    playing->depth = 0;
    
    playing->player_pos.x = 0;
    playing->player_pos.y = 16;
    
    playing->velocity.x = 0;
    playing->velocity.y = 0;
    
    playing->keys[0] = GE_K_LEFT;
    playing->keys[1] = GE_K_RIGHT;
    playing->keys[2] = GE_K_UP;
    
    playing->speed = 7000;
    playing->max = 400;
    playing->gravity = 200;
    playing->jump_force = 200;
    
    playing->food = 100;
    
    playing->copper_food = 2;
    playing->gold_food = 3;
    playing->platinum_food = 4;
    playing->diamond_food = 5;
    
    playing->dig_hunger = 1;
    playing->hunger = 1;
    
    playing->copper = 0;
    playing->gold = 0;
    playing->platinum = 0;
    playing->diamond = 0;
    
    playing->dig_radius = 2;
    
    playing->map_y = playing->player_pos.y-playing->h/2;
    
    if(text_init(&playing->food_text, font, font_texture, shaders[0], "Food:",
                 2)){
        puts("Failed to create food text");
        return 1;
    }
    ge_entity_set_extra_data(&playing->food_text.entity, (void*)4);
    playing->entities[3] = playing->food_text.entity;
    
    if(terrain_init(&playing->terrain, MAP_WIDTH, MAP_HEIGHT, seed)){
        puts("Failed to initialize terrain!");
        return 1;
    }
    terrain_gen(&playing->terrain);
    
    playing_screen_to_map(playing, playing->player_pos.x+playing->w/2,
                          playing->h/2, &x, &y);
    terrain_dig(&playing->terrain, (int)x, (int)y, 6, 0, NULL, NULL);
    
    /* Create the map */
    if(ge_tilemap_init(&playing->map, tileset, tileset_texture, shaders[0],
                       playing->terrain.tiles, MAP_WIDTH, MAP_HEIGHT, 16, 16,
                       32)){
        puts("Failed to init tilemap");
        return 1;
    }
    if(ge_loader_model_renderable(&playing->map_renderable,
                                  &GE_TILEMAP_GET_MODEL(&playing->map),
                                  shaders[0])){
        puts("Failed to create tilemap renderable");
        return 1;
    }
    if(ge_entity_init(playing->map_entity, &playing->map_renderable)){
        puts("Failed to create tilemap entity");
        return 1;
    }
    ge_entity_set_extra_data(playing->map_entity, (void*)1);
    
    /* Create the player sprite and entity */
    if(ge_sprite_init(&playing->player_sprite, player_texture, shaders[0], 32,
                      32)){
        puts("Failed to create player sprite");
        return 1;
    }
    if(ge_loader_model_renderable(&playing->player_renderable,
                                  &GE_SPRITE_MODEL(&playing->player_sprite),
                                  shaders[0])){
        puts("Failed to create player renderable");
        return 1;
    }
    if(ge_entity_init(playing->player, &playing->player_renderable)){
        puts("Failed to create player entity");
        return 1;
    }
    ge_entity_set_extra_data(playing->player, (void*)2);
    
    /* Create the food progress bar */
    if(progress_bar_init(&playing->food_bar_data, shaders[0], 50, 5,
                         playing->food, 100, 0, 255, 0, 255, 0, 0, 4)){
        puts("Failed to create food progress bar");
        return 1;
    }
    
    *playing->food_bar = playing->food_bar_data.entity;
    ge_entity_set_extra_data(playing->food_bar, (void*)3);
    ge_entity_set_position(playing->food_bar, -480+(8+25*4), -8-2.5*4, 0);
    ge_entity_update(playing->food_bar);
    
    /* Create the scene */
    if(ge_scene_init(&playing->scene, playing->entities, 4, shaders, 1, 0)){
        puts("Failed to init playing scene");
        return 1;
    }
    return 0;
}

void playing_screen_to_map(Playing *playing, int sx, int sy, float *x,
                           float *y) {
    *x = (-(sx-playing->w/2)+playing->w/2-(playing->w/2-MAP_WIDTH*32/2))/32.0;
    *y = (sy-playing->h/2+playing->player_pos.y)/32.0;
}

void playing_on_tile(int x, int y, int tile, void *data) {
    Playing *playing = data;
    (void)x;
    (void)y;
    switch(tile){
        case 3:
            playing->copper++;
            playing->food += playing->copper_food;
            break;
        case 4:
            playing->gold++;
            playing->food += playing->gold_food;
            break;
        case 5:
            playing->platinum++;
            playing->food += playing->platinum_food;
            break;
        case 6:
            playing->diamond++;
            playing->food += playing->diamond_food;
            break;
        default:
            break;
    }
}

void playing_starve(Playing *playing) {
    playing->food -= playing->hunger;
    progressbar_update(&playing->food_bar_data, playing->food, 100);
}

void playing_dig(Playing *playing) {
    float x, y;
    playing_screen_to_map(playing, playing->player_pos.x+playing->w/2,
                          playing->h/2, &x, &y);
    terrain_dig(&playing->terrain, (int)x, (int)y, playing->dig_radius, 0,
                playing_on_tile, playing);
    ge_tilemap_update(&playing->map, playing->terrain.tiles, MAP_WIDTH,
                      MAP_HEIGHT, 32);
    /*printf("%d, %d, %d, %d\n", playing->copper, playing->gold,
           playing->platinum, playing->diamond);*/
    playing->food -= playing->dig_hunger;
    progressbar_update(&playing->food_bar_data, playing->food, 100);
    if(playing->food > 100) playing->food = 100;
}

int playing_collision(Playing *playing, float sx, float sy) {
    int x = floor(sx);
    int y = floor(sy);
    if(x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) return 1;
    if(playing->terrain.tiles[y*MAP_WIDTH+x] &&
       playing->terrain.tiles[y*MAP_WIDTH+x] != 2) return 1;
    return 0;
}

void playing_move_player(Playing *playing, GEWindow *window, float delta) {
    float x, y;
    if(ge_window_key_pressed(window, playing->keys[0])){
        /* Move left */
        playing->velocity.x += playing->speed*delta;
        if(playing->velocity.x > playing->max){
            playing->velocity.x = playing->max;
        }
    }
    if(ge_window_key_pressed(window, playing->keys[1])){
        /* Move right */
        playing->velocity.x -= playing->speed*delta;
        if(playing->velocity.x < -playing->max){
            playing->velocity.x = -playing->max;
        }
    }
    if(!ge_window_key_pressed(window, playing->keys[0]) &&
       !ge_window_key_pressed(window, playing->keys[1])){
        if(playing->velocity.x > 0){
            playing->velocity.x -= playing->speed*delta/2;
            if(playing->velocity.x < 0) playing->velocity.x = 0;
        }else if(playing->velocity.x < 0){
            playing->velocity.x += playing->speed*delta/2;
            if(playing->velocity.x > 0) playing->velocity.x = 0;
        }
    }
    if(ge_window_key_pressed(window, playing->keys[2])){
        /* Jump */
        playing_screen_to_map(playing, playing->player_pos.x+playing->w/2,
                              playing->h/2+16, &x, &y);
        if(playing_collision(playing, x, y)){
            playing->velocity.y = -playing->jump_force;
        }
    }
    
    playing->player_pos.y += playing->velocity.y*delta;
    playing->player_pos.x += playing->velocity.x*delta;
    
    playing_screen_to_map(playing, playing->player_pos.x+playing->w/2,
                          playing->h/2+16, &x, &y);
    
    playing->velocity.y += playing->gravity*delta;
    if(playing->velocity.y > 0 && playing_collision(playing, x, y)){
        playing->player_pos.y -= playing->velocity.y*delta;
        playing->velocity.y = 0;
        playing->player_pos.y = floor(playing->player_pos.y/32)*32+17;
    }
    
    if(playing->velocity.y < 0 && playing_collision(playing, x, y-1)){
        playing->player_pos.y -= playing->velocity.y*delta;
        playing->velocity.y = 0;
        playing->player_pos.y = floor(playing->player_pos.y/32)*32+17;
    }
    
    if(playing->velocity.x > 0 && playing_collision(playing, x-0.5, y-0.5)){
        playing->player_pos.x -= playing->velocity.x*delta;
        playing->velocity.x = 0;
        playing->player_pos.x = floor(playing->player_pos.x/32)*32+16;
    }
    
    if(playing->velocity.x < 0 && playing_collision(playing, x+0.5, y-0.5)){
        playing->player_pos.x -= playing->velocity.x*delta;
        playing->velocity.x = 0;
        playing->player_pos.x = floor(playing->player_pos.x/32)*32+16;
    }
    if(playing->player_pos.y > playing->max_y){
        playing->max_y = playing->player_pos.y;
    }
}

void playing_update_lava(Playing *playing) {
    int x, y;
    unsigned char updated[MAP_WIDTH*MAP_HEIGHT];
    memset(updated, 0, MAP_WIDTH*MAP_HEIGHT);
    for(y=0;y<MAP_HEIGHT;y++){
        for(x=0;x<MAP_WIDTH;x++){
            if(playing->terrain.tiles[y*MAP_WIDTH+x] == 2 &&
               !updated[y*MAP_WIDTH+x]){
                if(y < MAP_HEIGHT-1){
                    if(!playing->terrain.tiles[(y+1)*MAP_WIDTH+x] &&
                       !updated[(y+1)*MAP_WIDTH+x]){
                        playing->terrain.tiles[(y+1)*MAP_WIDTH+x] = 2;
                        updated[(y+1)*MAP_WIDTH+x] = 1;
                    }else{
                        if(x){
                            if(!playing->terrain.tiles[y*MAP_WIDTH+x-1] &&
                               ((playing->terrain.tiles[(y+1)*MAP_WIDTH+x-1] &&
                                 playing->terrain.tiles[(y+1)*MAP_WIDTH+x-1]
                                                                    != 2 &&
                                 !updated[(y+1)*MAP_WIDTH+x-1]) ||
                                (playing->terrain.tiles[(y+1)*MAP_WIDTH+x+1]
                                                                    != 2 &&
                                 playing->terrain.tiles[(y+1)*MAP_WIDTH+x+1] &&
                                 !updated[(y+1)*MAP_WIDTH+x+1])) &&
                               !updated[y*MAP_WIDTH+x-1]){
                                playing->terrain.tiles[y*MAP_WIDTH+x-1] = 2;
                                updated[y*MAP_WIDTH+x-1] = 1;
                            }
                        }
                        if(x < MAP_WIDTH-1){
                            if(!playing->terrain.tiles[y*MAP_WIDTH+x+1] &&
                               ((playing->terrain.tiles[(y+1)*MAP_WIDTH+x+1] &&
                                 playing->terrain.tiles[(y+1)*MAP_WIDTH+x+1]
                                                                    != 2 &&
                                 !updated[(y+1)*MAP_WIDTH+x+1]) ||
                                (playing->terrain.tiles[(y+1)*MAP_WIDTH+x-1]
                                                                    != 2 &&
                                 playing->terrain.tiles[(y+1)*MAP_WIDTH+x-1] &&
                                 !updated[(y+1)*MAP_WIDTH+x-1])) &&
                               !updated[y*MAP_WIDTH+x+1]){
                                playing->terrain.tiles[y*MAP_WIDTH+x+1] = 2;
                                updated[y*MAP_WIDTH+x+1] = 1;
                                x++;
                            }
                        }
                    }
                }
            }
        }
    }
    ge_tilemap_update(&playing->map, playing->terrain.tiles, MAP_WIDTH,
                      MAP_HEIGHT, 32);
}

void playing_calculate_score(Playing *playing, int *score, int *depth) {
    *depth = (playing->max_y < 0 ? 0 : playing->max_y)/32+playing->scrolled;
    *score = (playing->copper+playing->gold*2+playing->platinum*4+
              playing->diamond*8)*(*depth)/32;
}

int playing_reset(Playing *playing, unsigned int seed) {
    float x, y;
    playing->player_pos.x = 0;
    playing->player_pos.y = 16;
    
    playing->velocity.x = 0;
    playing->velocity.y = 0;
    
    playing->max_y = 0;
    
    playing->food = 100;
    
    playing->copper = 0;
    playing->gold = 0;
    playing->platinum = 0;
    playing->diamond = 0;
    
    playing->scrolled = 0;
    
    playing->terrain.seed = seed;
    
    playing->map_y = playing->player_pos.y-playing->h/2;
    
    terrain_gen(&playing->terrain);
    
    playing_screen_to_map(playing, playing->player_pos.x+playing->w/2,
                          playing->h/2, &x, &y);
    terrain_dig(&playing->terrain, (int)x, (int)y, 6, 0, NULL, NULL);
    
    progressbar_update(&playing->food_bar_data, playing->food, 100);
    
    ge_tilemap_update(&playing->map, playing->terrain.tiles, MAP_WIDTH,
                      MAP_HEIGHT, 32);
    return 0;
}

int playing_game_over(Playing *playing) {
    float x, y;
    if(playing->food < 0) return 1;
    playing_screen_to_map(playing, playing->player_pos.x+playing->w/2,
                          playing->h/2, &x, &y);
    if((int)x < 0 || (int)x >= MAP_WIDTH ||
       (int)y < 0 || (int)y >= MAP_HEIGHT) return 0;
    if(playing->terrain.tiles[(int)y*MAP_WIDTH+(int)x] == 2) return 1;
    return 0;
}

void playing_update_entities(GEEntity *entity, void *data) {
    Playing *playing = data;
    switch((size_t)entity->extra){
        case 1:
            /* It's the tilemap */
            ge_entity_set_position(entity, -playing->w/2+MAP_WIDTH*32/2-
                                   playing->player_pos.x,
                                   (int)(playing->player_pos.y-playing->h/2),
                                   0);
            break;
        case 2:
            /* It's the player */
            ge_entity_set_position(entity,
                                   -playing->w/2,
                                   -playing->h/2, 0);
            break;
        case 3:
            /* It's the food progress bar */
            ge_entity_set_position(entity, -playing->w+(8+25*4), -8-2.5*4, 0);
            ge_entity_update(entity);
            break;
        case 4:
            /* It's the food text */
            ge_entity_set_position(entity, -(playing->w-(8+25*4)-
                                   playing->food_text.text.w*
                                   playing->food_text.text.size*2), -8-2.5*4,
                                   0);
            ge_entity_update(entity);
    }
    ge_entity_update(entity);
}

void playing_render(Playing *playing) {
    while(playing->player_pos.y > 192*32){
        printf("%f\n", playing->player_pos.y);
        /* Scroll the map */
        terrain_generate_128_lines(&playing->terrain);
        ge_tilemap_update(&playing->map, playing->terrain.tiles, MAP_WIDTH,
                          MAP_HEIGHT, 32);
        playing->scrolled += 128;
        playing->player_pos.y -= 128*32;
        printf("playing->scrolled = %d\n", playing->scrolled);
    }
    playing->map_y = playing->player_pos.y-playing->h/2;
    ge_scene_for_entity(&playing->scene, playing_update_entities, playing);
    ge_scene_update(&playing->scene);
    ge_scene_render(&playing->scene);
}

void playing_resize(Playing *playing, int w, int h) {
    playing->w = w;
    playing->h = h;
}

void playing_free(Playing *playing) {
    (void)playing;
    /* TODO */
}

