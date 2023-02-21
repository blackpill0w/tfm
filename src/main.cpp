#include <locale>
#include <ncurses.h>

#include "./file_manager.hpp"
#include "./ncurses_app.hpp"

int main()
{
   setlocale(LC_ALL, "");

   NcursesApp app{};
   FileManager fm{ stdscr };

   while (true)
   {
      const int input = getch();
      if (input == 'q')
      {
         break;
      }
      else if (input == KEY_RESIZE)
      {
         fm.resize(stdscr);
      }
      else if (input == 't')
      {
         wprintw(fm.title_win.getwin(), "sdkjf");
      }
      else if (input == 'p')
      {
         wprintw(fm.prev_win.getwin(), "sdkjf");
      }
      else if (input == 'c')
      {
         wprintw(fm.cwd_win.getwin(), "sdkjf");
      }
      else if (input == 'n')
      {
         wprintw(fm.content_win.getwin(), ": ");
      }
      else if (input == ':')
      {
         wprintw(fm.cmd_win.getwin(), ": ");
      }
      app.refresh_app();
      fm.refresh_fm();
   }
}
