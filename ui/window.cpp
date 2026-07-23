#include "window.hpp"

#include <algorithm>
#include <bits/stdc++.h>
#include <format>
#include <iostream>

namespace ui
{
using namespace ftxui;

Window::Window() : m_screen(ScreenInteractive::TerminalOutput()) {}

bool Window::init()
{
  std::cout << "\033[2J\033[1;1H";

  if (m_session.init() != 0)
    return false;

  std::vector<std::shared_ptr<Music>> initial_lib(
      m_session.get_database().begin(), m_session.get_database().end());
  sync_lib(initial_lib);
  auto modal = build_sort_modal();
  m_main_layout = construct_layout();
  m_modal_component = Modal(m_main_layout, modal, &m_sort_modal_open);

  return true;
}

void Window::sync_lib(const std::vector<std::shared_ptr<Music>> &vec)
{
  m_lib_titles.clear();

  for (const auto &music : vec)
  {
    m_lib_titles.push_back(music->title);
  }
}

void Window::sync_queue()
{
  m_queue_titles.clear();
  for (auto const &weak_ptr : m_session.get_queue())
  {
    if (auto m = weak_ptr.lock())
      m_queue_titles.push_back(m->title);
  }
}

ftxui::Component Window::build_library_component()
{
  MenuOption menu_opt;
  menu_opt.entries_option.transform = [this](EntryState state)
  {
    if (state.index < m_lib_titles.size())
    {
      const auto &t = m_lib_titles.at(state.index);
      // TODO: trocar quando houver o método de buscar diretamente na session
      // pois é bem ineficiente ficar fazendo isso aqui.
      auto it = std::find_if(
          m_session.get_database().begin(), m_session.get_database().end(),
          [&t](std::shared_ptr<Music> ptr) { return ptr->title == t; });

      auto item = *it;
      return render_row(*item, state.focused);
    }
    return text(state.label);
  };

  m_lib_menu = Menu(&m_lib_titles, &m_lib_entry, menu_opt);
  m_lib_menu |= CatchEvent(
      [this](Event event)
      {
        if (event == Event::Return && !m_lib_titles.empty())
        {
          std::string title = m_lib_titles.at(m_lib_entry);

          auto selected_it = std::find_if(m_session.get_database().begin(),
                                          m_session.get_database().end(),
                                          [&title](std::shared_ptr<Music> ptr)
                                          { return ptr->title == title; });

          auto selected = *selected_it;
          m_session.add_to_queue(selected);
          m_status_msg =
              std::format("'{}' adicionada à fila\n", selected->title);
          return true;
        }
        return false;
      });

  return Renderer(m_lib_menu,
                  [this]
                  {
                    return vbox({
                        hbox({
                            text("Título") | bold | flex,
                            text("Duração") | bold | size(WIDTH, EQUAL, 12),
                            text("Ano") | bold | size(WIDTH, EQUAL, 8),
                        }),
                        separator(),
                        m_lib_menu->Render() | vscroll_indicator | frame,
                    });
                  });
}

ftxui::Component Window::build_queue_component()
{
  auto queue_container = Container::Tab({}, &m_queue_entry);

  auto queue_renderer =
      Renderer(queue_container,
               [this, queue_container]
               {
                 sync_queue();
                 queue_container->DetachAllChildren();

                 if (!m_queue_titles.empty())
                 {
                   auto queue_menu = Menu(&m_queue_titles, &m_queue_entry);
                   queue_menu |= CatchEvent(
                       [this](Event event)
                       {
                         if (event == Event::Return && !m_queue_titles.empty())
                         {
                           if (m_queue_entry < m_session.get_queue().size())
                           {
                             auto music =
                                 m_session.get_queue().at(m_queue_entry);
                             if (auto m = music.lock())
                             {
                               m_status_msg =
                                   std::format("Reproduzindo: '{}'", m->title);
                               m_session.async_play(m->title);
                               m_screen.RequestAnimationFrame();
                             }
                           }
                           return true;
                         }
                         return false;
                       });
                   queue_container->Add(queue_menu);
                 }

                 return vbox({text(m_session.get_queue_size_msg()), separator(),
                              queue_container->Render()});
               });

  return queue_renderer;
}

ftxui::Component Window::build_sort_modal()
{
  auto box = Radiobox(&m_sort_modal_options, &m_sort_modal_selected_option);

  auto ok_button = Button("Ordenar",
                          [this]
                          {
                            sort_criteria crit =
                                (m_sort_modal_selected_option == 0)
                                    ? sort_criteria::TITLE
                                    : sort_criteria::DURATION;

                            if (m_selected_tab == 0)
                            {
                              auto vec = m_session.sort_library(crit);
                              sync_lib(vec);
                              m_status_msg = "Biblioteca ordenada.";
                            }
                            else
                            {
                              m_session.sort_queue(crit);
                              m_status_msg = "Fila ordenada.";
                            }
                            m_sort_modal_open = false;
                          });

  auto cancel_button =
      Button("Cancelar", [this] { m_sort_modal_open = false; });

  auto modal_container = Container::Vertical({
      box,
      Container::Horizontal({ok_button, cancel_button}),
  });

  auto modal_renderer =
      Renderer(modal_container,
               [this, box, ok_button, cancel_button]
               {
                 return vbox({
                            text("Escolha o critério de ordenação:") | bold,
                            separator(),
                            box->Render(),
                            separator(),
                            hbox({ok_button->Render(), text(" "),
                                  cancel_button->Render()}) |
                                center,
                        }) |
                        border | size(WIDTH, GREATER_THAN, 35);
               });

  return modal_renderer;
}

ftxui::Component Window::construct_layout()
{
  auto tab_menu = Menu(&m_tabs, &m_selected_tab);
  auto tab_container = Container::Tab(
      {build_library_component(), build_queue_component()}, &m_selected_tab);

  auto main_container = Container::Horizontal({tab_menu, tab_container});

  auto main_renderer = Renderer(
      main_container,
      [this, tab_menu, tab_container]
      {
        return vbox({text(m_program_name) | bold | center, tab_menu->Render(),
                     separator(), tab_container->Render(),
                     text(m_status_msg) | borderDashed}) |
               border;
      });

  return main_renderer;
}

ftxui::Element Window::render_row(const Music &music, bool focus)
{
  auto row = hbox({
      text(music.title) | flex,
      text(format_duration(music.duration)) | size(WIDTH, EQUAL, 12),
      text(std::to_string(music.year)) | size(WIDTH, EQUAL, 8),
  });

  return focus ? row | inverted | bold : row;
}

bool Window::handle(Event event)
{
  if (event == Event::q)
  {
    m_screen.Exit();
    return true;
  }

  if (event == Event::p)
  {
    if (m_session.get_queue().empty())
      m_status_msg = "Não há nada tocando.";

    else
    {
      m_session.toggle_paused();
      auto current = m_session.get_current().lock();
      m_status_msg =
          m_session.is_paused()
              ? "Pausado."
              : (current ? std::format("Reproduzindo '{}'", current->title)
                         : "Tocando...");
    }
    return true;
  }

  if (event == Event::r)
  {
    if (!m_session.get_queue().empty())
    {
      m_session.shuffle_queue();
      m_status_msg = "Fila embaralhada.";
    }
    return true;
  }

  if (event == Event::s)
  {
    m_sort_modal_open = true;
    return true;
  }

  return false;
}

void Window::run()
{
  auto renderer = Renderer(m_modal_component,
                           [this] { return m_modal_component->Render(); });

  renderer |= CatchEvent([this](Event event) { return handle(event); });

  m_screen.Loop(renderer);
}

} // namespace ui