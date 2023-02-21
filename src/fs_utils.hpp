#pragma once

#include <algorithm>
#include <filesystem>
#include <string>
#include <vector>

#include <ncurses.h>

using std::string;
using std::vector;
namespace fs = std::filesystem;

enum Err
{
   None,
   PermissionDenied
};
enum FileType
{
   Unknown,
   File,
   Directory,
   Link
};
struct FileItem
{
   string name;
   FileType type;
   FileItem(const string &n, const FileType ft) : name{ n }, type{ ft }
   {
   }
   // For std::sort()
   bool operator<(const FileItem &other)
   {
      // Dot file are last
      if (name[0] == '.' && other.name[0] != '.')
         return false;
      if (other.name[0] == '.' && name[0] != '.')
         return true;
      return name < other.name;
   }
};

Err get_dir_content(vector<FileItem> &v, string path = ".")
{
   /*
   ** Clear the vector, then go to the path,
   ** get content and return to the original position.
   */
   v.clear();

   auto cwd{ fs::current_path() };
   try
   {
      fs::current_path(cwd / path);
   }
   catch (fs::filesystem_error &e)
   {
      return Err::PermissionDenied;
   }
   for (const auto &dir_entry : fs::directory_iterator{ "." })
   {
      FileItem fi{ dir_entry.path().filename(), FileType::Unknown };
      if (dir_entry.is_regular_file())
         fi.type = FileType::File;
      else if (dir_entry.is_directory())
         fi.type = FileType::Directory;
      else if (dir_entry.is_symlink())
         fi.type = FileType::Link;
      v.emplace_back(fi);
   }
   fs::current_path(cwd);
   std::sort(v.begin(), v.end());
   return Err::None;
}

void print_dir_content(const vector<FileItem> &v, WINDOW *win, const int selected = -1, const int begin = 0)
{
   size_t i = (size_t)begin;
   size_t maxy = (size_t)getmaxy(win);
   for (; i < v.size() && i - (size_t)begin < maxy; ++i)
   {
      if (i == (size_t)selected)
         wattron(win, A_REVERSE);
      if (v[i].type == FileType::Directory)
         wattron(win, COLOR_PAIR(1) | A_BOLD);
      else if (v[i].type == FileType::Link)
         wattron(win, COLOR_PAIR(2));

      wprintw(win, " - %s\n", v[i].name.c_str());

      wattroff(win, A_REVERSE);
      wattroff(win, COLOR_PAIR(1) | A_BOLD);
      wattroff(win, COLOR_PAIR(2));
   }
}
