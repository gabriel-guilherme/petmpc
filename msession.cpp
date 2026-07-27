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

bool cmp_lib_by_title(const std::shared_ptr<Music> &,
                      const std::shared_ptr<Music> &);

bool cmp_lib_by_duration(const std::shared_ptr<Music> &,
                         const std::shared_ptr<Music> &);

bool cmp_queue_by_title(const std::weak_ptr<Music> &,
                        const std::weak_ptr<Music> &);

bool cmp_queue_by_duration(const std::weak_ptr<Music> &,
                           const std::weak_ptr<Music> &);

u8 MSession::init()
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

void MSession::remove_from_queue(size_t index)
{
  if (index >= m_queue->size())
    return;

  if (auto current = m_current.lock())
  {
    if (auto removed = m_queue->at(index).lock())
      if (removed == current)
        m_current.reset();
  }

  m_queue->erase(m_queue->begin() + index);
}

void MSession::sort_queue(sort_criteria crit)
{
  if (crit == sort_criteria::TITLE)
  {
    sa::insertion_sort(m_queue->data(), m_queue->data() + m_queue->size(),
                       cmp_queue_by_title);
  }
  else if (crit == sort_criteria::DURATION)
  {
    sa::insertion_sort(m_queue->data(), m_queue->data() + m_queue->size(),
                       cmp_queue_by_duration);
  }
}

std::vector<std::shared_ptr<Music>> MSession::sort_library(sort_criteria crit)
{
  std::vector<std::shared_ptr<Music>> vec(m_database->begin(),
                                          m_database->end());

  if (crit == sort_criteria::TITLE)
    sa::insertion_sort(vec.data(), vec.data() + vec.size(), cmp_lib_by_title);

  else if (crit == sort_criteria::DURATION)
    sa::insertion_sort(vec.data(), vec.data() + vec.size(),
                       cmp_lib_by_duration);

  return vec;
}

std::vector<std::shared_ptr<Music>>
MSession::search_library(const std::string &query)
{
  std::vector<std::shared_ptr<Music>> result;

  std::string needle = query;
  std::transform(needle.begin(), needle.end(), needle.begin(), ::tolower);

  // busca linear: percorre a biblioteca inteira comparando substrings
  for (auto it = m_database->begin(); it != m_database->end(); it++)
  {
    std::string haystack = (*it)->title;
    std::transform(haystack.begin(), haystack.end(), haystack.begin(),
                   ::tolower);

    if (haystack.find(needle) != std::string::npos)
      result.push_back(*it);
  }

  return result;
}

void MSession::shuffle_queue()
{
  auto rng = std::default_random_engine{};
  std::shuffle(std::begin(*m_queue), std::end(*m_queue), rng);
}

void MSession::async_play(const std::string &title)
{
  m_stop.store(true);
  if (m_play_thread.joinable())
    m_play_thread.join();

  m_stop.store(false);
  m_play_thread = std::thread([this, title] { this->play(title); });
}

bool MSession::play(const std::string &title)
{
  if (m_queue->empty())
  {
    return false;
  }

  // TODO: trocar função lambda bem como fazer com que sort de queue reflita na
  // reprodução atual.
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

  m_paused.store(false);
  m_stop.store(false);
  while (it != duplicate.end())
  {
    if (m_stop.load())
      break;
    if (auto m = it->lock())
    {
      m_current = m;
      m_player->music(m->get_path().c_str());
      m_player->play(m_paused, m_stop);
    }

    it++;
  }

  return true;
}

bool MSession::is_paused() { return m_paused.load(); }

bool MSession::toggle_paused()
{
  m_paused.store(!m_paused.load());
  return m_paused.load();
}

void MSession::stop_track() { m_stop.store(true); }

// TODO: mudar o current pra um current_idx e o acesso ser O(1)?!
bool MSession::advance_track()
{
  if (auto music = m_current.lock())
  {
    for (auto m_it = m_queue->begin(); m_it != m_queue->end(); m_it++)
    {
      if (auto it = m_it->lock())
      {
        if (it == music && m_it + 1 != m_queue->end())
        {
          async_play((m_it + 1)->lock()->title); 
          return true;
        }
      }
    }
  }

  return false;
}

bool MSession::rewind_track()
{
  if (auto music = m_current.lock())
  {
    for (auto m_it = m_queue->begin(); m_it != m_queue->end(); m_it++)
    {
      if (auto it = m_it->lock())
      {
        if (it == music && m_it != m_queue->begin())
        {
          async_play((m_it - 1)->lock()->title);
          return true;
        }
      }
    }
  }
  return false;
}

std::weak_ptr<Music> &MSession::get_current() { return m_current; }

const std::string MSession::get_queue_size_msg()
{
  return m_queue->size() == 0
             ? "Fila vazia."
             : std::format("Tamanho da fila: {}", m_queue->size());
}
const std::unordered_set<std::shared_ptr<Music>> &MSession::get_database() const
{
  return *m_database;
};

const std::vector<std::weak_ptr<Music>> &MSession::get_queue() const
{
  return *m_queue;
}

bool cmp_lib_by_title(const std::shared_ptr<Music> &m1,
                      const std::shared_ptr<Music> &m2)
{
  if (!m1 || !m2)
    return m1 != nullptr;
  return m1->title < m2->title;
}

bool cmp_lib_by_duration(const std::shared_ptr<Music> &m1,
                         const std::shared_ptr<Music> &m2)
{
  if (!m1 || !m2)
    return m1 != nullptr;
  return m1->duration < m2->duration;
}

bool cmp_queue_by_title(const std::weak_ptr<Music> &m1,
                        const std::weak_ptr<Music> &m2)
{
  auto l1 = m1.lock();
  auto l2 = m2.lock();
  if (l1 && l2)
    return l1->title < l2->title;
  return l1 != nullptr;
}

bool cmp_queue_by_duration(const std::weak_ptr<Music> &m1,
                           const std::weak_ptr<Music> &m2)
{
  auto l1 = m1.lock();
  auto l2 = m2.lock();
  if (l1 && l2)
    return l1->duration < l2->duration;
  return l1 != nullptr;
}
