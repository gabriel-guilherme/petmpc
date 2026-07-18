#pragma once
#include "mplayer.hpp"
#include "music.hpp"
#include "utils.h"
#include <atomic>
#include <format>
#include <iostream>
#include <memory>
#include <unordered_set>
#include <vector>

class MSession
{
  std::unique_ptr<std::unordered_set<std::shared_ptr<Music>>> m_database;
  std::unique_ptr<std::vector<std::weak_ptr<Music>>> m_queue;
  std::weak_ptr<Music> m_current;
  std::unique_ptr<MPlayer> m_player;

  const std::string ASSETS_STR = "assets/";
  u16 m_last_id = 0;
  std::atomic<bool> paused{true};
  std::atomic<bool> stop{false};

public:
  MSession() : m_database(nullptr), m_queue(nullptr) {};
  u8 init(char **argv);
  void end();
  void add_to_queue(u8);
  void add_to_queue(const std::shared_ptr<Music>);
  void clear_queue();
  bool play(const std::string &);
  void sort_queue();
  void shuffle_queue();
  bool is_paused() { return paused.load(); }
  bool toggle_paused()
  {
    paused.store(!paused.load());
    return paused.load();
  }
  void stop_track() { stop.store(true); }
  std::weak_ptr<Music> &get_current() { return m_current; }

  const std::string get_queue_size_msg()
  {
    return m_queue->size() == 0
               ? "Fila vazia."
               : std::format("Tamanho da fila: {}", m_queue->size());
  }
  const std::unordered_set<std::shared_ptr<Music>> &get_database() const
  {
    return *m_database;
  };
  const std::vector<std::weak_ptr<Music>> &get_queue() const
  {
    return *m_queue;
  }
};
