#include "gpiod_compat.hh"

#ifdef NODE_LIBGPIOD_USE_MOCK

#include <cerrno>
#include <cstdio>
#include <map>
#include <string>

constexpr unsigned int kMockLineCount = 256;

struct gpiod_chip {
  std::string device;
  std::string name;
  std::string label;
  std::map<unsigned int, int> values;
};

struct gpiod_line {
  gpiod_chip *chip;
  unsigned int offset;
  std::string name;
  std::string consumer;
  int flags;
  int value;
  bool requested;
  bool output;
  bool released;
};

namespace {

bool isValidMockDevice(const std::string &device) {
  return device == "0" || device == "gpiochip0" || device == "/dev/gpiochip0";
}

int invertIfActiveLow(int value, bool active_low) {
  if (!active_low) {
    return value;
  }
  return value == 0 ? 1 : 0;
}

void printAction(const char *action, const char *device, unsigned int offset,
                 const char *consumer, int value, int flags) {
  std::printf("[node-libgpiod mock] %s device=%s line=%u consumer=%s value=%d flags=%d\n",
              action, device, offset, consumer ? consumer : "", value, flags);
}

const char *defaultConsumer(const std::string &consumer) {
  return consumer.empty() ? "" : consumer.c_str();
}

}  // namespace

const char *gpiod_version_string() {
  return "mock-gpiod-wsl";
}

int gpiod_ctxless_get_value(const char *device, unsigned int offset,
                            bool active_low, const char *consumer) {
  if (!device || !isValidMockDevice(device) || offset >= kMockLineCount) {
    errno = ENODEV;
    return -1;
  }

  printAction("getInstantLineValue", device, offset, consumer, 0, 0);
  return invertIfActiveLow(0, active_low);
}

int gpiod_ctxless_set_value(const char *device, unsigned int offset,
                            int value, bool active_low,
                            const char *consumer, void *data,
                            void *callback) {
  (void)data;
  (void)callback;
  if (!device || !isValidMockDevice(device) || offset >= kMockLineCount) {
    errno = ENODEV;
    return -1;
  }
  if (value != 0 && value != 1) {
    errno = EINVAL;
    return -1;
  }

  printAction("setInstantLineValue", device, offset, consumer,
              invertIfActiveLow(value, active_low), 0);
  return 0;
}

gpiod_chip *gpiod_chip_open_lookup(const char *device) {
  std::string resolved = device ? device : "0";
  if (!isValidMockDevice(resolved)) {
    errno = ENODEV;
    return NULL;
  }

  gpiod_chip *chip = new gpiod_chip();
  chip->device = resolved;
  chip->name = "gpiochip0";
  chip->label = "WSL mock gpio chip";
  return chip;
}

void gpiod_chip_close(gpiod_chip *chip) {
  delete chip;
}

int gpiod_chip_num_lines(gpiod_chip *chip) {
  if (!chip) {
    errno = EINVAL;
    return -1;
  }
  return static_cast<int>(kMockLineCount);
}

const char *gpiod_chip_name(gpiod_chip *chip) {
  if (!chip) {
    errno = EINVAL;
    return NULL;
  }
  return chip->name.c_str();
}

const char *gpiod_chip_label(gpiod_chip *chip) {
  if (!chip) {
    errno = EINVAL;
    return NULL;
  }
  return chip->label.c_str();
}

gpiod_line *gpiod_chip_get_line(gpiod_chip *chip, unsigned int pin) {
  if (!chip || pin >= kMockLineCount) {
    errno = EINVAL;
    return NULL;
  }

  gpiod_line *line = new gpiod_line();
  line->chip = chip;
  line->offset = pin;
  line->name = "mock-line-" + std::to_string(pin);
  line->consumer.clear();
  line->flags = 0;
  line->value = 0;
  line->requested = false;
  line->output = false;
  line->released = false;

  std::map<unsigned int, int>::const_iterator existing = chip->values.find(pin);
  if (existing != chip->values.end()) {
    line->value = existing->second;
  }

  return line;
}

void gpiod_line_close_chip(gpiod_line *line) {
  delete line;
}

int gpiod_line_offset(gpiod_line *line) {
  if (!line || line->released) {
    errno = EINVAL;
    return -1;
  }
  return static_cast<int>(line->offset);
}

const char *gpiod_line_name(gpiod_line *line) {
  if (!line || line->released) {
    errno = EINVAL;
    return NULL;
  }
  return line->name.c_str();
}

const char *gpiod_line_consumer(gpiod_line *line) {
  if (!line || line->released || line->consumer.empty()) {
    return NULL;
  }
  return line->consumer.c_str();
}

int gpiod_line_get_value(gpiod_line *line) {
  if (!line || line->released) {
    errno = EINVAL;
    return -1;
  }
  printAction("getValue", line->chip->device.c_str(), line->offset,
              defaultConsumer(line->consumer), line->value, line->flags);
  return line->value;
}

int gpiod_line_set_value(gpiod_line *line, int value) {
  if (!line || line->released) {
    errno = EINVAL;
    return -1;
  }
  if (value != 0 && value != 1) {
    errno = EINVAL;
    return -1;
  }

  line->value = value;
  line->chip->values[line->offset] = value;
  printAction("setValue", line->chip->device.c_str(), line->offset,
              defaultConsumer(line->consumer), value, line->flags);
  return 0;
}

int gpiod_line_request_input(gpiod_line *line, const char *consumer) {
  if (!line || line->released) {
    errno = EINVAL;
    return -1;
  }

  line->consumer = consumer ? consumer : "";
  line->requested = true;
  line->output = false;
  line->flags = 0;
  printAction("requestInputMode", line->chip->device.c_str(), line->offset,
              defaultConsumer(line->consumer), line->value, line->flags);
  return 0;
}

int gpiod_line_request_input_flags(gpiod_line *line, const char *consumer,
                                   int flags) {
  if (gpiod_line_request_input(line, consumer) == -1) {
    return -1;
  }

  line->flags = flags;
  printAction("requestInputModeFlags", line->chip->device.c_str(),
              line->offset, defaultConsumer(line->consumer), line->value,
              line->flags);
  return 0;
}

int gpiod_line_request_output(gpiod_line *line, const char *consumer,
                              int value) {
  if (!line || line->released) {
    errno = EINVAL;
    return -1;
  }
  if (value != 0 && value != 1) {
    errno = EINVAL;
    return -1;
  }

  line->consumer = consumer ? consumer : "";
  line->requested = true;
  line->output = true;
  line->value = value;
  line->chip->values[line->offset] = value;
  printAction("requestOutputMode", line->chip->device.c_str(), line->offset,
              defaultConsumer(line->consumer), value, line->flags);
  return 0;
}

void gpiod_line_release(gpiod_line *line) {
  if (!line || line->released) {
    return;
  }

  printAction("release", line->chip->device.c_str(), line->offset,
              defaultConsumer(line->consumer), line->value, line->flags);
  line->released = true;
}

#endif