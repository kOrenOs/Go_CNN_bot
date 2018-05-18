#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sstream>

using namespace std;

/*
     This class parse sgf files, find specific data from records and check validity
     of sgf file.
*/
class sgf_parser
{
     const string WINNER_FLAG = "RE[";
     const string BOARD_SIZE_PATTERN = "SZ[";
     const string WHITE_MOVE_PATTERN = ";W[";
     const string BLACK_MOVE_PATTERN = ";B[";
     const string WHITE_HANDICAP_PATTERN = ";AW[";
     const string BLACK_HANDICAP_PATTERN = ";AB[";
     const int BASIC_CHAR_MOVE_LENGTH = 6;

     bool winner_black;
     bool black_first_player;
     size_t actual_move_index;
     size_t handicap_move_index = 0;
     int board_size;
     string text;

     /*
          Initialise full text from sgf file to text variable.
     */
     void read_dataset_file(string path)
     {
          stringstream str_stream;
          ifstream sgf_file (path);

          str_stream << sgf_file.rdbuf();
          text = str_stream.str();
     }

     /*
          Find winner of match in sgf_file. Return true if it is found, else false.
     */
     bool set_winner()
     {
          char winner_char = get_data_from_brackets(text.find(WINNER_FLAG), WINNER_FLAG.length())[0];

          if(winner_char == 'W'){
               winner_black = false;
               return true;
          }
          if(winner_char == 'B'){
               winner_black = true;
               return true;
          }

          size_t last_black_move_index = text.find_last_of(BLACK_MOVE_PATTERN);
          size_t last_white_move_index = text.find_last_of(WHITE_MOVE_PATTERN);

          if(last_black_move_index == string::npos || last_white_move_index == string::npos){
               return false;
          }

          if(last_black_move_index > last_white_move_index){
               winner_black = true;
          }else{
               winner_black = false;
          }

          return true;
     }

     int getboard_size()
     {
          size_t size_index = text.find(BOARD_SIZE_PATTERN);

          if(size_index == string::npos){
               return 0;
          }

          return stoi(get_data_from_brackets(size_index, BOARD_SIZE_PATTERN.length()), nullptr);
     }

     /*
          Try to find if there are some handicap moves in sgf_file.
     */
     bool check_handicap_moves_are_on_beginning(){
          handicap_move_index = 0;

          find_handicap_moves(true);
          if(handicap_move_index > actual_move_index){
               handicap_move_index = 0;
               return false;
          }

          find_handicap_moves(false);
          if(handicap_move_index > actual_move_index){
               handicap_move_index = 0;
               return false;
          }

          handicap_move_index = 0;
          return true;
     }

     /*
          Set point in sgf_file, where section with players moves sterts.
     */
     bool set_game_start_index()
     {
          actual_move_index = text.find(BLACK_MOVE_PATTERN);

          if(actual_move_index == string::npos){
               return false;
          }

          size_t tmp = text.find(WHITE_MOVE_PATTERN);

          if(tmp == string::npos){
               return false;
          }

          if(tmp < actual_move_index){
               actual_move_index = tmp;
               black_first_player = false;
          }else{
               black_first_player = true;
          }

          return true;
     }

public:
     bool open_new_sgf_file(string path, int board_size){
          handicap_move_index = 0;

          read_dataset_file(path);

          if(text.length() > 0 && set_winner() && set_game_start_index() && check_handicap_moves_are_on_beginning() && getboard_size() == board_size){
               return true;
          }
          return false;
     }

     bool check_next_move_index(bool blacksMove)
     {
          char nextMoveColor = text[actual_move_index + 1];

          if((blacksMove && nextMoveColor != 'B') || (!blacksMove &&  nextMoveColor != 'W')){
               return false;
          }

          return true;
     }

     /*
          Return data inside brackets which started in specified position.
     */
     string get_data_from_brackets(size_t index, int paternLength)
     {
          size_t tmp = text.find(']', index + 1);

          if(tmp == string::npos){
               return "";
          }
          return text.substr(index + paternLength, tmp - (index + paternLength));
     }

     /*
          Return next handicap move from sgf.
     */
     string find_handicap_moves(bool blackPlayer)
     {
          string patern;
          size_t tmp;

          if(blackPlayer){
               patern = BLACK_HANDICAP_PATTERN;
          }else{
               patern = WHITE_HANDICAP_PATTERN;
          }

          if(handicap_move_index == 0){
               tmp = text.find(patern);
          }else{
               tmp = text.find(patern, handicap_move_index);
          }

          if(tmp  == string::npos){
               return "";
          }
          handicap_move_index = tmp + 1;

          return get_data_from_brackets(tmp, patern.length());
     }

     /*
          Return next palyer's move from sgf.
     */
     string get_next_move(bool blacks_move)
     {
          string tmp = get_data_from_brackets(actual_move_index, WHITE_MOVE_PATTERN.length());

          actual_move_index += BASIC_CHAR_MOVE_LENGTH;

          if(actual_move_index < text.length() - 2 && text[actual_move_index] != ';'){
               //cout << "Last move index " << actual_move_index << '\n';

               if(blacks_move){
                    actual_move_index = text.find(BLACK_MOVE_PATTERN, actual_move_index - (BASIC_CHAR_MOVE_LENGTH - 1));
               }else{
                    actual_move_index = text.find(WHITE_MOVE_PATTERN, actual_move_index - (BASIC_CHAR_MOVE_LENGTH - 1));
               }

               if(actual_move_index == string::npos){
                    //cout << "End file: " << setPath << '\n';
               }
          }

          return tmp;
     }

     bool is_end_of_file()
     {
          return actual_move_index < text.length() - 2;
     }

     void reset_handicap_index()
     {
          handicap_move_index = 0;
     }

     bool is_black_first_player()
     {
          return black_first_player;
     }

     bool is_black_winner()
     {
          return winner_black;
     }
};
