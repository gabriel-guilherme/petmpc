#include "msession.hpp"
#include "sortingLib/sorting.hpp"
#include "string"
#include <format>
#include <fstream>
#include <future>
#include <memory>
#include <sstream>

u8 MSession::init(char **argv)
{
  clrscr();
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

  m_options.insert({0, "Sair"});
  m_options.insert({1, "Exibir biblioteca"});
  m_options.insert({2, "Exibir fila"});
  m_options.insert({3, "Buscar música na biblioteca"});
  m_options.insert({4, "Adicionar música à fila"});
  m_options.insert({5, "Ordenar fila"});
  m_options.insert({6, "Reproduzir"});
  m_options.insert({7, "Limpar fila"});
  m_options.insert({9, "Estatísticas"});

  std::cout << "[PETMPC]\n";

  return 0;
}

void MSession::display_songs()
{
  std::cout << "== BIBLIOTECA == \n";
  for (auto music : *m_database)
  {
    std::cout << music.get()->to_string();
  }
  std::cout << "== \n";
}

void MSession::menu()
{
  std::cout << "== MENU ==\n";
  std::cout << "Selecione uma das opções.\n";
  for (auto o : m_options)
  {
    std::cout << std::format("[{}] {}\n", o.first, o.second);
  }
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
  {
    if (it->get()->get_id() == id)
    {
      music = *it;
    }
  }

  if (music.lock())
    m_queue->emplace_back(music);
  else
    std::cout << "Música não encontrada. \n";
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
                    return m1->get_title() < m2->get_title() ? true : false;

                  return m1 != nullptr;
                });
}

void MSession::display_queue()
{
  if (m_queue->empty())
  {
    std::cout << "Fila de reprodução vazia.\n";
    return;
  }

  std::cout << "== FILA DE REPRODUÇÃO ==\n";
  for (auto q : *m_queue)
  {
    if (auto m = q.lock())
    {
      std::cout << m->to_string() << "\n";
    }
  }
}

void MSession::play()
{
  if (m_queue->empty())
  {
    std::cout
        << "Fila de reprodução vazia, adicione ao menos 1 música antes.\n";
    return;
  }

  std::future<bool> result =
      std::async(std::launch::async, &MSession::play_loop,
                 this); // ver alguma forma de liberar a UI pro user
}

bool MSession::play_loop()
{
  auto duplicate = *m_queue;

  for (auto it = duplicate.begin(); it != duplicate.end(); it++)
  {
    m_current = *it;
    if (auto value = m_current.lock())
    {
      m_player->music(value->get_path().c_str());
      m_player->play();
      m_queue->erase(m_queue->begin());
    }
  }

  return true;
}
