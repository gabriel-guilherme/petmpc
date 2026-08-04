# PETMusicPlayer (SPETIFY)

Um player de música para terminal, escrito em C++20, com interface interativa construída com [FTXUI](https://github.com/ArthurSonzogni/ftxui). O projeto foi desenvolvido como parte de um minicurso de Estruturas de Dados organizado pelo Programa de Educação Tutorial do Bacharelado em Ciência da Computação da UFRN, e serve como estudo de caso: cada funcionalidade do player é a aplicação prática de uma estrutura de dados ou algoritmo clássico — vetores dinâmicos, pilhas, tabelas hash, busca linear/binária e algoritmos de ordenação — dentro de um programa real e interativo, e não apenas em exercícios isolados.

Este material foi preparado para ser apresentado aos calouros participantes do minicurso, junto com o **NotePET** (editor de texto de terminal com o mesmo propósito pedagógico): os(as) calouros(as) escolhem um dos dois projetos para acompanhar as missões práticas.

## Funcionalidades

Cada funcionalidade abaixo corresponde a uma "Missão" do minicurso e à estrutura de dados/algoritmo que ela exercita:

| Missão | Funcionalidade | Estrutura/Algoritmo | Onde encontrar |
|---|---|---|---|
| 2.1 | Busca por título na biblioteca (`/`) | Busca linear por substring | `msession.cpp`, função `search_library` |
| 2.2 | Busca exata por título em lista ordenada | Busca binária | `msession.cpp`, função `binary_search_library` |
| 2.3 | Ordenação da biblioteca/fila (modal "Ordenar", `s`) | Selection sort | `sortingLib/sorting.hpp`, função `selection_sort` |
| 3.1 | Remover música específica da fila (`d`) | Remoção por posição em `std::vector` (TAD Fila) | `msession.cpp`, função `remove_from_queue` |
| 3.2 | Voltar para a música anterior (`v`) | Pilha (`std::stack<string>`) | `msession.cpp`, função `rewind_to_history` |
| 4.1 | Tabela hash própria (estrutura standalone, didática) | Hash table com encadeamento separado | `hashLib/hashtable.hpp`, método `HashTable::insert` |
| 5.1 | Ordenação alternativa (revisão do Dia 2) | Quick sort | `sortingLib/sorting.hpp`, função `quick_sort` |
| 5.2 | Agrupamento da biblioteca por gênero | Tabela hash (`unordered_map<string, vector<Music>>`) | `msession.cpp`, dentro de `init()` (bloco `m_genre_index`) |
| 5.3 | Carregamento rápido da fila salva (`l`) | Tabela hash (`unordered_map<string, Music>`) | `msession.cpp`, dentro de `init()` (bloco `m_index`) e em `load_queue()` |

Funcionalidades adicionais de infraestrutura do projeto (já implementadas, servem de base para as missões acima):

- **Biblioteca de músicas** carregada de um `unordered_set<shared_ptr<Music>>` a partir do `database.csv`.
- **Insertion Sort** e **Merge Sort**, usados internamente como algoritmos de ordenação de referência (o aluno os compara com o Selection Sort e o Quick Sort implementados nas missões).
- **Reprodução real de `.mp3`** via `libmpg123` + `libao`, rodando em uma thread separada com controle de play/pause/stop via `std::atomic`.
- **Salvar / Carregar fila** em um arquivo de texto (`assets/queue.txt`).
- **Ponteiros inteligentes** (`shared_ptr`/`weak_ptr`) para gerenciar o ciclo de vida das músicas entre biblioteca, fila e faixa atual sem cópias nem *dangling pointers*.

---

## Dependências

Antes de compilar, você precisa ter instalado no sistema:

- Um compilador com suporte a **C++20** (g++ 10+ ou clang 12+).
- **CMake** (versão recente — o projeto declara `cmake_minimum_required(VERSION 4.2)`, então use a versão mais atual disponível para o seu sistema).
- **libao** (biblioteca de saída de áudio).
- **libmpg123** (decodificador de MP3).

As bibliotecas **argparse** e **ftxui** são baixadas e compiladas automaticamente pelo CMake através de `FetchContent`, então você não precisa instalá-las manualmente — apenas garanta que você tem conexão com a internet na primeira vez que compilar o projeto.

### Instalando as dependências (Debian/Ubuntu)

```bash
sudo apt update
sudo apt install build-essential cmake libao-dev libmpg123-dev
```

### Instalando as dependências (Arch Linux)

```bash
sudo pacman -S base-devel cmake libao mpg123
```

### macOS (via Homebrew)

```bash
brew install cmake libao mpg123
```

## Configurando os assets (obrigatório antes de rodar)

O player **não vem com músicas incluídas**. Para funcionar, você precisa montar sua própria biblioteca dentro da pasta `assets/`:

1. **Arquivos `.mp3`**: coloque dentro de `assets/` cada música em formato MP3. O nome do arquivo deve ser exatamente igual ao título que você vai cadastrar no `database.csv` (veja abaixo), seguido da extensão `.mp3`. Por exemplo, uma música com título `Crush` deve existir como:

   ```
   assets/Crush.mp3
   ```

2. **Arquivo `database.csv`**: também dentro de `assets/`, crie (ou edite) um arquivo chamado `database.csv` listando cada música, uma por linha, no seguinte formato:

   ```
   Título, Gênero, Duração, Ano
   ```

   - **Título**: o nome da música (e do arquivo `.mp3` correspondente, sem a extensão).
   - **Gênero**: o gênero musical (texto livre).
   - **Duração**: no formato `M.SS` (minutos ponto segundos), e não em minutos decimais. Por exemplo, uma música de 5 minutos e 58 segundos deve ser escrita como `5.58`, e uma música de apenas 40 segundos deve ser escrita como `0.40`.
   - **Ano**: o ano de lançamento (número inteiro).

   Exemplo de `database.csv`:

   ```
   Blood Eagle, Progressive Metalcore, 5.58, 2019
   CHVRCH BVRNER, Progressive Metalcore, 3.40, 2019
   Crush, Progressive Metalcore, 6.49, 2019
   ```

   Nesse exemplo, o player espera encontrar os arquivos `assets/Blood Eagle.mp3`, `assets/CHVRCH BVRNER.mp3` e `assets/Crush.mp3`.

> **Atenção:** a comparação entre CSV e nomes de arquivo é sensível a maiúsculas/minúsculas e a espaços — mantenha o título no CSV idêntico ao nome do arquivo (sem a extensão `.mp3`).

## Compilando o projeto (CMake)

A partir da raiz do projeto:

```bash
# 1. Cria a pasta de build (fora do código-fonte)
mkdir -p build
cd build

# 2. Gera os arquivos de build com o CMake
cmake ..

# 3. Compila o projeto
cmake --build .
```

Isso vai:

- Baixar e compilar `argparse` e `ftxui` automaticamente (só na primeira vez, ou quando o cache do CMake for limpo).
- Compilar todos os arquivos `.cpp` do projeto (raiz + `ui/`).
- Copiar a pasta `assets/` (com o seu `database.csv` e seus `.mp3`) para dentro da pasta de build, ao lado do executável.

O binário final se chama `petmpc` e fica dentro da pasta `build/`.

### Recompilando após alterar código

Sempre que você alterar algum `.cpp`/`.hpp`, basta rodar de dentro da pasta `build/`:

```bash
cmake --build .
```

Não é necessário rodar `cmake ..` novamente, a menos que você tenha adicionado/removido arquivos-fonte ou alterado o `CMakeLists.txt`.

## Executando

De dentro da pasta `build/` (para que o executável encontre a pasta `assets/` copiada ao lado dele):

```bash
./petmpc
```

O programa não aceita argumentos de linha de comando; qualquer argumento extra fará com que ele mostre a mensagem de uso e encerre.

## Controles

| Tecla(s)          | Ação                                              |
|-------------------|----------------------------------------------------|
| `↑` / `↓`         | Navegar entre os itens da lista (biblioteca ou fila) |
| `←` / `→`         | Alternar entre as abas "Biblioteca" e "Fila de Reprodução" |
| `Enter` (na Biblioteca) | Adicionar a música selecionada à fila         |
| `Enter` (na Fila) | Tocar a música selecionada                        |
| `p`               | Tocar / Pausar                                    |
| `q`               | Sair do programa                                  |
| `s`               | Abrir modal de ordenação (por título ou duração)  |
| `r`               | Embaralhar a fila                                 |
| `/`               | Abrir modal de busca por título na biblioteca     |
| `c`               | Limpar a fila de reprodução                       |
| `d`               | Remover da fila a música atualmente selecionada   |
| `v`               | Voltar para a música anterior (via histórico em pilha) |
| `l`               | Carregar fila de reprodução salva                 |
| `e`               | Salvar fila de reprodução atual para o arquivo    |
| `Ctrl` + `→`      | Avançar para a próxima faixa da fila              |
| `Ctrl` + `←`      | Retroceder para a faixa anterior da fila (por posição na fila) |

> **Nota:** `Ctrl+←` e `v` fazem coisas parecidas mas não idênticas — `Ctrl+←` retrocede pela posição atual na fila, enquanto `v` usa o histórico de reprodução (pilha), então pode levar a uma faixa que nem está mais na posição anterior da fila se ela foi reordenada ou embaralhada nesse meio-tempo.

## Estrutura do projeto

```
petmpc/
├── CMakeLists.txt       # configuração de build (CMake + FetchContent)
├── main.cpp             # ponto de entrada
├── mplayer.{hpp,cpp}    # camada de reprodução de áudio (libao + libmpg123)
├── msession.{hpp,cpp}   # sessão do player: biblioteca, fila, buscas,
│                        # ordenação, histórico (pilha) e índices (hash)
├── music.hpp            # struct Music (representa uma faixa)
├── utils.h              # tipos utilitários e formatação de duração
├── sortingLib/
│   └── sorting.hpp      # algoritmos de ordenação (Insertion, Merge,
│                        # Selection e Quick Sort)
├── hashLib/
│   └── hashtable.hpp    # tabela hash própria com encadeamento separado
│                        # (estrutura didática standalone, Missão 4.1)
├── ui/
│   ├── window.{hpp,cpp} # interface de terminal (FTXUI)
└── assets/
    └── database.csv     # biblioteca de músicas (você monta a sua!)
```

**Resumo dos módulos:**

- **`msession`**: o "cérebro" do player — guarda a biblioteca (`unordered_set`), a fila (`vector<weak_ptr<Music>>`), o histórico de reprodução (`stack<string>`) e os índices de título/gênero (`unordered_map`), além de orquestrar buscas, ordenações e a thread de reprodução.
- **`sortingLib`**: os algoritmos de ordenação usados por `msession` (Insertion e Merge Sort, prontos, usados como referência) e implementados como missão (Selection Sort no Dia 2, Quick Sort no Dia 5).
- **`hashLib`**: uma tabela hash própria, isolada do resto do projeto — existe só para o aluno enxergar o mecanismo (baldes + encadeamento) que `std::unordered_map`/`unordered_set` escondem.
- **`mplayer`**: camada mais baixa — decodifica e toca o `.mp3` via `libmpg123`/`libao`.
- **`ui/window`**: interface de terminal construída com FTXUI — abas, modais e o loop de eventos de teclado que conecta as teclas às operações de `msession`.

---

## Sobre o minicurso

O PETMusicPlayer foi criado como um dos dois projetos-guia do minicurso de **Estruturas de Dados** do **PET Computação (UFRN)** — a outra opção é o **NotePET**, um editor de texto de terminal com o mesmo propósito pedagógico e o mesmo nível de dificuldade por missão. A ideia é que, ao explorar o código, os(as) calouros(as) enxerguem estruturas de dados clássicas (vetores dinâmicos, pilhas, tabelas hash) e algoritmos (busca linear, busca binária, insertion/selection/quick sort) resolvendo problemas concretos de um programa real — em vez de apenas exercícios teóricos isolados.
