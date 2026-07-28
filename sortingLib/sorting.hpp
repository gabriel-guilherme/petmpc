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

//{{{ QUICK SORT

/// [DIA 2 - Ordenação] TODO:
/// Implemente o Quick Sort no intervalo [first, last).
///
/// Dicas:
/// 1. Caso base: se `last - first <= 1`, o intervalo já está ordenado
///    (0 ou 1 elemento), então apenas retorne.
/// 2. Escolha um pivô (o último elemento, `*(last - 1)`, é a escolha mais
///    simples).
/// 3. Particione o intervalo: mova pra frente os elementos que, comparados
///    com o pivô usando `cmp`, deveriam vir antes dele; o pivô fica no
///    "meio", entre a partição da esquerda e a da direita.
/// 4. Chame quick_sort recursivamente nas duas partições, sem incluir o
///    pivô nelas.
///
/// Pode usar std::swap (já usado no insertion_sort acima) pra trocar
/// elementos de posição durante o particionamento.
template <typename DataType, typename Compare>
void quick_sort(DataType *first, DataType *last, Compare cmp)
{
}

//}}} QUICK SORT

}; // namespace sa
#endif // SORTING_H
