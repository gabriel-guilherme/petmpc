#pragma once
#include "mplayer.hpp"
#include "music.hpp"
#include "utils.h"
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <unordered_map>
#include <unordered_set>

class MSession
{
  std::unique_ptr<std::unordered_set<std::shared_ptr<Music>>> m_database;
  // std::unique_ptr<std::unordered_map<u16, std::shared_ptr<Music>>>
  // m_database;
  std::unique_ptr<std::list<std::weak_ptr<Music>>> m_queue;
  std::weak_ptr<Music> m_current;
  std::unique_ptr<MPlayer> m_player;

  const std::string ASSETS_STR = "assets/";
  u16 m_last_id = 0;

  std::map<u8, std::string> m_options;

protected:
  void clrscr() { std::cout << "\033[2J\033[1;1H"; }

public:
  MSession() : m_database(nullptr), m_queue(nullptr) {};
  u8 init(char **argv);
  void end();
  void menu();
  void display_songs();
  void display_queue();
  void add_to_queue(u8);
  void clear_queue();
  void play();
};
