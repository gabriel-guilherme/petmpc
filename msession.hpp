#pragma once
#include "mplayer.hpp"
#include "music.hpp"
#include "utils.h"
#include <atomic>
#include <format>
#include <iostream>
#include <fstream>
#include <memory>
#include <stack>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class MSession
{
private:
  std::unique_ptr<std::unordered_set<std::shared_ptr<Music>>> m_database;
  std::unique_ptr<std::vector<std::weak_ptr<Music>>> m_queue;
  // Índice título -> música (Missão 5.3) e índice gênero -> músicas
  // (Missão 5.2). Ambos usados pra evitar buscas lineares repetidas em
  // m_database -- ver TODOs no .cpp.
  std::unordered_map<std::string, std::shared_ptr<Music>> m_index;
  std::unordered_map<std::string, std::vector<std::shared_ptr<Music>>>
      m_genre_index;
  // Histórico de reprodução (Missão 3.2): guarda os títulos das músicas
  // tocadas antes da atual, na ordem em que foram tocadas.
  std::stack<std::string> m_history;
  std::weak_ptr<Music> m_current;
  std::unique_ptr<MPlayer> m_player;
  std::thread m_play_thread;

  std::ofstream output;

  const std::string ASSETS_STR = "assets/";
  u16 m_last_id = 0;
  std::atomic<bool> m_paused{true};
  std::atomic<bool> m_stop{false};

  bool play(const std::string &);
  void log(const std::string &msg){
    output.open("log.txt", std::ios::app);
    if(!output.is_open()){
      std::cerr << "fasdfasdf\n";
      return;
    }
    output << msg << "\n";
    output.close();
  }

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
  long binary_search_library(const std::vector<std::shared_ptr<Music>> &,
                             const std::string &) const;
  void sort_queue(sort_criteria);
  void shuffle_queue();
  void save_queue();
  std::string load_queue();
  bool is_paused();
  bool toggle_paused();
  void stop_track();
  bool advance_track();
  bool rewind_track();
  bool rewind_to_history();
  std::vector<std::string> get_genres() const;
  std::vector<std::shared_ptr<Music>> get_by_genre(const std::string &) const;
  std::weak_ptr<Music> &get_current();
  const std::string get_queue_size_msg();
  const std::unordered_set<std::shared_ptr<Music>> &get_database() const;
  const std::vector<std::weak_ptr<Music>> &get_queue() const;
};
