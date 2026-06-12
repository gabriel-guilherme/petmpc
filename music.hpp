#pragma once
#include "utils.h"
#include <string>

class Music
{
  u16 id;
  std::string title;
  std::string genre;
  float duration;
  u16 year;

public:
  Music(u16, std::string &&, std::string &&, float, u16);
  virtual ~Music() = default;

  const std::string to_string();
  const std::string get_path() const;
  const u16 get_id() { return id; };
};
