#ifndef LINE_HH
#define LINE_HH

#include <gpiod.h>
#include <nan.h>

#include "chip.hh"

class Line : public Nan::ObjectWrap {
 public:
  static NAN_MODULE_INIT(Init);
  static NAN_METHOD(New);
  gpiod_line *getNativeLine();

 private:
  explicit Line(Chip *chip, unsigned int pin);
  ~Line();

  static NAN_METHOD(getValue);
  static NAN_METHOD(setValue);

  static NAN_METHOD(requestInput);
  static NAN_METHOD(requestOutput);
  static NAN_METHOD(requestBoth);

  static Nan::Persistent<v8::Function> constructor;

  gpiod_line *line;
};

#endif  // LINE_HH