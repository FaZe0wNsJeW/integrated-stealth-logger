#ifndef STB_IMAGE_WRITE_H
#define STB_IMAGE_WRITE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char stbi_uc;

int stbi_write_png(char const *filename, int w, int h, int comp, const void *data, int stride_in_bytes);
int stbi_write_bmp(char const *filename, int w, int h, int comp, const void *data);
int stbi_write_tga(char const *filename, int w, int h, int comp, const void *data);
int stbi_write_hdr(char const *filename, int w, int h, int comp, const float *data);

int stbi_write_png_to_func(void (*func)(void *context, void *data, int size), void *context, int w, int h, int comp, const void *data, int stride_in_bytes);
int stbi_write_bmp_to_func(void (*func)(void *context, void *data, int size), void *context, int w, int h, int comp, const void *data);
int stbi_write_tga_to_func(void (*func)(void *context, void *data, int size), void *context, int w, int h, int comp, const void *data);
int stbi_write_hdr_to_func(void (*func)(void *context, void *data, int size), void *context, int w, int h, int comp, const float *data);

#ifdef __cplusplus
}
#endif

#endif
