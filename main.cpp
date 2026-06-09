#include "msession.hpp"
#include <iostream>

int main(int argc, char **argv)
{
  if (argc != 1)
  {
    std::cerr << "Usage: ./petmpc\n";
    return EXIT_FAILURE;
  }

  MSession session;
  if (session.init(argv) != 0)
  {
    std::cerr << "Error during initialization..\n";
  }

  int opt = 99;
  session.display_songs();
  while (true)
  {
    session.menu();
    std::cin >> opt;
    if (opt == 0)
    {
      session.end();
      return EXIT_SUCCESS;
    }

    switch (opt)
    {
    case 0:
      break;

    case 1:
      session.display_songs();
      break;
    case 2:
      session.display_queue();
      break;
    case 3:
      std::cout << "Qual será o critério de busca? \n";
      std::cout << "";
      break;

    default:
      std::cout << "Opção inválida, tente novamente.\n";
      break;
    }
    // TODO: lógica
  }

  // auto p = std::make_unique<MPlayer>();
  // auto m = session.find_music_on_database(5)->get_path();
  // p->music(m.c_str());
  // p->play();

  return EXIT_SUCCESS;
}
