#include "mplayer.hpp"
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
    case 4:
    {
      int id;
      std::cout << "Insira o Id da música: ";
      std::cin >> id; // TODO: ver a necessidade de criar um overload pra isso
      session.add_to_queue((u8)id);
      break;
    }
    case 5:
    {
      // TODO
    }
    case 6:
    {
      session.play();
      break;
    }

    default:
      std::cout << "Opção inválida, tente novamente.\n";
      break;
    }
    // TODO: lógica
  }

  return EXIT_SUCCESS;
}
