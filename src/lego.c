#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "raylib.h"
#include "stdio.h"

typedef struct {
    const char *name;
    int id;
    unsigned char r;
    unsigned char g;
    unsigned char b;
} LegoColor;

static LegoColor lego_palette[] =
{
    {"White",                    1,   255,255,255},
    {"Brick Yellow",             5,   217,187,123},
    {"Nougat",                   18,  214,114,64},
    {"Bright Red",               21,  221,26,34},
    {"Bright Blue",              23,  0,108,181},
    {"Bright Yellow",            24,  255,204,3},
    {"Black",                    26,  0,0,0},
    {"Dark Green",               28,  0,153,0},
    {"Bright Green",             37,  0,204,0},
    {"Dark Orange",              38,  168,61,21},

    {"Medium Blue",              102, 71,140,198},
    {"Bright Orange",            106, 247,124,30},
    {"Bright Bluish Green",      107, 5,157,158},
    {"Bright Yellowish Green",   119, 149,185,11},
    {"Bright Reddish Violet",    124, 153,0,102},

    {"Sand Blue",                135, 94,116,140},
    {"Sand Yellow",              138, 141,116,82},
    {"Earth Blue",               140, 0,37,65},
    {"Earth Green",              141, 0,51,0},
    {"Sand Green",               151, 95,130,101},

    {"Dark Red",                 154, 128,8,27},
    {"Flame Yellowish Orange",   191, 252,170,22},
    {"Reddish Brown",            192, 91,28,12},
    {"Medium Stone Grey",        194, 156,146,145},
    {"Dark Stone Grey",          199, 76,81,86},

    {"Light Stone Grey",         208, 228,228,218},
    {"Light Royal Blue",         212, 135,192,234},
    {"Bright Purple",            221, 222,55,139},
    {"Light Purple",             222, 238,157,195},
    {"Cool Yellow",              226, 255,255,153},

    {"Dark Purple",              268, 44,21,119},
    {"Light Nougat",             283, 245,193,137},
    {"Dark Brown",               308, 48,15,6},
    {"Medium Nougat",            312, 170,125,85},

    {"Dark Azur",                321, 70,155,195},
    {"Medium Azur",              322, 104,195,226},
    {"Aqua",                     323, 211,242,234},
    {"Medium Lavender",          324, 160,110,185},
    {"Lavender",                 325, 205,164,222},

    {"White Glow",               329, 245,243,215},
    {"Spring Yellowish Green",   330, 226,249,154},
    {"Olive Green",              331, 119,119,78},
    {"Medium Yellowish Green",   332, 150,185,59},

    {"Vibrant Coral",            353, 244,132,124},
    {"Vibrant Yellow",           368, 255,252,0},
    {"Reddish Orange",           402, 255,75,30}
};

static Color clamp_color(int r, int g, int b)
{
    if (r < 0) r = 0;
    if (r > 255) r = 255;

    if (g < 0) g = 0;
    if (g > 255) g = 255;

    if (b < 0) b = 0;
    if (b > 255) b = 255;

    return (Color){r, g, b, 255};
}

int nearest_lego_index(Color c)
{
    int bestDist = INT_MAX;
    int bestIndex = 0;

    int count =
        sizeof(lego_palette) /
        sizeof(lego_palette[0]);

    for (int i = 0; i < count; i++)
    {
        int dr = c.r - lego_palette[i].r;
        int dg = c.g - lego_palette[i].g;
        int db = c.b - lego_palette[i].b;

        int dist = dr*dr + dg*dg + db*db;

        if (dist < bestDist)
        {
            bestDist = dist;
            bestIndex = i;
        }
    }

    return bestIndex;
}

void lego_filter(Image *img, int brickSize) {
    int width = img->width;
    int height = img->height;
    int paletteCount = sizeof(lego_palette) / sizeof(lego_palette[0]);
    int counts[64] = {0};

    Color *src = (Color *)img->data;

    Color *dst = malloc(width * height * sizeof(Color));

    for (int by = 0; by < height; by += brickSize)
    {
        for (int bx = 0; bx < width; bx += brickSize)
        {
            int r = 0;
            int g = 0;
            int b = 0;
            int count = 0;

            for (int y = 0; y < brickSize; y++)
            {
                for (int x = 0; x < brickSize; x++)
                {
                    int px = bx + x;
                    int py = by + y;

                    if (px >= width || py >= height)
                        continue;

                    Color c = src[py * width + px];

                    r += c.r;
                    g += c.g;
                    b += c.b;

                    count++;
                }
            }

            r /= count;
            g /= count;
            b /= count;

            int nearestIndex = nearest_lego_index((Color){r, g, b, 255});
            counts[nearestIndex]++;
            
            Color base = {
                lego_palette[nearestIndex].r,
                lego_palette[nearestIndex].g,
                lego_palette[nearestIndex].b,
                255
            };

            float radius = brickSize * 0.38f;
            float cx = bx + brickSize / 2.0f;
            float cy = by + brickSize / 2.0f;

            for (int y = 0; y < brickSize; y++)
            {
                for (int x = 0; x < brickSize; x++)
                {
                    int px = bx + x;
                    int py = by + y;

                    if (px >= width || py >= height)
                        continue;

                    float dx = px - cx;
                    float dy = py - cy;

                    float dist = sqrtf(dx * dx + dy * dy);

                    Color out = base;

                    // ====================================
                    // Circular stud shading
                    // ====================================

                    if (dist < radius)
                    {
                        // fake directional light
                        float light = 1.15f - (dist / radius) * 0.35f;

                        // top-left highlight
                        if (dx < 0 && dy < 0)
                            light += 0.15f;

                        out = clamp_color(
                            base.r * light,
                            base.g * light,
                            base.b * light
                        );
                    }
                    else
                    {
                        // darker edge between bricks
                        out = clamp_color(
                            base.r * 0.75f,
                            base.g * 0.75f,
                            base.b * 0.75f
                        );
                    }

                    dst[py * width + px] = out;
                }
            }
        }
    }

    for (int i = 0; i < paletteCount; i++){
        if (counts[i] > 0){
            printf(
                "%-25s %5d\n",
                lego_palette[i].name,
                counts[i]
            );
        }
    }
    memcpy(src, dst, width * height * sizeof(Color));

    free(dst);
}