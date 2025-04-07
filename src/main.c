#include <stdio.h>
#include <stdlib.h>

#include <game.h>

#include <mibiengine2/base/window.h>

typedef struct {
    Game game;
    GEWindow window;
} Data;

Data data;

void draw(void *_data) {
    (void)_data;
    game_render(&data.game, &data.window);
}

void resize(void *_data, int w, int h) {
    (void)_data;
    game_resize(&data.game, &data.window, w, h);
}

void keyevent(void *_data, int key, int released) {
    (void)_data;
    (void)key;
    (void)released;
    if(released) game_keypress(&data.game, &data.window, key);
}

void mouseevent(void *_data, int x, int y, GEWindowMouseEvent event,
                int released) {
    (void)_data;
    (void)x;
    (void)y;
    (void)event;
    (void)released;
    if(event == GE_B_LEFT && released){
        game_click(&data.game, &data.window, x, y);
    }
}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    
    if(ge_window_init(&data.window, "GOLD HUNGER")){
        return EXIT_FAILURE;
    }
    
    if(game_init(&data.game, &data.window)){
        ge_window_free(&data.window);
        return EXIT_FAILURE;
    }
    
    ge_window_set_callbacks(&data.window, draw, resize, keyevent, mouseevent);
    
    ge_window_depth_test(&data.window, 0);
    ge_window_blending(&data.window, 1);
    
    ge_window_mainloop(&data.window);
    
    game_free(&data.game);
    
    ge_window_free(&data.window);
    
    return EXIT_SUCCESS;
}
