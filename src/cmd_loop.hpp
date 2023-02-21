#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include <ncurses.h>

using std::string;
using std::vector;

#include "./str_utils.hpp"

namespace fs = std::filesystem;

enum class Command
{
   DeleteFile,
   RenameFile,
   Quit,
   Unknown,
   MissingArgs
};

/**
 * Pareses input, executes the command, and returns the command executed (see enum Command).
 */
Command exec_cmd(string cmd, const string &selected_file)
{
   vector<string> tokens{ split(cmd, " ") };

   if (tokens[0] == "q")
   {
      return Command::Quit;
   }
   else if (tokens[0] == "rm")
   {
      fs::remove_all(selected_file);
      return Command::DeleteFile;
   }
   else if (tokens[0] == "rename")
   {
      if (tokens.size() < 2)
         return Command::MissingArgs;
      else
         fs::rename(selected_file, tokens[1]);
      return Command::RenameFile;
   }
   return Command::Unknown;
}

std::string cmd_loop(WINDOW *cmd_win)
{
   std::string s{};
   while (true)
   {
      const int input = getch();
      if (input == '\n')
      {
         break;
      }
      else if ((isalpha(input) || input == ' '))
      {
         waddch(cmd_win, unsigned(input));
         wrefresh(cmd_win);
         s += char(input);
      }
   }
   return s;
}
