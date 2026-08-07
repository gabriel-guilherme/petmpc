/*!
 * Insertion and Merge sorting algorithms that work on a data range.
 * @author Luis Filipe de Sousa, Edvaldo Henrique dos Santos
 * @date June 9th, 2026.
 * @file sorting.h
 */

#ifndef SORTING_H
#define SORTING_H

#include <cstddef>
#include <sstream>
#include <utility>
using std::ostringstream;
#include <iterator>
using std::next;
using std::ostream_iterator;
#include <functional>
using std::function;
using std::less;
#include <vector>
using std::vector;
#include <array>
using std::array;
#include <algorithm>
using std::copy;
using std::for_each;
using std::max_element;
#include <cmath>
using std::pow;
#include <string>
using std::string;
using std::to_string;
#include <fstream>
#include <iostream>


  void log(const std::string &msg){
    std::ofstream output;
    output.open("log.txt", std::ios::app);
    if(!output.is_open()){
      std::cerr << "fasdfasdf\n";
      return;
    }
    output << msg << "\n";
    output.close();
  }

namespace sa
{ // sa = sorting algorithms
/// Prints out the range to a string and returns it to the client.
template <typename FwrdIt> std::string to_string(FwrdIt first, FwrdIt last)
{
  std::ostringstream oss;
  oss << "[ ";
  while (first != last)
  {
    oss << *first++ << " ";
  }
  oss << "]";
  return oss.str();
}

//{{{ INSERTION SORT
/// Implementation of the Insertion Sort algorithm.
template <typename DataType, typename Compare>
void insertion_sort(DataType *first, DataType *last, Compare cmp)
{
  if (first == last)
    return;

  DataType *i = first + 1;
  while (i < last)
  {
    DataType *j = i;

    while (j > first && cmp(*j, *(j - 1)))
    {
      std::swap(*j, *(j - 1));
      --j;
    }

    ++i;
  }
}
//}}} INSERTION SORT

//{{{ MERGE SORT

template <typename DataType, typename Compare>
void merge_array(DataType *first, DataType *mid, DataType *last, Compare cmp)
{
  size_t leftSize = std::distance(first, mid);
  size_t rightSize = std::distance(mid, last);

  DataType *left = new DataType[leftSize];
  DataType *right = new DataType[rightSize];
  std::copy(first, mid, left);
  std::copy(mid, last, right);

  auto lStart = left;
  auto lEnd = left + leftSize;
  auto rStart = right;
  auto rEnd = right + rightSize;

  while (first != last)
  {
    if (lStart == lEnd)
    {
      std::copy(rStart, rEnd, first);
      break;
    }
    if (rStart == rEnd)
    {
      std::copy(lStart, lEnd, first);
      break;
    }
    if (cmp(*lStart, *rStart))
    {
      *first = *lStart;
      ++lStart;
    }
    else
    {
      *first = *rStart;
      ++rStart;
    }
    ++first;
  }

  delete[] left;
  delete[] right;
}

template <typename DataType, typename Compare>
void merge(DataType *first, DataType *last, Compare cmp)
{
  if (last - first <= 1)
  {
    return;
  }

  DataType *mid = first + (last - first) / 2;

  merge(first, mid, cmp);
  merge(mid, last, cmp);

  merge_array(first, mid, last, cmp);
}
//}}} MERGE SORT

//{{{ SELECTION SORT

// =============================================================================
// [MISSÃO 2.3 - INÍCIO: Ordenação por Seleção (Selection Sort)]
// =============================================================================

/// Implementation of the Selection Sort algorithm.
template <typename DataType, typename Compare>
void selection_sort(DataType *first, DataType *last, Compare cmp)
{
  if (first >= last)
    return;

  for (DataType *i = first; i < last - 1; ++i)
  {
    DataType *min_it = i;

    for (DataType *j = i + 1; j < last; ++j)
    {
      if (cmp(*j, *min_it))
      {
        min_it = j;
      }
    }
    if (min_it != i)
    {
      std::swap(*i, *min_it);
    }
  }
}

// =============================================================================
// [MISSÃO 2.3 - FIM]
// =============================================================================

//}}} SELECTION SORT

//{{{ QUICK SORT

// =============================================================================
// [MISSÃO 5.1 - INÍCIO: Ordenação Rápida (Quick Sort) -- revisão do Dia 2]
// =============================================================================

template <typename DataType, typename Compare>
void quick_sort(DataType *first, DataType *last, Compare cmp)
{
  (void)first;
  (void)last;
  (void)cmp; // remova essas linhas depois que implementar a função

  // DICA 0: Caso base -- se 'last - first <= 1', o intervalo tem 0 ou 1
  // elemento e já está ordenado; apenas retorne.

  // DICA 1: Escolha um pivô. O elemento em '*(last - 1)' é a escolha mais
  // simples.

  // DICA 2: Particione o intervalo: percorra de 'first' até o pivô (sem
  // incluí-lo) com um ponteiro 'j'; mantenha outro ponteiro 'i' marcando o
  // fim da região "menor que o pivô". Toda vez que cmp(*j, *pivot) for
  // verdadeiro, troque *i e *j e avance 'i'.

  // DICA 3: Depois de percorrer tudo, troque o pivô pra posição 'i' -- ele
  // já está no lugar certo, dividindo o intervalo em duas partições.

  // DICA 4: Chame quick_sort recursivamente nas duas partições resultantes,
  // sem incluir o pivô em nenhuma delas.

    if (last - first <= 1) {
        return;
    }

    DataType *pivot = last - 1;

    DataType *i = first;
    for (DataType *j = first; j < pivot; ++j) {
        if (cmp(*j, *pivot)) {
            DataType temp = *i;
            *i = *j;
            *j = temp;
            ++i;
        }
    }

    DataType temp = *i;
    *i = *pivot;
    *pivot = temp;
    DataType *pivot_pos = i;

    quick_sort(first, pivot_pos, cmp);
    quick_sort(pivot_pos + 1, last, cmp);
}

// =============================================================================
// [MISSÃO 5.1 - FIM]
// =============================================================================

//}}} QUICK SORT

}; // namespace sa
#endif // SORTING_H
