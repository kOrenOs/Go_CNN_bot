#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include "group.cpp"
#include "board.cpp"

#pragma once

using namespace std;

/*
     Class containing logic and rules of go game.
*/
class go_game
{
     int dimension_length;

     bool blacks_turn;
     board play_board;

     int x_actual_position;
     int y_actual_position;

     map<int, group> groups;
     int next_group_number = 1;

     bool player_passed = false;
     bool game_finished = false;

public:
     static const int ROTATION_DEGREE_0 = 0;
     static const int ROTATION_DEGREE_90 = 90;
     static const int ROTATION_DEGREE_180 = 180;
     static const int ROTATION_DEGREE_270 = 270;

private:

     /*
          Check neighbourhood groups, update theirs freedom lvls and remove groups
          with zero freedom lvl.
     */
     void check_neighbourhood(int x, int y, bool black)
     {
          vector<vector<int>> neighbours = play_board.find_neighbour_groups(x, y, black);

          vector<int> neighbour_groups = neighbours[0];
          vector<int> same_colour_groups = neighbours[1];

          if(same_colour_groups.empty()){
               add_new_group(x,y, black);
          }

          if(same_colour_groups.size() == 1){
               add_to_existing_group(x, y, same_colour_groups[0], black);
          }

          update_freedom_lvl_of_neighbours(neighbour_groups);

          if(same_colour_groups.size() > 1){
               collapse_groups(x, y, same_colour_groups);
          }

          check_for_zero_groups(x, y, neighbour_groups, same_colour_groups);
     }

     /*
          Collapse groups of same colour, which were joined by new added stone.
     */
     void collapse_groups(int x, int y, vector<int>& same_colour_groups)
     {
          play_board.set_group(x, y, same_colour_groups[0]);
          int new_freedom_lvl = 0;

          for(size_t i = 0; i < same_colour_groups.size(); i++){
               bool unique_group = true;

               for(size_t j = 0; j < i; j++){
                    if(same_colour_groups[i] == same_colour_groups[j]){
                         unique_group = false;
                         break;
                    }
               }
               if(unique_group){
                    group collapsing_group = groups.find(same_colour_groups[i])->second;
                    new_freedom_lvl += collapsing_group.get_freedom_lvl();
               }

               if(same_colour_groups[i] != same_colour_groups[0] && groups.find(same_colour_groups[i]) != groups.end()){
                    change_whole_group(same_colour_groups[i], same_colour_groups[0]);
                    groups.erase(same_colour_groups[i]);
               }
          }

          set_or_add_freedom_lvl_of_group(same_colour_groups[0], new_freedom_lvl + play_board.check_freedom_of_position(x, y), true);
     }

     /*
          Check if parametred groups have zero freedom lvl after last move.
     */
     void check_for_zero_groups(int x, int y, vector<int>& neighbours, vector<int>& same_colour_groups)
     {
          same_colour_groups.push_back(play_board.get_group(x, y));

          vector_subtract(neighbours, same_colour_groups);

          if(delete_freedom_zero_group(neighbours)){
               recursive_freedom_update();
          }

          if(delete_freedom_zero_group(same_colour_groups)){
               recursive_freedom_update();
          }
     }

     /*
          Delete groups with freedom lvl 0.
     */
     bool delete_freedom_zero_group(vector<int>& groups_for_check)
     {
          bool deleted = false;

          for(auto &grp_for_check : groups_for_check){
               if(groups.find(grp_for_check) != groups.end()){
                    group tmp = groups.find(grp_for_check)->second;
                    if(tmp.get_freedom_lvl() == 0){
                         change_whole_group(grp_for_check, 0);
                         groups.erase(grp_for_check);

                         deleted = true;
                    }
                    if(tmp.get_freedom_lvl() < 0){
                         cout << "Something went very wrong. Negative freedom lvl.";
                    }
               }
          }

          return deleted;
     }

     /*
          Remove from one vector all records contained in second vector.
     */
     void vector_subtract(vector<int>& neighbours, vector<int>& same_colour_groups)
     {
          for(auto same_colour : same_colour_groups){
               if(find(neighbours.begin(), neighbours.end(), same_colour) != neighbours.end()){
                    neighbours.erase(remove(neighbours.begin(), neighbours.end(), same_colour), neighbours.end());
               }
          }
     }

     int compute_position_of_move_in_matrix(int x, int y){
		return y * dimension_length + x;
	}

     /*
          Check suicide rule in go game.
     */
     bool check_suicide(int x, int y, bool black_player)
     {
          if(play_board.check_freedom_of_position(x,y) != 0){
               return false;
          }

          vector<vector<int>> neighbours = play_board.find_neighbour_groups(x, y, black_player);
          bool opponent_zero_freedom = false;
          bool actual_player_zero_freedom = false;
          bool no_zero_freedom_actual_player = true;

          map<int, int> freedom_of_groups;
          map<int, int> nighbours_of_same_group;
          for(size_t i = 0; i < neighbours[0].size(); i++){
               freedom_of_groups[neighbours[0][i]] += groups.at(neighbours[0][i]).get_freedom_lvl();
               nighbours_of_same_group[neighbours[0][i]]++;
          }

          for (auto& same_groups_count: nighbours_of_same_group) {
               freedom_of_groups[same_groups_count.first] -= same_groups_count.second * same_groups_count.second;
          }

          for (auto& freedom_group: freedom_of_groups) {
               if(freedom_group.second == 0){
                    if(groups.at(freedom_group.first).get_black_player() == black_player){
                         if(no_zero_freedom_actual_player){
                              actual_player_zero_freedom = true;
                         }
                    }else{
                         opponent_zero_freedom = true;
                    }
               }else{
                    if(groups.at(freedom_group.first).get_black_player() == black_player){
                         no_zero_freedom_actual_player = false;
                         actual_player_zero_freedom = false;
                    }
               }
          }

          if((actual_player_zero_freedom && !opponent_zero_freedom) || (neighbours[1].size() == 0 && !opponent_zero_freedom)){
               return true;
          }

          return false;
     }

     /*
          Create new group of stones on board.
     */
     void add_new_group(int x, int y, bool black)
     {
          //cout << "1 position x " << x << " , position y " << y << '\n';
          groups.insert(pair<int,group>(next_group_number,group(x, y, play_board.check_freedom_of_position(x, y), black)));
          play_board.set_group(x, y, next_group_number);

          next_group_number++;
     }

     void change_whole_group(int group_no, int changed_no)
     {
          group tmp = groups.find(group_no)->second;
          bool erase = false;

          if(changed_no == 0){
               erase = true;
          }
          play_board.recursively_change_group(tmp.get_x_position(), tmp.get_y_position(), group_no, changed_no, erase);
     }

     /*
          Update freedom level of all groups on board.
     */
     void recursive_freedom_update()
     {
          play_board.recursive_map_reset();

          for (auto &saved_group : groups) {
               group &tmp = saved_group.second;
               tmp.set_freedom_lvl(play_board.recursively_compute_freedom(tmp.get_x_position(), tmp.get_y_position(), saved_group.first));
          }
     }

     /*
          Update freedom level of parametred groups.
     */
     void update_freedom_lvl_of_neighbours(vector<int>& neighbours)
     {
          for(auto &neighbour : neighbours){
               group &tmp = groups.find(neighbour)->second;
               tmp.set_freedom_lvl(tmp.get_freedom_lvl() - 1);
          }
     }

     void set_or_add_freedom_lvl_of_group(int group_no, int freedom_lvl, bool set_new_freedom)
     {
          group &new_stone = groups.find(group_no)->second;
          if(set_new_freedom){
               new_stone.set_freedom_lvl(freedom_lvl);
          }else{
               new_stone.set_freedom_lvl(new_stone.get_freedom_lvl() + freedom_lvl);
          }
     }

     /*
          Add stone to one of groups on board.
     */
     void add_to_existing_group(int x, int y, int group_no, bool black)
     {
          play_board.set_group(x, y, group_no);
          set_or_add_freedom_lvl_of_group(group_no, play_board.check_freedom_of_position(x, y), false);
     }

public:

     /*
          Return number of classes of actual go board.
     */
     static int maxClassNumbers(int dimension){
          return dimension * dimension + 1;
     }

     go_game(int dimension = 19, bool black_first = true)
     :play_board(dimension)
     {
          dimension_length = dimension;
          blacks_turn = black_first;
     }

     bool check_move_is_legal(int x, int y)
     {
          if(game_finished){
               return false;
          }
          if(x == dimension_length && y == dimension_length){
               return true;
          }
          if(!play_board.add_stone(y, x, blacks_turn)){
               return false;
          }
          if(check_suicide(y, x, blacks_turn)){
               play_board.remove_stone(y, x);
               return false;
          }

          play_board.remove_stone(y, x);
          return true;
     }

     bool move(string position, bool black_player)
     {
          bool tmp = blacks_turn;
          bool success;
          blacks_turn = black_player;

          success = move(position);
          blacks_turn = tmp;

          return success;
     }

     bool move(int x, int y)
     {
          if(game_finished){
               return false;
          }

          if(x == dimension_length && y == dimension_length){
               x_actual_position = x;
               y_actual_position = y;

               if(player_passed){
                    game_finished = true;
               }else{
                    player_passed = true;
               }

               return true;
          }

          if(!play_board.add_stone(y, x, blacks_turn)){
               return false;
          }

          if(check_suicide(y, x, blacks_turn)){
               play_board.remove_stone(y, x);
               return false;
          }

          player_passed = false;

          x_actual_position = x;
          y_actual_position = y;

          check_neighbourhood(y, x, blacks_turn);
          blacks_turn = !blacks_turn;

          return true;
     }

     bool move(string position)
     {
          if(position[0] < 97 || position[1] < 97 || position[0] > 116 || position[1] > 116){
               return false;
          }

          return move(position[0] - 97, position[1] - 97);
     }

     /*
          Return serialised actual board stone positions.
     */
     vector< vector< vector<int> > > create_trainset(bool black_first)
     {
          vector< vector< vector<int> > > trainsetVector;

          for(int i = 0; i < 3; i++){
               vector< vector<int> > tmp;
               for(int j = 0; j < dimension_length; j++){
                    tmp.push_back(vector<int> (dimension_length, 0));
               }

               trainsetVector.push_back(tmp);
          }

          play_board.get_dataset_vector(trainsetVector, black_first);

          return trainsetVector;
     }

     /*
          Return class code of last played move.
     */
     int code_number_of_last_move(int rotation)
     {
          return create_right_move_code_number(x_actual_position, y_actual_position, rotation);
     }

     /*
          Return class code of board position from parameters.
     */
     int create_right_move_code_number(int x, int y, int rotation)
     {
          if(x == dimension_length && y == dimension_length){
               return dimension_length * dimension_length;
          }

          int tmp;

		switch (rotation) {
		case ROTATION_DEGREE_0:
			break;
		case ROTATION_DEGREE_90:
			tmp = x;
			x = (dimension_length - 1) - y;
			y = tmp;
			break;
		case ROTATION_DEGREE_180:
			x = (dimension_length - 1) - x;
			y = (dimension_length - 1) - y;
		    	break;
		case ROTATION_DEGREE_270:
			tmp = x;
			x = y;
			y = (dimension_length - 1) - tmp;
		    	break;
		}

		return compute_position_of_move_in_matrix(x, y);
     }

     /*
          Return board position form code number of position.
     */
     vector<int> get_real_position(int code_number)
     {
         int x, y;
         vector<int> output;

         if(code_number == dimension_length*dimension_length){
             x = dimension_length;
             y = dimension_length;
         }else{
             x = code_number % dimension_length;
             y = code_number / dimension_length;
         }

         output.push_back(x);
         output.push_back(y);

         return output;
     }

     /*
          Return board stone positions serialised in openCV compatible way.
     */
     vector <unsigned char> get_openCV_compatible_board_array(bool black_first)
     {
          vector <unsigned char> output_array;

          vector< vector< vector<int> > > image_data = create_trainset(black_first);

          for(int i = 0; i < dimension_length; i++){
               for(int j = 0; j < dimension_length; j++){
                    for(int k = 0; k < 3; k++){
                         if(image_data[k][i][j] != 0){
                              output_array.push_back(255);
                         }else{
                              output_array.push_back(0);
                         }
                    }
               }
          }

          return output_array;
     }

     bool is_game_finished()
     {
          return game_finished;
     }

     bool is_blacks_turn()
     {
          return blacks_turn;
     }

     void print_board()
     {
          play_board.print_board(true);
     }

     void print_groups()
     {
          play_board.print_board(false);
     }
};
