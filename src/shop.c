#include <shop.h>
#include <stdio.h>
#include <string.h>

void buy_iron_body(void *_shop, Playing *playing, int level, int *price) {
    (void)_shop;
    (void)level;
    *price *= 2;
    playing->dig_radius += 2;
    playing->dig_hunger++;
}

void buy_spring_boots(void *_shop, Playing *playing, int level, int *price) {
    (void)_shop;
    (void)level;
    *price += 20;
    playing->gravity -= 10;
}

void buy_batteries_included(void *_shop, Playing *playing, int level,
                            int *price) {
    (void)_shop;
    (void)level;
    *price += 20;
    playing->max += 75;
}

void buy_tasty_diamonds(void *_shop, Playing *playing, int level, int *price) {
    (void)_shop;
    (void)level;
    *price *= 2;
    playing->gravity += 20;
}

int shop_init(Shop *shop, GEFont *font, GETexture *font_texture,
              GEStdShader **shaders) {
    size_t i;
    char *upgrades[UPGRADE_NUM] = {
        "Iron body - Increase the mining radius by 2 (Also causes +1 hunger "
        "when mining)",
        "Magnet boots - Increase gravity (+20 gravity)",
        "Batteries included - Move faster (+75 speed)",
        "Tasty diamonds - Get +1 food point when mining diamonds"
    };
    int base_price[UPGRADE_NUM] = {
        20,
        30,
        50,
        80
    };
    int upgrade_max[UPGRADE_NUM] = {
        1,
        5,
        5,
        4
    };
    char buffer[20];
    
    void (*buy[UPGRADE_NUM])(void *_shop, Playing *playing, int level,
                             int *price) = {
        buy_iron_body,
        buy_spring_boots,
        buy_batteries_included,
        buy_tasty_diamonds
    };
    
    if(text_init(&shop->title, font, font_texture, shaders[0], "UPGRADES", 2)){
        puts("Failed to create shop text");
        return 1;
    }
    if(text_init(&shop->nothing_available, font, font_texture, shaders[0],
                 "No upgrades available - Get more money by playing more", 1)){
        puts("Failed to create no upgrades text");
        return 1;
    }
    if(text_init(&shop->go_to_title, font, font_texture, shaders[0],
                 "Go to the title", 2)){
        puts("Failed to create go to title text");
        return 1;
    }
    if(text_init(&shop->money_text, font, font_texture, shaders[0], "$0", 2)){
        puts("Failed to create money text");
        return 1;
    }
    
    ge_entity_set_extra_data(&shop->title.entity, (void*)0);
    ge_entity_set_extra_data(&shop->nothing_available.entity, (void*)1);
    ge_entity_set_extra_data(&shop->go_to_title.entity, (void*)2);
    ge_entity_set_extra_data(&shop->money_text.entity, (void*)3);
    
    shop->entities[0] = shop->title.entity;
    shop->entities[1] = shop->nothing_available.entity;
    shop->entities[2] = shop->go_to_title.entity;
    shop->entities[3] = shop->money_text.entity;
    
    memcpy(shop->price, base_price, UPGRADE_NUM*sizeof(int));
    memcpy(shop->upgrade_max, upgrade_max, UPGRADE_NUM*sizeof(int));
    memcpy(shop->buy, buy,
           UPGRADE_NUM*sizeof(void (*)(void*, Playing*, int, int*)));
    memset(shop->upgrade_level, 0, UPGRADE_NUM*sizeof(int));
    
    for(i=0;i<UPGRADE_NUM;i++){
        if(text_init(shop->upgrades+i, font, font_texture, shaders[0],
                     upgrades[i], 2)){
            puts("Failed to create upgrade description text");
            return 1;
        }
        sprintf(buffer, "$%d", shop->price[i]);
        if(text_init(shop->price_text+i, font, font_texture, shaders[0],
                     buffer, 2)){
            puts("Failed to create upgrade price text");
            return 1;
        }
        sprintf(buffer, "Lvl.%d/%d", shop->upgrade_level[i],
                shop->upgrade_max[i]);
        if(text_init(shop->level_text+i, font, font_texture, shaders[0],
                     buffer, 2)){
            puts("Failed to create upgrade level text");
            return 1;
        }
        ge_entity_set_extra_data(&shop->upgrades[i].entity,
                                 (void*)(MIN_ENTITY_NUM+i));
        ge_entity_set_extra_data(&shop->price_text[i].entity,
                                 (void*)(MIN_ENTITY_NUM+UPGRADE_NUM+i));
        ge_entity_set_extra_data(&shop->level_text[i].entity,
                                 (void*)(MIN_ENTITY_NUM+UPGRADE_NUM*2+i));
        
        /* Hide all of the upgrades by putting them offscreen */
        ge_entity_set_position(&shop->upgrades[i].entity, 0, 100, 0);
        ge_entity_set_position(&shop->price_text[i].entity, 0, 100, 0);
        ge_entity_set_position(&shop->level_text[i].entity, 0, 100, 0);
        ge_entity_update(&shop->upgrades[i].entity);
        ge_entity_update(&shop->price_text[i].entity);
        ge_entity_update(&shop->level_text[i].entity);
        
        shop->entities[MIN_ENTITY_NUM+i] = shop->upgrades[i].entity;
        shop->entities[MIN_ENTITY_NUM+UPGRADE_NUM+i] =
                                                shop->price_text[i].entity;
        shop->entities[MIN_ENTITY_NUM+UPGRADE_NUM*2+i] =
                                                shop->level_text[i].entity;
    }
    
    if(ge_scene_init(&shop->scene, shop->entities,
                     MIN_ENTITY_NUM+UPGRADE_NUM*3, shaders, 1, 0)){
        puts("Failed to init title scene");
        return 1;
    }
    
    shop->money = 0;
    return 0;
}

int shop_on_go_to_title(Shop *shop, int x, int y) {
    /*printf("%d, %d\n", shop->text_x[2], shop->text_y[2]);*/
    x -= shop->text_x[2]-16;
    y -= shop->text_y[2]-8;
    if(x >= 0 && x < shop->go_to_title.text.w*shop->go_to_title.text.size &&
       y >= 0 && y < 16){
        return 1;
    }
    return 0;
}

void shop_click(Shop *shop, Playing *playing, int x, int y) {
    size_t i;
    char buffer[20];
    for(i=0;i<UPGRADE_NUM;i++){
        /*printf("%d, %f, %d, %d\n", shop->text_x[i+MIN_ENTITY_NUM],
               shop->text_x[i+MIN_ENTITY_NUM]+256+shop->upgrades[i].text.w*
               shop->upgrades[i].text.size, shop->text_y[i+MIN_ENTITY_NUM],
               shop->text_y[i+MIN_ENTITY_NUM]+16);*/
        if(x >= shop->text_x[i+MIN_ENTITY_NUM]-16 &&
           x < shop->text_x[i+MIN_ENTITY_NUM]-16+256+shop->upgrades[i].text.w*
               shop->upgrades[i].text.size &&
           y >= shop->text_y[i+MIN_ENTITY_NUM]-8 &&
           y < shop->text_y[i+MIN_ENTITY_NUM]+8){
            if(shop->money >= shop->price[i] &&
               shop->upgrade_level[i] < shop->upgrade_max[i]){
                shop->money -= shop->price[i];
                shop->buy[i](shop, playing, shop->upgrade_level[i],
                             shop->price+i);
                shop->upgrade_level[i]++;
                
                /* Update the price and level text */
                if(shop->upgrade_level[i] < shop->upgrade_max[i]){
                    sprintf(buffer, "$%d", shop->price[i]);
                    ge_text_update(&shop->price_text[i].text, buffer);
                }
                sprintf(buffer, "Lvl.%d/%d", shop->upgrade_level[i],
                        shop->upgrade_max[i]);
                ge_text_update(&shop->level_text[i].text, buffer);
                break;
            }
        }
    }
}

void shop_resize(Shop *shop, int w, int h) {
    shop->w = w;
    shop->h = h;
}

void shop_update_entities(GEEntity *entity, void *data) {
    Shop *shop = data;
    size_t i;
    switch((size_t)entity->extra){
        case 0:
            ge_entity_set_position(entity, -(shop->w-shop->title.text.w*
                                             shop->title.text.size)/2,
                                   -96, 0);
            ge_entity_update(entity);
            break;
        case 1:
            ge_entity_set_position(entity, 0, 100, 0);
            ge_entity_update(entity);
            break;
        case 2:
            shop->text_x[2] = (shop->w-shop->go_to_title.text.w*
                               shop->go_to_title.text.size)/2;
            shop->text_y[2] = shop->h-64;
            ge_entity_set_position(entity, -shop->text_x[2],
                                   -shop->text_y[2], 0);
            ge_entity_update(entity);
            break;
        case 3:
            ge_entity_set_position(entity, -shop->w+16+shop->money_text.text.w*
                                   shop->money_text.text.size, -16, 0);
            ge_entity_update(entity);
            break;
        default:
            if((size_t)entity->extra >= MIN_ENTITY_NUM &&
               (size_t)entity->extra < MIN_ENTITY_NUM+UPGRADE_NUM){
                /* It is the upgrade description */
                i = (size_t)entity->extra-MIN_ENTITY_NUM;
                ge_entity_set_position(entity, -256, -shop->current_y[0],
                                       0);
                shop->current_y[0] += 32;
            }else if((size_t)entity->extra >= MIN_ENTITY_NUM+UPGRADE_NUM &&
                     (size_t)entity->extra < MIN_ENTITY_NUM+UPGRADE_NUM*2){
                /* It is the price text */
                i = (size_t)entity->extra-(MIN_ENTITY_NUM+UPGRADE_NUM);
                ge_entity_set_position(entity, -160, -shop->current_y[1],
                                       0);
                shop->current_y[1] += 32;
            }else if((size_t)entity->extra < MIN_ENTITY_NUM+UPGRADE_NUM*3){
                /* It is the level text */
                i = (size_t)entity->extra-(MIN_ENTITY_NUM+UPGRADE_NUM*2);
                shop->text_x[i+MIN_ENTITY_NUM] = 16;
                shop->text_y[i+MIN_ENTITY_NUM] = shop->current_y[2];
                ge_entity_set_position(entity, -16, -shop->current_y[2],
                                       0);
                shop->current_y[2] += 32;
            }else{
                puts("Unknown entity in the shop scene");
            }
            ge_entity_update(entity);
            break;
    }
}

void shop_render(Shop *shop) {
    char buffer[20];
    shop->current_y[0] = 160;
    shop->current_y[1] = 160;
    shop->current_y[2] = 160;
    sprintf(buffer, "$%d", shop->money);
    ge_text_update(&shop->money_text.text, buffer);
    ge_scene_for_entity(&shop->scene, shop_update_entities, shop);
    ge_scene_update(&shop->scene);
    ge_scene_render(&shop->scene);
}

void shop_free(Shop *shop) {
    (void)shop;
    /* TODO */
}

