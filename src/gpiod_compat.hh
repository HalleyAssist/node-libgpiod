#ifndef GPIOD_COMPAT_HH
#define GPIOD_COMPAT_HH

#if defined(NODE_LIBGPIOD_MOCK)

#define NODE_LIBGPIOD_USE_MOCK 1

#elif defined(__has_include)

#if __has_include(<gpiod.h>)

#include <gpiod.h>

#else

#define NODE_LIBGPIOD_USE_MOCK 1

#endif

#else

#include <gpiod.h>

#endif

#ifdef NODE_LIBGPIOD_USE_MOCK

#include <cstddef>

struct gpiod_chip;
struct gpiod_line;

static const int GPIOD_LINE_REQUEST_FLAG_OPEN_DRAIN = 1 << 0;
static const int GPIOD_LINE_REQUEST_FLAG_OPEN_SOURCE = 1 << 1;
static const int GPIOD_LINE_REQUEST_FLAG_ACTIVE_LOW = 1 << 2;
static const int GPIOD_LINE_REQUEST_FLAG_BIAS_DISABLE = 1 << 3;
static const int GPIOD_LINE_REQUEST_FLAG_BIAS_PULL_DOWN = 1 << 4;
static const int GPIOD_LINE_REQUEST_FLAG_BIAS_PULL_UP = 1 << 5;

const char *gpiod_version_string();
int gpiod_ctxless_get_value(const char *device, unsigned int offset,
                            bool active_low, const char *consumer);
int gpiod_ctxless_set_value(const char *device, unsigned int offset,
                            int value, bool active_low,
                            const char *consumer, void *data,
                            void *callback);

gpiod_chip *gpiod_chip_open_lookup(const char *device);
void gpiod_chip_close(gpiod_chip *chip);
int gpiod_chip_num_lines(gpiod_chip *chip);
const char *gpiod_chip_name(gpiod_chip *chip);
const char *gpiod_chip_label(gpiod_chip *chip);
gpiod_line *gpiod_chip_get_line(gpiod_chip *chip, unsigned int pin);

void gpiod_line_close_chip(gpiod_line *line);
int gpiod_line_offset(gpiod_line *line);
const char *gpiod_line_name(gpiod_line *line);
const char *gpiod_line_consumer(gpiod_line *line);
int gpiod_line_get_value(gpiod_line *line);
int gpiod_line_set_value(gpiod_line *line, int value);
int gpiod_line_request_input(gpiod_line *line, const char *consumer);
int gpiod_line_request_input_flags(gpiod_line *line, const char *consumer,
                                   int flags);
int gpiod_line_request_output(gpiod_line *line, const char *consumer,
                              int value);
void gpiod_line_release(gpiod_line *line);
#endif

#endif  // GPIOD_COMPAT_HH