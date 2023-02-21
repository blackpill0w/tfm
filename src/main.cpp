#include <filesystem>
#include <locale>
#include <ncurses.h>

#include "./file_manager.hpp"
#include "./fs_utils.hpp"
#include "./ncurses_app.hpp"
#include "./cmd_loop.hpp"

namespace fs = std::filesystem;

int main()
{
   setlocale(LC_ALL, "");

   NcursesApp app{};
   start_color();
   init_pair(1, COLOR_CYAN, 0); // color for directories
   init_pair(2, COLOR_GREEN, 0);  // color for links
   init_pair(3, COLOR_RED, 0);  // color for errors

   FileManager fm{ stdscr };
   vector<FileItem> dir_content{};
   vector<FileItem> prev_dir_files{};

   int selected = 0;
   int begin = 0;

   while (true)
   {
      Err prev_dir_e = get_dir_content(prev_dir_files, "..");
      Err err = get_dir_content(dir_content);

      if (prev_dir_e == Err::PermissionDenied)
         wprintw(fm.prev_win.getwin(), "Permission denied");
      else
         print_dir_content(prev_dir_files, fm.prev_win.getwin());

      if (err == Err::PermissionDenied)
         wprintw(fm.cwd_win.getwin(), "Permission denied");
      else
         print_dir_content(dir_content, fm.cwd_win.getwin(), selected, begin);

      refresh();
      fm.refresh_fm();

      const int input = getch();

      if (input == 'q')
      {
         break;
      }
      else if (input == ':')
      {
         // Erase anything that might be in the cmd_win
         werase(fm.cmd_win.getwin());
         // Display the prompt
         wprintw(fm.cmd_win.getwin(), ": ");
         wrefresh(fm.cmd_win.getwin());
         // Get input
         std::string cmd_str = cmd_loop(fm.cmd_win.getwin());
         // Execute
         Command cmd = exec_cmd(cmd_str, dir_content[(size_t)selected].name);
         fm.erase_fm();
         // Display err if necessary
         if (cmd == Command::Quit)
            break;
         else if (cmd == Command::MissingArgs)
            fm.display_err("Arguments missing");
         else if (cmd == Command::Unknown)
            fm.display_err("Unknown command");
      }
      else if (input == KEY_RESIZE)
      {
         fm.resize(stdscr);
      }
      else if (input == KEY_RIGHT)
      {
         fm.erase_fm();
         if (dir_content[(size_t)selected].type == FileType::Directory)
         {
            try
            {
               fs::current_path(dir_content[(size_t)selected].name);
               selected = 0;
            }
            catch (fs::filesystem_error &e)
            {
               fm.display_err("Error: Permission denied");
            }
         }
         else
         {
            fm.display_err("Error: Not a directory");
         }
      }
      else if (input == KEY_LEFT)
      {
         fm.erase_fm();
         fs::current_path("..");
         selected = 0;
      }
      else if (input == KEY_DOWN)
      {
         fm.erase_fm();
         if ((size_t)selected < dir_content.size() - 1)
         {
            int maxy = getmaxy(fm.cwd_win.getwin());
            ++selected;
            if (selected - begin >= maxy)
               ++begin;
         }
      }
      else if (input == KEY_UP)
      {
         fm.erase_fm();
         if ((size_t)selected > 0)
         {
            --selected;
            if (selected - begin < 0)
               --begin;
         }
      }
      else
      {
         fm.erase_fm();
      }
   }
}
