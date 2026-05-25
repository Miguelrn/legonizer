#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "raylib.h"
#include "lego.h"

typedef struct {
    Rectangle bounds;
    const char *text;
} Button;

void negative(Image *img){
    Color *pixels = (Color *)img->data;

    for (int i = 0; i < img->width * img->height; i++){
        pixels[i].r = 255 - pixels[i].r;
        pixels[i].g = 255 - pixels[i].g;
        pixels[i].b = 255 - pixels[i].b;
    }
}

void black_white(Image *img) {
    Color *pixels = (Color *)img->data;

    for(int i = 0; i < img->width; ++i) {
        for(int j = 0; j < img->height; ++j){
            const int index = j * img->width + i;
            unsigned char gray = (pixels[index].r + pixels[index].g + pixels[index].b) / 3;
            pixels[index].r = gray; 
            pixels[index].g = gray;
            pixels[index].b = gray;
        }
    }
}

void blur(Image *img, int radius) {
    int w = img->width;
    int h = img->height;
    Color *src = (Color *)img->data;
    Color *dst = malloc(w * h * sizeof(Color));

    for (int y = 0; y < h; y++){
        for (int x = 0; x < w; x++){
            int r = 0, g = 0, b = 0;

            for (int j = -radius; j <= radius; j++){
                for (int i = -radius; i <= radius; i++){
                    int nx = x + i;
                    int ny = y + j;

                    if (nx >= 0 && ny >= 0 && nx < w && ny < h){
                        Color c = src[ny * w + nx];
                        r += c.r;
                        g += c.g;
                        b += c.b;
                    }
                }
            }

            int idx = y * w + x;
            dst[idx].r = r / ((2 * radius + 1) * (2 * radius + 1));
            dst[idx].g = g / ((2 * radius + 1) * (2 * radius + 1));
            dst[idx].b = b / ((2 * radius + 1) * (2 * radius + 1));
            dst[idx].a = 255;
        }
    }

    memcpy(src, dst, w * h * sizeof(Color));
    free(dst);
}

void sharpen(Image *img){
    
    int w = img->width;
    int h = img->height;
    Color *src = (Color *)img->data;
    Color *dst = malloc(w * h * sizeof(Color));

    for (int y = 1; y < h - 1; y++)
    {
        for (int x = 1; x < w - 1; x++)
        {
            Color c  = src[y * w + x];
            Color l  = src[y * w + (x - 1)];
            Color r  = src[y * w + (x + 1)];
            Color u  = src[(y - 1) * w + x];
            Color d  = src[(y + 1) * w + x];

            int rr = c.r * 5 - l.r - r.r - u.r - d.r;
            int gg = c.g * 5 - l.g - r.g - u.g - d.g;
            int bb = c.b * 5 - l.b - r.b - u.b - d.b;

            int idx = y * w + x;

            dst[idx].r = (rr < 0) ? 0 : (rr > 255 ? 255 : rr);
            dst[idx].g = (gg < 0) ? 0 : (gg > 255 ? 255 : gg);
            dst[idx].b = (bb < 0) ? 0 : (bb > 255 ? 255 : bb);
            dst[idx].a = 255;
        }
    }

    memcpy(src, dst, w * h * sizeof(Color));
    free(dst);
}

void edge_detect(Image *img){
    Color *src = (Color *)img->data;
    Color *dst = malloc(img->width * img->height * sizeof(Color));

    int w = img->width;
    int h = img->height;

    for (int y = 1; y < h - 1; y++)
    {
        for (int x = 1; x < w - 1; x++)
        {
            int gx = 0;
            int gy = 0;

            int kernelX[3][3] = {
                {-1, 0, 1},
                {-2, 0, 2},
                {-1, 0, 1}
            };

            int kernelY[3][3] = {
                {-1, -2, -1},
                { 0,  0,  0},
                { 1,  2,  1}
            };

            for (int j = -1; j <= 1; j++)
            {
                for (int i = -1; i <= 1; i++)
                {
                    Color c = src[(y + j) * w + (x + i)];
                    int intensity = (c.r + c.g + c.b) / 3;

                    gx += intensity * kernelX[j + 1][i + 1];
                    gy += intensity * kernelY[j + 1][i + 1];
                }
            }

            int mag = (int)sqrt(gx * gx + gy * gy);
            if (mag > 255) mag = 255;

            int idx = y * w + x;
            dst[idx].r = mag;
            dst[idx].g = mag;
            dst[idx].b = mag;
            dst[idx].a = 255;
        }
    }

    memcpy(src, dst, w * h * sizeof(Color));
    free(dst);
}

bool button_draw(Button btn){
    Vector2 mouse = GetMousePosition();

    bool hovered = CheckCollisionPointRec(mouse, btn.bounds);

    Color color = hovered ? LIGHTGRAY : GRAY;

    DrawRectangleRec(btn.bounds, color);

    DrawText(
        btn.text,
        btn.bounds.x + 10,
        btn.bounds.y + 10,
        20,
        BLACK
    );

    return hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

void load_lenna(void){
    Image img = LoadImage("assets/lenna.png");
    const int screenWidth = img.width;
    const int screenHeight = img.height;
    const int sidebarWidth = 160;
    InitWindow(screenWidth+sidebarWidth, screenHeight, "Pixeler");

    ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);

    Texture2D lenna = LoadTextureFromImage(img);
    Image working = ImageCopy(img);

    Button blurBtn = {
        .bounds = {20, 20, 120, 40},
        .text = "Blur"
    };

    Button sharpBtn = {
        .bounds = {20, 70, 120, 40},
        .text = "Sharpen"
    };

    Button edgeBtn = {
        .bounds = {20, 120, 120, 40},
        .text = "Edge"
    };

    Button negativeBtn = {
        .bounds = {20, 170, 120, 40},
        .text = "Negative"
    };

    Button whiteBtn = {
        .bounds = {20, 220, 120, 40},
        .text = "B & W"
    };

    Button originalBtn = {
        .bounds = {20, 270, 120, 40},
        .text = "Original"
    };

    Button legoBtn = {
        .bounds = {20, 270, 120, 40},
        .text = "Legonizer"
    };

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();

        if (button_draw(blurBtn)){
            working = ImageCopy(img);
            blur(&working, 3);
            lenna = LoadTextureFromImage(working);
            UnloadImage(working);
        }

         if (button_draw(sharpBtn)){
            working = ImageCopy(img);
            sharpen(&working);
            lenna = LoadTextureFromImage(working);
            UnloadImage(working);
        }

         if (button_draw(edgeBtn)){
            working = ImageCopy(img);
            edge_detect(&working);
            lenna = LoadTextureFromImage(working);
            UnloadImage(working);
        }

         if (button_draw(negativeBtn)){
            working = ImageCopy(img);
            negative(&working);
            lenna = LoadTextureFromImage(working);
            UnloadImage(working);
        }

        if (button_draw(whiteBtn)){
            working = ImageCopy(img);
            black_white(&working);
            lenna = LoadTextureFromImage(working);
            UnloadImage(working);
        }

        if (button_draw(originalBtn)){
            working = ImageCopy(img);
            lenna = LoadTextureFromImage(working);
            UnloadImage(working);
        }

        if (button_draw(legoBtn)){
            working = ImageCopy(img);
            lego_filter(&working, 20);
            lenna = LoadTextureFromImage(working);
            UnloadImage(working);
        }

        ClearBackground(BLACK);

        DrawTexture(lenna, sidebarWidth, 0, WHITE);

        EndDrawing();
    }

    UnloadTexture(lenna);

    CloseWindow();
}