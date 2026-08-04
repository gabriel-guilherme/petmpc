/*!
 * Simple hash table with separate chaining (array of buckets, each a
 * linked list). Not used by the project itself -- this is a standalone
 * exercise to build the mechanism std::unordered_map hides from you.
 * @file hashtable.hpp
 */

#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <functional>
#include <list>
#include <utility>
#include <vector>

namespace ht // ht = hash table
{

template <typename Key, typename Value> class HashTable
{
private:
  static const size_t BUCKET_COUNT = 16;
  std::vector<std::list<std::pair<Key, Value>>> m_buckets;

  size_t bucket_index(const Key &key) const
  {
    return std::hash<Key>{}(key) % BUCKET_COUNT;
  }

public:
  HashTable() : m_buckets(BUCKET_COUNT) {}

  // =============================================================================
  // [MISSÃO 4.1 - INÍCIO: Inserção com encadeamento separado (Separate Chaining)]
  // =============================================================================

  // Insere o par (key, value) na tabela. Se 'key' já existir, atualiza o
  // valor em vez de duplicar a entrada.
  void insert(const Key &key, const Value &value)
  {
    (void)key;
    (void)value; // remova essas linhas depois que implementar a função

    // DICA 0: Ache o balde certo com bucket_index(key) -- já implementado
    // acima, você só precisa chamar.

    // DICA 1: m_buckets[bucket_index(key)] é um std::list<std::pair<Key,
    // Value>>. Percorra essa lista com um for-each procurando uma entrada
    // (entry.first) igual a 'key'.

    // DICA 2: Se achar, atualize entry.second = value e retorne.

    // DICA 3: Se percorrer a lista inteira sem achar, adicione um novo par
    // no fim dela: bucket.push_back({key, value});
  }

  // =============================================================================
  // [MISSÃO 4.1 - FIM]
  // =============================================================================

  // Procura 'key' na tabela. Devolve um ponteiro pro valor correspondente
  // se encontrar, ou nullptr se não encontrar. Já implementada -- útil pra
  // testar sua insert() acima.
  Value *find(const Key &key)
  {
    auto &bucket = m_buckets[bucket_index(key)];

    for (auto &entry : bucket)
      if (entry.first == key)
        return &entry.second;

    return nullptr;
  }

  // Remove a entrada de chave 'key', se ela existir. Não faz nada se a
  // chave não estiver na tabela. Já implementada.
  void remove(const Key &key)
  {
    auto &bucket = m_buckets[bucket_index(key)];

    for (auto it = bucket.begin(); it != bucket.end(); ++it)
    {
      if (it->first == key)
      {
        bucket.erase(it);
        return;
      }
    }
  }

  size_t bucket_count() const { return BUCKET_COUNT; }
};

} // namespace ht

#endif // HASHTABLE_H
