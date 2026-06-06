#pragma once
#include "music.hpp"
#include "utils.h"
#include <memory>
#include <unordered_map>
#include <vector>

class MSession
{
  std::unique_ptr<std::unordered_map<u16, std::shared_ptr<Music>>> m_database;
  std::unique_ptr<std::vector<std::weak_ptr<Music>>> m_queue;
  std::weak_ptr<Music> m_current;

  const std::string ASSETS_STR = "assets/";
  u16 last_id = 0;

public:
  MSession() : m_database(nullptr), m_queue(nullptr) {};
  u8 init(char **argv);
  const Music *find_music_on_database(u16 id) const
  {
    return m_database->at(id).get();
  }
};
