#include "mplayer.hpp"
#include "music.hpp"
#include <format>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>

int main(int argc, char **argv)
{
  std::string data_path = "assets/database.csv";
  std::string line;
  auto songs =
      std::make_unique<std::unordered_map<u16, std::shared_ptr<Music>>>();

  if (argc > 1)
  {
    data_path = "assets/" + std::string(argv[1]);
  }

  std::ifstream ifstream(data_path);
  if (!ifstream.is_open())
  {
    std::cerr << std::format("couldn't find the database file '{}' \n",
                             data_path);
    return EXIT_FAILURE;
  }

  u16 song_id = 0;
  while (std::getline(ifstream, line))
  {
    if (line.empty())
      continue;

    std::stringstream ss_buf;
    ss_buf << line;
    std::string t, g, d, y;
    if (std::getline(ss_buf, t, ',') && std::getline(ss_buf, g, ',') &&
        std::getline(ss_buf, d, ',') && std::getline(ss_buf, y, ','))
    {
      float f = std::stof(d);
      u16 u = std::stoi(y);

      auto new_music =
          std::make_shared<Music>(std::move(t), std::move(g), f, u);
      songs->insert({song_id, std::move(new_music)});
      song_id++;
    }
  }

  for (auto s : *songs)
  {
    std::cout << "song " << std::to_string(s.first) << ": "
              << s.second.get()->to_string();
  }

  auto p = std::make_unique<MPlayer>();
  auto m = songs->at(0).get()->get_path();
  p->music(m.c_str());
  p->play();

  return 0;
}
