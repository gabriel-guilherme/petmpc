#include "msession.hpp"
#include "sortingLib/sorting.hpp"
#include "string"
#include <algorithm>
#include <format>
#include <fstream>
#include <future>
#include <memory>
#include <random>
#include <sstream>

u8 MSession::init(char **argv)
{
  std::string line, path = ASSETS_STR + "database.csv";
  m_database = std::make_unique<std::unordered_set<std::shared_ptr<Music>>>();
  m_queue = std::make_unique<std::vector<std::weak_ptr<Music>>>();
  m_player = std::make_unique<MPlayer>();

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

      auto new_music = std::make_shared<Music>(++m_last_id, std::move(t),
                                               std::move(g), f, u);
      m_database->insert(std::move(new_music));
    }
  }

  return 0;
}

void MSession::end()
{
  std::cout << "Fechando aplicação..\n";

  // TODO: deletar recursos
}

void MSession::add_to_queue(u8 id)
{
  std::weak_ptr<Music> music;

  for (auto it = m_database->begin(); it != m_database->end(); it++)
    if (it->get()->id == id)
      music = *it;

  if (music.lock())
  {
    m_queue->emplace_back(music);
  }
  else
    std::cout << "Música não encontrada. \n";
}

void MSession::add_to_queue(const std::shared_ptr<Music> ptr)
{
  m_queue->emplace_back(ptr);
}

void MSession::clear_queue()
{
  m_queue->clear();
  m_current.reset();
}

void MSession::sort_queue()
{
  sa::insertion(m_queue->data(), m_queue->data() + m_queue->size(),
                [](std::weak_ptr<Music> &pt1, std::weak_ptr<Music> &pt2)
                {
                  auto m1 = pt1.lock();
                  auto m2 = pt2.lock();
                  if (m1 && m2)
                    return m1->title < m2->title ? true : false;

                  return m1 != nullptr;
                });
}

void MSession::shuffle_queue()
{
  auto rng = std::default_random_engine {};
  std::shuffle(std::begin(*m_queue) + 1, std::end(*m_queue), rng);
}

bool MSession::play(const std::string &title)
{
  paused.store(true);
  stop.store(true);
  // m_player = std::make_unique<MPlayer>();
  if (m_queue->empty())
  {
    // TODO: erro
  }

  auto duplicate = *m_queue;
  auto it = std::find_if(duplicate.begin(), duplicate.end(),
                         [&](std::weak_ptr<Music> ptr)
                         {
                           if (auto m = ptr.lock())
                           {
                             if (m->title == title)
                               return true;
                           }
                           return false;
                         });

  if (it == duplicate.end())
  {
    return false;
  }

  paused.store(false);
  stop.store(false);
  while (it != duplicate.end())
  {
    if (auto m = it->lock())
    {
      m_current = m;
      m_player->music(m->get_path().c_str());
      m_player->play(paused, stop);
      m_queue->erase(m_queue->begin());
    }

    it++;
  }

  return true;
}
