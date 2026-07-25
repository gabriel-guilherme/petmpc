#pragma once
#include "utils.h"
#include <format>
#include <string>

struct Music
{
  std::string title;
  std::string genre;
  u16 id;
  float duration;
  u16 year;
  Music(u16 i, std::string &&t, std::string &&g, float d, u16 y)
      : title(t), genre(g), id(i), duration(d), year(y)
  {
  }
  virtual ~Music() = default;

  const std::string to_string()
  {
    return std::format("({}) {} - {} [{}]\n", id, title, year, duration);
  };
  const std::string get_path() const
  {
    return std::string("assets/" + this->title + ".mp3");
  };
};
