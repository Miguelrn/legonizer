#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "raylib.h"

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

void lego_filter(Image *img, int brickSize) {
    int width = img->width;
    int height = img->height;

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

            // ====================================
            // Average block color
            // ====================================
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

            Color base = {r, g, b, 255};

            // ====================================
            // Draw LEGO stud
            // ====================================

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

    memcpy(src, dst, width * height * sizeof(Color));

    free(dst);
}