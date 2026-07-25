#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "../msession.hpp"
#include "../music.hpp"
#include "../utils.h"
#include <ftxui/component/app.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <memory>
#include <string>
#include <vector>

namespace ui
{
class Window
{
public:
  Window();
  bool init();
  void run();

private:
  const std::string m_program_name{"SPETIFY"};
  MSession m_session;
  ftxui::ScreenInteractive m_screen;
  std::string m_status_msg;
  int m_selected_tab = 0;
  int m_lib_entry = 0;
  int m_queue_entry = 0;
  std::vector<std::string> m_tabs{"Biblioteca", "Fila de Reprodução"};
  std::vector<std::string> m_sort_modal_options{"Título", "Duração"};
  std::vector<std::string> m_lib_titles;
  std::vector<std::string> m_queue_titles;
  bool m_sort_modal_open = false;
  int m_sort_modal_selected_option = 0;

  // TODO: remover esse campo aqui e testar com variável normal no
  // build_lib_component
  ftxui::Component m_lib_menu;
  ftxui::Component m_modal_component;
  ftxui::Component m_main_layout;
  void sync_lib(const std::vector<std::shared_ptr<Music>> &);
  void sync_queue();
  ftxui::Component build_library_component();
  ftxui::Component build_queue_component();
  ftxui::Component build_sort_modal();
  ftxui::Component construct_layout();
  ftxui::Element render_row(const Music &, bool);
  bool handle(ftxui::Event event);
};

} // namespace ui

#endif