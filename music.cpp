#include "music.hpp"
#include <sstream>

Music::Music(std::string &&t, std::string &&g, float d, u16 y)
    : title(t), genre(g), duration(d), year(y)
{
}

const std::string Music::to_string()
{
  std::stringstream ss;
  ss << title << " - " << year << " [" << duration << "] \n";
  return ss.str();
}

const std::string Music::get_path() const
{
  return std::string("assets/" + this->title + ".mp3");
}
