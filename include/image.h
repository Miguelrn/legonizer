#ifndef IMAGE_H
#define IMAGE_H 

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} Pixel;

typedef struct {
    int width;
    int height;
    Pixel* data;
} Image;

Image *image_create(int width, int height);
void image_free(Image *img);
void image_set_pixel(Image img, int x, int y, Pixel color);
void image_save_ppm(Image img, const char *filename);

#endif