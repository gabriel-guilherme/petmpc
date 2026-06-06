#include "msession.hpp"
#include "string"
#include <format>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

u8 MSession::init(char **argv)
{
  std::string line, path = ASSETS_STR + "database.csv";
  m_database =
      std::make_unique<std::unordered_map<u16, std::shared_ptr<Music>>>();
  m_queue = std::make_unique<std::vector<std::weak_ptr<Music>>>();

  std::ifstream ifstream(path);
  if (!ifstream.is_open())
  {
    std::cerr << std::format("couldn't find the database file '{}' \n", path);
    return EXIT_FAILURE;
  }

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
      m_database->insert({last_id++, std::move(new_music)});
    }
  }

  for (auto s : *m_database)
  {
    std::cout << "song " << std::to_string(s.first) << ": "
              << s.second.get()->to_string();
  }

  return 0;
}
