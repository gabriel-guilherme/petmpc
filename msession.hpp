#pragma once
#include "mplayer.hpp"
#include "music.hpp"
#include "utils.h"
#include <atomic>
#include <format>
#include <iostream>
#include <memory>
#include <thread>
#include <unordered_set>
#include <vector>

class MSession
{
private:
  std::unique_ptr<std::unordered_set<std::shared_ptr<Music>>> m_database;
  std::unique_ptr<std::vector<std::weak_ptr<Music>>> m_queue;
  std::weak_ptr<Music> m_current;
  std::unique_ptr<MPlayer> m_player;
  std::thread m_play_thread;

  const std::string ASSETS_STR = "assets/";
  u16 m_last_id = 0;
  std::atomic<bool> m_paused{true};
  std::atomic<bool> m_stop{false};

  bool play(const std::string &);

public:
  MSession() : m_database(nullptr), m_queue(nullptr) {};
  virtual ~MSession() = default;
  u8 init();
  void add_to_queue(u8);
  void add_to_queue(const std::shared_ptr<Music>);
  void clear_queue();
  void remove_from_queue(size_t);
  void async_play(const std::string &);
  std::vector<std::shared_ptr<Music>> sort_library(sort_criteria);
  std::vector<std::shared_ptr<Music>> search_library(const std::string &);
  void sort_queue(sort_criteria);
  void shuffle_queue();
  void save_queue();
  std::string load_queue();
  bool is_paused();
  bool toggle_paused();
  void stop_track();
  bool advance_track();
  bool rewind_track();
  std::weak_ptr<Music> &get_current();
  const std::string get_queue_size_msg();
  const std::unordered_set<std::shared_ptr<Music>> &get_database() const;
  const std::vector<std::weak_ptr<Music>> &get_queue() const;
};
