#pragma once
#include "music.hpp"
#include "utils.h"
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <unordered_map>

class MSession
{
  std::unique_ptr<std::unordered_map<u16, std::shared_ptr<Music>>> m_database;
  std::unique_ptr<std::list<std::weak_ptr<Music>>> m_queue;
  std::weak_ptr<Music> m_current;

  const std::string ASSETS_STR = "assets/";
  u16 m_last_id = 0;

  std::map<u8, std::string> m_options;

protected:
  void clrscr() { std::cout << "\033[2J\033[1;1H"; }

public:
  MSession() : m_database(nullptr), m_queue(nullptr) {};
  u8 init(char **argv);
  const Music *find_music_on_database(u16 id) const
  {
    return m_database->at(id).get();
  }
  void end();
  void menu();
  void display_songs();
  void display_queue();
};
