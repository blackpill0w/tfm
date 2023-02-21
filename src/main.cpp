#include <filesystem>
#include <locale>
#include <ncurses.h>

#include "./file_manager.hpp"
#include "./fs_utils.hpp"
#include "./ncurses_app.hpp"

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
      Err e = get_dir_content(dir_content);

      if (prev_dir_e == Err::PermissionDenied)
         wprintw(fm.prev_win.getwin(), "Permission denied");
      else
         print_dir_content(prev_dir_files, fm.prev_win.getwin());

      if (e == Err::PermissionDenied)
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
      else if (input == KEY_RESIZE)
      {
         fm.resize(stdscr);
      }
      else if (input == KEY_RIGHT)
      {
         fm.erase_fm();
         if (dir_content[selected].type == FileType::Directory)
         {
            try
            {
               fs::current_path(dir_content[selected].name);
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
      else if (input == ':')
      {
         wprintw(fm.cmd_win.getwin(), ": ");
      }
      else
      {
         fm.erase_fm();
      }
   }
}
