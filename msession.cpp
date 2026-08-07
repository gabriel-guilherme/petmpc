#include "msession.hpp"
#include "sortingLib/sorting.hpp"
#include <string>
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

  // =============================================================================
  // [MISSÃO 5.3 - INÍCIO: Índice título -> música (revisão do Dia 4)]
  // =============================================================================

  // DICA 0: m_database é um unordered_set: ótimo pra checar existência e
  // iterar, mas não permite achar uma música pelo título sem varrer tudo.
  // load_queue() (mais abaixo) faz exatamente isso hoje -- um find_if pra
  // CADA linha do arquivo salvo, ou seja, O(n) por busca.

  // DICA 1: Percorra m_database (já populado acima) com um laço for-each.

  // DICA 2: Preencha m_index com uma entrada por música: a chave é o
  // título (music->title) e o valor é o próprio std::shared_ptr<Music>.

  // =============================================================================
  // [MISSÃO 5.3 - FIM]
  // =============================================================================

  // =============================================================================
  // [MISSÃO 5.2 - INÍCIO: Índice gênero -> músicas (revisão do Dia 4)]
  // =============================================================================

  // DICA 0: Diferente de m_index (uma música por chave), aqui cada chave
  // (gênero) guarda uma LISTA de músicas daquele gênero.

  // DICA 1: Percorra m_database com um laço for-each.

  // DICA 2: unordered_map::operator[] cria o vector vazio automaticamente
  // na primeira vez que uma chave nova (um gênero novo) é acessada -- você
  // só precisa dar push_back nele: m_genre_index[music->genre].push_back(music);

  // =============================================================================
  // [MISSÃO 5.2 - FIM]
  // =============================================================================

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

// =============================================================================
// [MISSÃO 3.1 - INÍCIO: Remoção de uma música específica da fila]
// =============================================================================

void MSession::remove_from_queue(size_t index)
{
  (void)index; // remova essa linha depois que implementar a função

  // DICA 0: Se 'index' estiver fora dos limites de m_queue (index >=
  // m_queue->size()), apenas retorne -- não faça nada.

  // DICA 1: Se a música removida for a que está tocando no momento
  // (m_current), resete m_current -- senão o player fica com uma
  // referência "fantasma" pra uma música que não está mais na fila. Use
  // m_current.lock() e m_queue->at(index).lock() pra comparar os dois
  // shared_ptr resultantes.

  // DICA 2: std::vector::erase remove um elemento numa posição específica:
  // m_queue->erase(m_queue->begin() + index);

  if(index >= m_queue->size()){
    return;
  }

  m_queue->erase(m_queue->begin() + index);

  if(m_queue->at(index).lock() == m_current.lock()){
    m_current = m_queue->at(0);
  }

  return;

}

// =============================================================================
// [MISSÃO 3.1 - FIM]
// =============================================================================

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

// =============================================================================
// [MISSÃO 2.1 - INÍCIO: Busca Linear por substring no título]
// =============================================================================

std::string debug(std::string d){
  return d;
}

std::vector<std::shared_ptr<Music>>
MSession::search_library(const std::string &query)
{

  std::vector<std::shared_ptr<Music>> result;

  std::string lower_query = query;
  std::transform(lower_query.begin(), lower_query.end(), lower_query.begin(), ::tolower);

  for (auto it = m_database->begin(); it != m_database->end(); ++it) {
    
    std::string title = (*it)->title;

    std::string lower_title = title;
    std::transform(lower_title.begin(), lower_title.end(), lower_title.begin(), ::tolower);

    if (lower_title.find(lower_query) != std::string::npos) {
      result.push_back(*it);
    }
  }

  return result;
}

// =============================================================================
// [MISSÃO 2.1 - FIM]
// =============================================================================

// =============================================================================
// [MISSÃO 2.2 - INÍCIO: Busca Binária por título exato]
// =============================================================================

long MSession::binary_search_library(
    const std::vector<std::shared_ptr<Music>> &sorted,
    const std::string &title) const
{
    long low = 0;
    long high = static_cast<long>(sorted.size()) - 1;

    while (low <= high) {
        long mid = low + (high - low) / 2;
        
        const std::string& mid_title = sorted[static_cast<size_t>(mid)]->title;

        if (mid_title == title) {
            return mid;
        } 
        else if (mid_title < title) {
            low = mid + 1;
        } 
        else {
            high = mid - 1;
        }
    }

    return -1;
}

// =============================================================================
// [MISSÃO 2.2 - FIM]
// =============================================================================

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

void MSession::save_queue()
{
  std::ofstream queue_file("assets/queue.txt");
  auto first = get_queue().begin();
  auto last = get_queue().end();

  while (first != last)
  {
    queue_file << first->lock()->title << '\n';
    first++;
  }

  queue_file.close();
}

std::string MSession::load_queue()
{
  std::ifstream queue_file("assets/queue.txt");
  if (!queue_file.is_open()) {return "Não foi possível ler a fila salva.";}
  clear_queue();
  
  std::string s;

  while(std::getline(queue_file, s))
  {
    // Missão 5.3: depois que m_index estiver preenchido (ver init()),
    // troque a busca abaixo por uma consulta direta em m_index -- em vez
    // de varrer m_database inteiro pra cada linha do arquivo, procure 's'
    // com m_index.find(s) e confira contra m_index.end(). Se a música
    // salva não existir mais na biblioteca, pule a linha em vez de travar.
    auto music_it = std::find_if(get_database().begin(),
                                          get_database().end(),
                                          [&s](std::shared_ptr<Music> ptr)
                                          { return ptr->title == s; });
    add_to_queue(*music_it);
  }
  queue_file.close();
  return "Fila carregada do arquivo.";
}

bool MSession::play(const std::string &title)
{
  if (m_queue->empty())
  {
    return false;
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
          // Infra da Missão 3.2: registra a música atual no histórico
          // antes de trocar pra próxima, pra que rewind_to_history() (ver
          // abaixo) tenha o que desempilhar depois.
          m_history.push(music->title);
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

// =============================================================================
// [MISSÃO 3.2 - INÍCIO: Voltar pra música anterior usando o histórico (Pilha)]
// =============================================================================

bool MSession::rewind_to_history()
{
  // DICA 0: Diferente de rewind_track() (que usa a posição na fila),
  // aqui você usa m_history -- a pilha (std::stack<std::string>)
  // preenchida em advance_track() com os títulos das músicas já tocadas.

  // DICA 1: Primeiro, trate o caso de a pilha estar vazia (m_history.empty()):
  // não há pra onde voltar, então retorne false.

  // DICA 2: Se não estiver vazia, o título do topo é m_history.top().
  // Guarde-o numa variável antes de desempilhar com m_history.pop() --
  // depois do pop() o valor de top() não existe mais.

  // DICA 3: Chame async_play com o título guardado, e retorne true.

  if(m_history.empty()){
    return false;
  }

  async_play(m_history.top());
  m_history.pop();

  return false;
}

// =============================================================================
// [MISSÃO 3.2 - FIM]
// =============================================================================

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

// Missão 5.2: depois que m_genre_index estiver preenchido (ver init()),
// estas duas funções te dão acesso a ele pronto pra uso pela UI (uma
// lista de gêneros disponíveis, e as músicas de um gênero específico).
// Não precisam de laços de busca -- é só consultar o mapa.
std::vector<std::string> MSession::get_genres() const
{
  std::vector<std::string> genres;
  for (const auto &[genre, musics] : m_genre_index)
    genres.push_back(genre);
  return genres;
}

std::vector<std::shared_ptr<Music>>
MSession::get_by_genre(const std::string &genre) const
{
  auto it = m_genre_index.find(genre);
  if (it == m_genre_index.end())
    return {};
  return it->second;
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
