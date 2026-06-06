#pragma once
#include "utils.h"
#include <string>

class Music
{
  std::string title;
  std::string genre;
  float duration;
  u16 year;

public:
  Music(std::string &&, std::string &&, float, u16);
  virtual ~Music() = default;

  const std::string to_string();
  const std::string get_path() const;
};
