/**
 * Rastrigin function
 */
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <png.h>

double taget(double x, double y)
{
    double pi = 3.14159265358979323846;
    double tx, ty, tz;
    tx = 5.0 * (x - 0.5);
    ty = 5.0 * (y - 0.5);
    tz = 20.0 + tx * tx + ty * ty - 10.0 * (cos(2.0 * pi * tx) + cos(2.0 * pi * ty));
    return tz / 52.5;
}

double rainbow_r(double v)
{
    double res;
    if (v < 0.0) {
        return 1.0;
    }
    if (v > 1.0) {
        return 0.0;
    }
    if (v < 0.5) {
        res = 1.0 - 2.0 * v;
    } else {
        res = 0.0;
    }
    return res;
}

double rainbow_g(double v)
{
    double res;
    if (v < 0.0) {
        return 0.0;
    }
    if (v > 1.0) {
        return 0.0;
    }
    if (v < 0.5) {
        res = 2.0 * v;
    } else {
        res = 2.0 * (1.0 - v);
    }
    return res;
}

double rainbow_b(double v)
{
    double res;
    if (v < 0.0) {
        return 0.0;
    }
    if (v > 1.0) {
        return 1.0;
    }
    if (v < 0.5) {
        res = 0.0;
    } else {
        res = 2.0 * (v - 0.5);
    }
    return res;
}

/**
 * Image Data Object. "ido" is a shorthand for image data objects. It includes
 * the following properties:
 *
 * image : A "png_image" structure.
 * buffer : A "png_uint_16p" structure, used to storage image pixels data.
 * error : Normally, the value of this property is equal to 0. When an error
 *         occurs, it is not equal to 0.
 * message : If an error occurs, this will be text messages.
 */
struct ido
{
    png_image image;
    png_uint_16p buffer;
    int error;
    char* message;
};

/**
 * Used to create a structure. This structure is experienced by other
 * functions. Specifically, an RGB image with a black background is created,
 * inside memory.
 *
 * width : The width of the image, the pixels, of course.
 * height : The height of the image, also in pixels.
 * return value :  A "ido" structure.
 */
struct ido ido_create(int width, int height)
{
    char* error_no_memory_space = "Failed to request memory space using \"malloc\".\0";
    struct ido image;
    memset(&(image.image), 0, sizeof (image.image));
    image.image.version   = PNG_IMAGE_VERSION;
    image.image.format    = PNG_FORMAT_RGB;
    image.image.width     = width;
    image.image.height    = height;
    if (NULL == (image.buffer = malloc(PNG_IMAGE_SIZE(image.image)))) {
        image.error   = 1;
        image.message = error_no_memory_space;
    }
    image.error = 0;
    return image;
}

/**
 * Save the ido as an image file.
 *
 * image : An IDO structure.
 * file_name : The name of the file used to save.
 * return value : If the save is successful then return 1, otherwise return 0.
 */
int ido_save(struct ido image, const char* file_name)
{
    char* error_save_image_fail = "The image saved failed.\0";
    if (png_image_write_to_file(&(image.image), file_name, 0, image.buffer, 0, NULL)) {
        return 1;
    }
    image.error = 2;
    image.message = error_save_image_fail;
    return 0;
}

/**
 * Free disfigured memory space occupied by image data. Once the call to this
 * function is successful, you can no longer manipulate the "ido" with another
 * function, otherwise an unpredictable error will occur.
 *
 * image : An IDO structure.
 * return value : If it is successful then return 1, otherwise return 0.
 */
int ido_destory(struct ido image)
{
    free(image.buffer);
    image.buffer = NULL;
    return 1;
}

/**
 * Sets the color of the pixels at the specified location of the image. Colors
 * are divided into red, green and blue components, each of which ranges from
 * 0 to 255, including 0 and 255. When the value of the whole color is equal to
 * 0, the color looks black. When the value of the full color is equal to 255,
 * the color looks white.
 *
 * image : An IDO structure.
 * x : The distance from the first pixel to the left of the point that needs
 *     to be set.
 * y : The distance from the first pixel at the top of the point that needs to
 *     be set.
 * r : Red intensity, take the range is 0 to 255.
 * g : Green intensity, take the range is 0 to 255.
 * b : Blue intensity, take the range is 0 to 255.
 * return value : If it is successful then return 1, otherwise return 0.
 */
int ido_setrgb(struct ido image, int x, int y, int r, int g, int b)
{
    char* error_pointer_is_null = "Empty pointer.\0";
    unsigned char* byte_buffer = (unsigned char*)(image.buffer);
    int offset = (x + y * image.image.width) * 3;
    if (!byte_buffer) {
        image.error = 3;
        image.message = error_pointer_is_null;
        return 0;
    }
    byte_buffer[offset] = 0xFF & r;
    byte_buffer[offset + 1] = 0xFF & g;
    byte_buffer[offset + 2] = 0xFF & b;
    return 1;
}

/**
 * Main function
 *
 * @param int argc Number of command parameters
 * @param char** argv The contents of the argument
 * @return int Return 0 if execution is successful
 */
int main(int argc, char *argv[])
{
    struct ido image;
    int len = 1000;
    image = ido_create(len, len);
    int r, g, b;
    double v;
    for (int y = 0; y < len; y++) {
        for (int x = 0; x < len; x++) {
            v = taget((double)x / len, 1.0 - (double)y / len);
            r = 255 * rainbow_r(v);
            g = 255 * rainbow_g(v);
            b = 255 * rainbow_b(v);
            ido_setrgb(image, x, y, r, g, b);
        }
    }
    ido_save(image, "resources/t05_image.png");
    ido_destory(image);
    return 0;
}
