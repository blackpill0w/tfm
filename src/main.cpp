#include <locale>
#include <ncurses.h>

#include "./ncurses_app.hpp"

int main()
{
   setlocale(LC_ALL, "");

   NcursesApp app{};

   while (true)
   {
      const int input = getch();
      if (input == 'q')
      {
         break;
      }
      app.refresh_app();
   }
}
