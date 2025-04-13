#include <progress_bar.h>
#include <stdio.h>

void progressbar_draw_image(ProgressBar *bar, int value, int max) {
    int end;
    unsigned int x, y;
    
    if(value > max) value = max;
    if(value < 0) value = 0;
    if(max < 1) max = 1;
    /*printf("%d, %d\n", value, max);*/
    end = ((bar->image.width-2)/(float)max)*value;
    
    for(y=0;y<bar->image.height;y++){
        for(x=0;x<bar->image.width;x++){
            if(y > 0 && y < bar->image.height-1 && x > 0 && (int)x <= end){
                bar->image.data[(y*bar->image.width+x)*4] = bar->r1;
                bar->image.data[(y*bar->image.width+x)*4+1] = bar->g1;
                bar->image.data[(y*bar->image.width+x)*4+2] = bar->b1;
            }else{
                bar->image.data[(y*bar->image.width+x)*4] = bar->r2;
                bar->image.data[(y*bar->image.width+x)*4+1] = bar->g2;
                bar->image.data[(y*bar->image.width+x)*4+2] = bar->b2;
            }
            bar->image.data[(y*bar->image.width+x)*4+3] = 255;
        }
    }
}

int progress_bar_init(ProgressBar *bar, GEStdShader *shader, int w, int h,
                      int value, int max, int r1, int g1, int b1, int r2,
                      int g2, int b2, float scale) {
    if(ge_image_empty(&bar->image, w, h)){
        puts("Failed to create progress bar image");
        return 1;
    }
    
    bar->r1 = r1;
    bar->g1 = g1;
    bar->b1 = b1;
    
    bar->r2 = r2;
    bar->g2 = g2;
    bar->b2 = b2;
    
    progressbar_draw_image(bar, value, max);
    
    if(ge_texture_init(&bar->texture, &bar->image, 0, 0)){
        puts("Failed to init progress bar texture");
        return 1;
    }
    if(ge_sprite_init(&bar->sprite, &bar->texture, shader, w*scale, h*scale)){
        puts("Failed to create progress bar sprite");
        return 1;
    }
    if(ge_loader_model_renderable(&bar->renderable,
                                  &GE_SPRITE_MODEL(&bar->sprite), shader)){
        puts("Failed to create progress bar renderable");
        return 1;
    }
    if(ge_entity_init(&bar->entity, &bar->renderable)){
        puts("Failed to create progress bar entity");
        return 1;
    }
    return 0;
}

int progressbar_update(ProgressBar *bar, int value, int max) {
    progressbar_draw_image(bar, value, max);
    if(ge_texture_update(&bar->texture, &bar->image)){
        puts("Failed to create progress bar sprite");
        return 1;
    }
    return 0;
}

void progressbar_free(ProgressBar *bar) {
    ge_image_free(&bar->image);
    ge_sprite_free(&bar->sprite);
    ge_texture_free(&bar->texture);
    ge_renderable_free(&bar->renderable);
}

