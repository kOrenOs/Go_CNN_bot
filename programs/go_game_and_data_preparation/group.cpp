#pragma once

/*
     Supplementary Data class representing group of connected stones with same colour.
*/
class group{
     int x_position;
     int y_position;
     int freedom_lvl;
     bool black_player;

public:
     group(int x, int y, int freedom, bool black)
     {
          x_position = x;
          y_position = y;
          freedom_lvl = freedom;
          black_player = black;
     }
     
     int get_x_position()
     {
          return x_position;
     }

     int get_y_position()
     {
          return y_position;
     }

     int get_freedom_lvl()
     {
          return freedom_lvl;
     }

     bool get_black_player()
     {
          return black_player;
     }

     void set_freedom_lvl(int freedom)
     {
          freedom_lvl = freedom;
     }
};
