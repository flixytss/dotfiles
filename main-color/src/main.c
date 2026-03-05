#include <pngconf.h>
#include <setjmp.h>
#include <stdio.h>
#include <string.h>
#include <time.h> // png.h requires it
#include <png.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

struct pixel {
   unsigned char r;
   unsigned char g;
   unsigned char b;
   // unsigned char a; In this program it is useless
};

struct pixel get_pixel_png(int x, int y, png_bytep row_ptrs[]);
int get_average(unsigned char arr[], int size);
int print_png_color(const char* file);
int print_jpeg_color(const char* file);

int main(int argc, char** argv) {
    if (argc < 2) { fprintf(stderr, "binary to get the main color of a png or jpg image\n"); return 0; }

    int mode = 0; // 0 = PNG; 1 = JPG
    for (int i = 1; i < argc; i++) {
        const char* str = argv[i];

        if (strcmp(str, "-png") == 0) mode = 0;
        else if (strcmp(str, "-jpeg") == 0) mode = 1;
        else {
            return mode ? print_jpeg_color(str) : print_png_color(str);
        }
    }

    return 0;
}
struct pixel get_pixel_png(int x, int y, png_bytep row_ptrs[]) {
    png_bytep row = row_ptrs[y];
    png_bytep pixel = &(row[x * 4]);

    return (struct pixel){ pixel[0], pixel[1], pixel[2] };
}
struct pixel get_pixel_jpg(int x, int y, int width, unsigned char pixels[]) {
    unsigned char* pixel = pixels + (3 * (width * y + x));

    return (struct pixel){ pixel[0], pixel[1], pixel[2] };
}
int get_average(unsigned char arr[], int size) {
    int average = 0;
    int sum = 0;

    for (int i = 1; i < size; i++) sum += arr[i];
    average = sum / size;

    return average;
}
int print_png_color(const char* file) {
    FILE* fp = fopen(file, "rb");
    if (!fp) {
        fprintf(stderr, "failed when creating FILE* (file %s dosen't exists)!\n", file);
        return 1;
    }

    png_const_bytep sig[8];
    png_uint_32 width, height;
    int rowbytes;
    struct pixel max_pixel;

    fread(sig, 1, 8, fp);
    if (!png_check_sig(sig, 8)) {
        fprintf(stderr, "bad file signature!\n");
        fclose(fp);
        return 1;
    }

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        fprintf(stderr, "bad png pointer (out of memory)!\n");
        fclose(fp);
        return 1;
    }
    png_infop inf_ptr = png_create_info_struct(png_ptr);
    if (!inf_ptr) {
        fprintf(stderr, "bad inf pointer (out of memory)!\n");
        fclose(fp);
        return 1;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "failed setjmp funtion!\n");
        png_destroy_read_struct(&png_ptr, &inf_ptr, NULL);
        fclose(fp);
        return 1;
    }

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);

    png_read_info(png_ptr, inf_ptr);

    height = png_get_image_height(png_ptr, inf_ptr);
    width = png_get_image_width(png_ptr, inf_ptr);

    png_set_strip_16(png_ptr);
    png_set_palette_to_rgb(png_ptr);
    png_set_expand(png_ptr);

    png_read_update_info(png_ptr, inf_ptr);
    rowbytes = png_get_rowbytes(png_ptr, inf_ptr);

    // Allocating pixels
    png_bytep* row_ptrs = (png_bytep*)malloc(sizeof(png_bytep) * height);
    for (int y = 0; y < height; y++)
        row_ptrs[y] = (png_byte*) malloc(rowbytes);

    png_read_image(png_ptr, row_ptrs);

    // Reading pixels
    unsigned char* pixel_r = (unsigned char*)malloc((height * width) * sizeof(unsigned char));
    unsigned char* pixel_g = (unsigned char*)malloc((height * width) * sizeof(unsigned char));
    unsigned char* pixel_b = (unsigned char*)malloc((height * width) * sizeof(unsigned char));

    int i = 0;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            struct pixel p = get_pixel_png(x, y, row_ptrs);
            
            pixel_r[i] = p.r;
            pixel_g[i] = p.g;
            pixel_b[i] = p.b;

            i++;
        }
    }

    printf("%d %d %d\n", get_average(pixel_r, (height * width)), get_average(pixel_g, (height * width)), get_average(pixel_b, (height * width)));
    
    free(pixel_r);
    free(pixel_g);
    free(pixel_b);

    // Freeing
    for (int y = 0; y < height; y++)
        free(row_ptrs[y]);
    free(row_ptrs);

    fclose(fp);
    png_destroy_read_struct(&png_ptr, &inf_ptr, NULL);

    return 0;
}
int print_jpeg_color(const char* file) {
    int width, height, channels;

    unsigned char* image = stbi_load(file, &width, &height, &channels, 3);
    if (!image) {
        fprintf(stderr, "failed when creating unsigned char* (file %s dosen't exists)!\n", file);
        return 1;
    }

    long total_pixels = width * height;

    unsigned char* pixel_r = (unsigned char*)malloc(total_pixels * sizeof(unsigned char));
    unsigned char* pixel_g = (unsigned char*)malloc(total_pixels * sizeof(unsigned char));
    unsigned char* pixel_b = (unsigned char*)malloc(total_pixels * sizeof(unsigned char));

    int i = 0;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            struct pixel p = get_pixel_jpg(x, y, width, image);
            
            pixel_r[i] = p.r;
            pixel_g[i] = p.g;
            pixel_b[i] = p.b;

            i++;
        }
    }

    printf("%d %d %d\n", get_average(pixel_r, total_pixels), get_average(pixel_g, total_pixels), get_average(pixel_b, total_pixels));

    stbi_image_free(image);
    free(pixel_r);
    free(pixel_g);
    free(pixel_b);

    return 0;
}