#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <png.h>
#include <string.h>

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
struct ido ido_new(int width, int height)
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

/*
 * 一个具有质量的点
 *
 */
struct point {
    double x;   /* 横坐标 */
    double y;   /* 纵坐标 */
    double m;   /* 质量 */
    double vx;  /* 横速度 */
    double vy;  /* 纵速度 */
    double fx;  /* 横作用力 */
    double fy;  /* 纵作用力 */
};

/*
 * 新建一个点，并且给点设置初始化的坐标
 *
 * x 横坐标
 * y 纵坐标
 *
 */
struct point *point_new(double x, double y) {
    struct point *p;
    p = (struct point *)malloc(sizeof(struct point));
    p->x = x;
    p->y = y;
    p->m = 1;
    p->vx = 0;
    p->vy = 0;
    p->fx = 0;
    p->fy = 0;
    return p;
}

/*
 * 释放点占据的内存
 *
 * p 点
 *
 */
void point_distory(struct point *p) {
    free(p);
}

/*
 * 模拟器
 *
 */
struct simulation {
    struct point **points;
    int point_num;
};

/*
 * 新建模拟器
 *
 * point_num 点的数量
 *
 */
struct simulation *simulation_new(int point_num) {
    struct simulation *s;
    int i;
    s = (struct simulation *)malloc(sizeof(struct simulation));
    s->points = (struct point **)calloc(point_num, sizeof(struct point *));
    s->point_num = point_num;
    for (i = 0; i < point_num; i++) {
        s->points[i] = point_new((double)rand() / RAND_MAX, (double)rand() / RAND_MAX);
    }
    return s;
}

/*
 * 运行模拟
 *
 * @method
 * @private
 *
 * s 模拟器
 *
 */
void __simulation_calculate_clean_f(struct simulation *s) {
    int i;
    for (i = 0; i < s->point_num; i++) {
        s->points[i]->fx = 0;
        s->points[i]->fy = 0;
    }
}

/*
 * 计算引力
 *
 * @method
 * @private
 *
 */
void __simulation_calculate_points_f(struct point *a, struct point *b) {
    double r, r2, dx, dy, f, fx, fy;
    dx = a->x - b->x;
    dy = a->y - b->y;
    r2 = dx * dx + dy * dy;
    r = sqrt(r2);
    f = a->m * b->m / r2;
    fx = dx / r * f;
    fy = dy / r * f;
    a->fx -= fx;
    a->fy -= fy;
    b->fx += fx;
    b->fy += fy;
}

/*
 * 运行模拟
 *
 * @method
 * @protected
 *
 * s 模拟器
 *
 */
void _simulation_calculate_f(struct simulation *s) {
    int i, j, ti, tj;
    ti = s->point_num - 1;
    tj = s->point_num;
    __simulation_calculate_clean_f(s);
    for (i = 0; i < ti; i++) {
        for (j = 1 + i; j < tj; j++) {
            __simulation_calculate_points_f(s->points[i], s->points[j]);
        }
    }
}

/*
 * 运行模拟
 *
 * @method
 * @protected
 *
 * s 模拟器
 *
 */
void _simulation_calculate_update_xy_and_v(struct simulation *s, double t) {
    /* x=0.5*f*t^2/m+v0*t
       v=v0+f/m*t          */
    int i;
    for (i = s->point_num - 1; i > -1; i--) {
        s->points[i]->x += (0.5 * s->points[i]->fx * t * t + s->points[i]->vx * t);
        s->points[i]->vx += (s->points[i]->vx + s->points[i]->fx * t);
        s->points[i]->y += (0.5 * s->points[i]->fy * t * t + s->points[i]->vy * t);
        s->points[i]->vy += (s->points[i]->vy + s->points[i]->fy * t);
    }
}

/*
 * 运行模拟
 *
 * @method
 * @protected
 *
 * s 模拟器
 *
 */
void _simulation_save_image(struct simulation *s, int idx) {
    struct ido image;
    int i, j, x, y;
    char file_name[200];
    image = ido_new(100, 100);
    for (i = 0; i < 100; i++) {
        for (j = 0; j < 100; j++) {
            ido_setrgb(image, i, j, 0, 0, 0);
        }
    }
    for (i = 0; i < s->point_num; i++) {
        x = (int)(s->points[i]->x * 100);
        y = (int)(s->points[i]->y * 100);
        if (x < 0 || x > 255 || y < 0 || y > 255) {
            continue;
        }
        ido_setrgb(image, x, y, 255, 255, 255);
    }
    sprintf(file_name, "resources/t06_image_%03d.png", idx);
    printf("Write image(%d).\n", idx);
    ido_save(image, file_name);
    ido_destory(image);
}

/*
 * 运行模拟
 *
 * s 模拟器
 * t 时间
 *
 */
void simulation_run(struct simulation *s, double t, int i) {
    _simulation_calculate_f(s);
    _simulation_calculate_update_xy_and_v(s, t);
    _simulation_save_image(s, i);
}

/*
 * 释放点占据的内存
 *
 * p 点
 *
 */
void simulation_distory(struct simulation *s) {
    int i;
    for (i = 0; i < s->point_num; i++) {
        point_distory(s->points[i]);
    }
    free(s->points);
    free(s);
}

int main(void) {
    int i;
    struct simulation *s = simulation_new(10);
    for (i = 0; i < 60; i++) {
        simulation_run(s, 0.001, i);
    }
    simulation_distory(s);
    return 0;
}