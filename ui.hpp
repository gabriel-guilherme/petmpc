#ifndef UI_HPP
#define UI_HPP

#include "msession.hpp"
#include "music.hpp"
#include <algorithm>
#include <ftxui/component/app.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <future>
#include <memory>
#include <unordered_set>

namespace ui
{
using namespace ftxui;

void init(char **filename)
{
  std::cout << "\033[2J\033[1;1H";

  auto screen = App::TerminalOutput();
  auto session = MSession();
  session.init(filename);

  std::string result;

  std::vector<std::string> tabs{"Biblioteca", "Fila de Reprodução"};
  int selected_tab = 0;
  auto tab_menu = Menu(&tabs, &selected_tab);

  int lib_entry = 0;
  int queue_entry = 0;
  std::vector<std::string> lib_titles;
  std::vector<std::string> queue_titles;
  std::for_each(session.get_database().begin(), session.get_database().end(),
                [&lib_titles](std::shared_ptr<Music> ptr)
                { lib_titles.push_back(ptr->title); });

  auto lib = Menu(&lib_titles, &lib_entry);
  lib |= CatchEvent(
      [&](Event event)
      {
        if (event == Event::Return)
        {
          const auto library = session.get_database();
          auto title = lib_titles.at(lib_entry);
          auto selected_it = std::find_if(library.begin(), library.end(),
                                          [&title](std::shared_ptr<Music> ptr)
                                          { return ptr->title == title; });

          auto selected = *selected_it;
          session.add_to_queue(selected);
          result = std::format("'{}' adicionada à fila\n", selected->title);
          return true;
        }
        else if (event == Event::Character('s'))
        {
          // TODO: chamar algoritmo de sort
          return false;
        }
        else if (event == Event::Character('p'))
        {
          // TODO: chamar algoritmo de find
          return false;
        }
        return false;
      });

  auto queue_container = Container::Tab({}, &queue_entry);

  auto queue_renderer = Renderer(
      queue_container,
      [&]
      {
        queue_titles.clear();
        for (auto const &weak_ptr : session.get_queue())
        {
          if (auto m = weak_ptr.lock())
          {
            queue_titles.push_back(m->title);
          }
        }
        queue_container->DetachAllChildren();
        if (!queue_titles.empty())
        {
          auto queue_menu = Menu(&queue_titles, &queue_entry);

          queue_menu |= CatchEvent(
              [&](Event event)
              {
                if (event == Event::Return && !queue_titles.empty())
                {
                  if (!session.is_paused())
                  {
                    result =
                        "Pause o player antes de escolher uma nova música.";
                    return true;
                  }

                  if (queue_entry < session.get_queue().size())
                  {
                    auto music = session.get_queue().at(queue_entry);
                    if (auto m = music.lock())
                    {
                      std::string title = m->title;
                      session.stop_track();
                      result = std::format(
                          "Reproduzindo a fila\n"); // se queremos ser
                                                    // simplistas não podemos
                                                    // usar mutex e nada mais de
                                                    // s.o ...
                                                    // tirando isso aqui pra
                                                    // conseguir rodar em
                                                    // paralelo com a UI
                      std::thread play_thread([&session, title]
                                              { session.play(title); });
                      play_thread.detach();
                    }
                  }
                  return true;
                }
                return false;
              });

          queue_container->Add(queue_menu);
        }

        return vbox({text(session.get_queue_size_msg()), separator(),
                     queue_container->Render()});
      });

  auto tab_container = Container::Tab({lib, queue_renderer}, &selected_tab);

  auto container = Container::Horizontal({
      tab_menu,
      tab_container,
  });

  auto renderer = Renderer(container,
                           [&]
                           {
                             return vbox({text("PETMPC") | bold | center,
                                          tab_menu->Render(), separator(),
                                          tab_container->Render(),
                                          text(result) | borderDashed}) |
                                    border;
                           });

  renderer |= CatchEvent(
      [&](Event event)
      {
        if (event == Event::q)
        {
          screen.Exit();
          return true;
        }
        else if (event == Event::p)
        {
          if (session.get_queue().empty())
            result = "Não há nada tocando.";
          else
          {
            session.toggle_paused();
            if (session.is_paused())
            {
              result = "Reprodução pausada.";
            }
            else
            {
              auto current = session.get_current().lock();
              if (current)
                result = std::format("Voltando a tocar {}", current->title);
            }
          }
        }
        else if (event == Event::r)
        {
          if (session.get_queue().empty())
          {
            result = "Adicione músicas à fila para embaralhar.";
          }
          else
          {
            session.shuffle_queue();
            result = "Fila embaralhada.";
          }
        }
        return false;
      });

  screen.Loop(renderer);
}

} // namespace ui

#endif
