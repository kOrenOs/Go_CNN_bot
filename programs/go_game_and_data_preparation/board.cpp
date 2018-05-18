#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

#pragma once

using namespace std;

/*
     Class holds board logic with all stones placed on board. Makes basic actions
     as adding and removing stones, computing freedom lvl of positions and groups
*/
class board
{
     int dimension_length;
     int black_player_representation = 5;
     int white_player_representation = 1;

     vector< vector<int> > play_board_vector;
     vector< vector<int> > group_mapping;
     vector< vector<bool> > recursive_scan_map;

public:
     board(int dimension)
     {
          dimension_length = dimension;

          for(int i = 0; i < dimension_length; i++){
               play_board_vector.push_back(vector<int> (dimension_length, 0));
          }

          for(int i = 0; i < dimension_length; i++){
               group_mapping.push_back(vector<int> (dimension_length, 0));
          }
     }

     bool add_stone(int x, int y, bool black_player)
     {
          if(!(x >= 0 && y >= 0 && x < dimension_length && y < dimension_length)){
               return false;
          }
          if(play_board_vector[x][y] != 0){
               return false;
          }

          if(black_player){
               play_board_vector[x][y] = black_player_representation;
          }else{
               play_board_vector[x][y] = white_player_representation;
          }

          return true;
     }

     int get_group(int x, int y)
     {
          return group_mapping[x][y];
     }

     void set_group(int x, int y, int group_number)
     {
          group_mapping[x][y] = group_number;
     }

     void remove_stone(int x, int y)
     {
          play_board_vector[x][y] = 0;
     }

     /*
          Return freedom level of single board position.
     */
     int check_freedom_of_position(int x, int y)
     {
          int freedom_lvl = 0;

          if(x - 1 >= 0){
               if(play_board_vector[x - 1][y] == 0){
                    freedom_lvl++;
               }
          }
          if(x + 1 < dimension_length){
               if(play_board_vector[x + 1][y] == 0){
                    freedom_lvl++;
               }
          }
          if(y - 1 >= 0){
               if(play_board_vector[x][y - 1] == 0){
                    freedom_lvl++;
               }
          }
          if(y + 1 < dimension_length){
               if(play_board_vector[x][y + 1] == 0){
                    freedom_lvl++;
               }
          }

          return freedom_lvl;
     }

     /*
          Return all groups which are in the near neighbourhood of parametred position.
          Returned groups are divided into two parts: black and white stone groups
     */
     vector< vector<int> > find_neighbour_groups(int x, int y, bool black)
     {
          vector<int> neighbour_groups;
          vector<int> same_colour_groups;
          int colour_representation;

          vector<vector<int>> output;

          if(black){
               colour_representation = black_player_representation;
          }else{
               colour_representation = white_player_representation;
          }

          if(x - 1 >= 0){
               if(play_board_vector[x - 1][y] != 0){
                    neighbour_groups.push_back(group_mapping[x - 1][y]);
                    if(play_board_vector[x - 1][y] == colour_representation){
                         same_colour_groups.push_back(group_mapping[x - 1][y]);
                    }
               }
          }
          if(x + 1 < dimension_length){
               if(play_board_vector[x + 1][y] != 0){
                    neighbour_groups.push_back(group_mapping[x + 1][y]);
                    if(play_board_vector[x + 1][y] == colour_representation){
                         same_colour_groups.push_back(group_mapping[x + 1][y]);
                    }
               }
          }
          if(y - 1 >= 0){
               if(play_board_vector[x][y - 1] != 0){
                    neighbour_groups.push_back(group_mapping[x][y - 1]);
                    if(play_board_vector[x][y - 1] == colour_representation){
                         same_colour_groups.push_back(group_mapping[x][y - 1]);
                    }
               }
          }
          if(y + 1 < dimension_length){
               if(play_board_vector[x][y + 1] != 0){
                    neighbour_groups.push_back(group_mapping[x][y + 1]);
                    if(play_board_vector[x][y + 1] == colour_representation){
                         same_colour_groups.push_back(group_mapping[x][y + 1]);
                    }
               }
          }

          output.push_back(neighbour_groups);
          output.push_back(same_colour_groups);

          return output;
     }

     /*
          Change group into different group on the board (merging) or remove group
          from board.
     */
     void recursively_change_group(int init_x, int init_y, int affected_group, int change_to, bool erase_stones)
     {
          group_mapping[init_x][init_y] = change_to;

          if(erase_stones){
               play_board_vector[init_x][init_y] = 0;
          }

          if(init_x - 1 >= 0){
               if(group_mapping[init_x - 1][init_y] == affected_group){
                    recursively_change_group(init_x - 1, init_y, affected_group, change_to, erase_stones);
               }
          }
          if(init_x + 1 < dimension_length){
               if(group_mapping[init_x + 1][init_y] == affected_group){
                    recursively_change_group(init_x + 1, init_y, affected_group, change_to, erase_stones);
               }
          }
          if(init_y - 1 >= 0){
               if(group_mapping[init_x][init_y - 1] == affected_group){
                    recursively_change_group(init_x, init_y - 1, affected_group, change_to, erase_stones);
               }
          }
          if(init_y + 1 < dimension_length){
               if(group_mapping[init_x][init_y + 1] == affected_group){
                    recursively_change_group(init_x, init_y + 1, affected_group, change_to, erase_stones);
               }
          }
     }
     /*
          Compute freedom lvl of whole group.
     */
     int recursively_compute_freedom(int init_x, int init_y, int affected_group)
     {
          int freedom_lvl = 0;
          recursive_scan_map[init_x][init_y] = true;

          if(init_x - 1 >= 0){
               if(group_mapping[init_x - 1][init_y] == affected_group && recursive_scan_map[init_x - 1][init_y] == false){
                    freedom_lvl += recursively_compute_freedom(init_x - 1, init_y, affected_group);
               }
          }
          if(init_x + 1 < dimension_length){
               if(group_mapping[init_x + 1][init_y] == affected_group && recursive_scan_map[init_x + 1][init_y] == false){
                    freedom_lvl += recursively_compute_freedom(init_x + 1, init_y, affected_group);
               }
          }
          if(init_y - 1 >= 0){
               if(group_mapping[init_x][init_y - 1] == affected_group && recursive_scan_map[init_x][init_y - 1] == false){
                    freedom_lvl += recursively_compute_freedom(init_x, init_y - 1, affected_group);
               }
          }
          if(init_y + 1 < dimension_length){
               if(group_mapping[init_x][init_y + 1] == affected_group && recursive_scan_map[init_x][init_y + 1] == false){
                    freedom_lvl += recursively_compute_freedom(init_x, init_y + 1, affected_group);
               }
          }

          return freedom_lvl + check_freedom_of_position(init_x, init_y);
     }

     /*
          Reset secondary data structure used for recursive scanning.
     */
     void recursive_map_reset()
     {
          recursive_scan_map.clear();

          for(int i = 0; i < dimension_length; i++){
               recursive_scan_map.push_back(vector<bool> (dimension_length, 0));
          }
     }

     /*
          Serialisation of actual board stone positions into multidimensional vector.
     */
     void get_dataset_vector(vector< vector< vector<int> > > &created_vector, bool black_first)
     {
          int tmp;
          int first_player_representation = white_player_representation;

          if(black_first){
               first_player_representation = black_player_representation;
          }

          for(int i = 0; i < dimension_length; i++){
               for(int j = 0; j < dimension_length; j++){
                    tmp = play_board_vector[i][j];
                    if(tmp == first_player_representation){
                         created_vector[0][i][j] = 1;
                    }else{
                         if(tmp == 0){
                              created_vector[2][i][j] = 1;
                         }else{
                              created_vector[1][i][j] = 1;
                         }
                    }
               }
          }

     }

     /*
          Print actual board position into terminal.
     */
     void print_board(bool stones)
     {
          cout << "    ";
          for(int i = 1; i < dimension_length + 1; i++){
               cout << i;
               if(i >= 10){
                    cout << "  ";
               }else{
                    cout << "   ";
               }
          }
          cout << endl << "   ";

          for(int i = 0; i < dimension_length; i++){
               cout << "----";
          }
          cout << endl;

          for(int i = 0; i < dimension_length; i++){
               if(i+1 >= 10){
                    cout << i+1 << "| ";
               }else{
                    cout << i+1 << " | ";
               }
               for(int j = 0; j < dimension_length; j++){
                    if(stones){
                         if(play_board_vector[i][j] == 0){
                              cout << " ";
                         }else{
                              cout << play_board_vector[i][j];
                         }
                         if(j < dimension_length-1){
                              cout << " - ";
                         }
                    }else{
                         if(group_mapping[i][j] == 0){
                              cout << " ";
                         }else{
                              cout << group_mapping[i][j];
                         }
                         if(j < dimension_length-1){
                              cout << " - ";
                         }
                    }
               }
               cout << endl << "    ";

               if(i < dimension_length-1){
                    for(int j = 0; j < dimension_length; j++){
                         cout << "|   ";
                    }
                    cout << endl;
               }
          }
          cout << '\n';
     }
};
