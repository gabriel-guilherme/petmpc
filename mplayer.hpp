#pragma once

#include "utils.h"
#include <ao/ao.h>
#include <atomic>
#include <iostream>
#include <memory>
#include <mpg123.h>
#include <string>

class MPlayer
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
  MPlayer();
  ~MPlayer();
  void reset();
  // void debug()
  // {
  //   std::cout << "first 16 bytes: ";
  //   for (size_t i = 0; i < std::min(buffer_size, size_t(16)); ++i)
  //   {
  //     std::cout << std::hex << (int)(unsigned char)buffer.get()[i] << " ";
  //   }
  //   std::cout << std::dec << std::endl;
  // };
  void music(const char *);
  void play(const std::atomic<bool> &, const std::atomic<bool> &);
};
