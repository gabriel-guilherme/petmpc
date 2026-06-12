#include "music.hpp"
#include <sstream>

Music::Music(u16 i, std::string &&t, std::string &&g, float d, u16 y)
    : id(i), title(t), genre(g), duration(d), year(y)
{
}

const std::string Music::to_string()
{
  std::stringstream ss;
  ss << "(" << id << ")" << title << " - " << year << " [" << duration
     << "] \n";
  return ss.str();
}

const std::string Music::get_path() const
{
  return std::string("assets/" + this->title + ".mp3");
}
