#ifndef NODE_LIBGPIOD_HH
#define NODE_LIBGPIOD_HH

#include <iostream>

#include <nan.h>

#include "gpiod_compat.hh"

NAN_METHOD(version);
NAN_METHOD(getInstantLineValue);
NAN_METHOD(setInstantLineValue);

NAN_MODULE_INIT(InitAll);

#endif  // NODE_LIBGPIOD_HH