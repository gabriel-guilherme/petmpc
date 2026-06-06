#pragma once

#include "utils.h"
#include <ao/ao.h>
#include <memory>
#include <mpg123.h>
#include <string>

class LibPlayMP3
{
  // Primitive types
  std::size_t buffer_size, done;
  i32 driver, err, channels, encoding;
  long rate;
  std::string track;

  // LibMPG123 type and smart pointer
  mpg123_handle *mh;
  std::shared_ptr<char> buffer;

  // AO types
  ao_sample_format format;
  ao_device *dev;

public:
  LibPlayMP3();
  ~LibPlayMP3();
  void music(const char *);
  void play();
};
