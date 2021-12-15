#include "png.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t size)
{
    // TODO setup png_image struct and read image from buffer
    png_image image;
    image.version = PNG_IMAGE_VERSION;
    image.opaque = NULL;

    png_image_begin_read_from_memory(&image, Data, size);
    image.format = PNG_FORMAT_GRAY;
    void *img_buffer = malloc(PNG_IMAGE_SIZE(image));

    png_image_finish_read(&image, NULL, img_buffer, PNG_IMAGE_ROW_STRIDE(image), NULL);
    free(img_buffer);
    return 0;
}
