#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sstream>
#include "go_game.cpp"
#include "sgf_parser.cpp"
#include "image_creator.cpp"
#include "dataset_image_unique.cpp"
#include "rotate_image.cpp"

using namespace std;

/*
     Class simulate game, send there move commands from sgf_parser class and gather
     data to create dataset.
*/
class data_preparation
{
     const string DATASET_FORMAT_EXTENSION = ".sgf";
     const string DATASET_FILE_NAME_TEMPLATE = "dataset_image";
     string INPUT_FOLDER = "./execute_sgf";
     string DATASET_FOLDER_PATH = "./new_dataset";
     const string DATASET_FOLDER_NAME = "Go_train_dataset";
     const string DATASET_TXT_FILE_NAME = "dataset_train.txt";

     const int SIZE_OF_BOARD = 19;
     vector <vector <string>> map_dataset_list;

     vector <string> found_files;
     int correct_datasets;
     bool blacks_move;

     vector <string> dataset_picture_paths;
     vector <int> right_moves;

     int actual_dataset_file_index = 0;
     int actual_picture_number_in_file = 0;
     int max_picture_number_in_folder = 2000;

     int all_images = 0;
     int not_unique_images = 0;

     sgf_parser parser;
     dataset_image_unique unique_check;
     image_creator dataset_image_creator;
     rotate_image to_rotate_image;

     /*
          Find all dataset files (.sgf extension) in parametred directory and all
          his subdirectories.
     */
     void find_all_dataset_files_in_dir(string actual_position)
     {
          DIR* input_dir = opendir(actual_position.c_str());
          dirent* directory;

          if (input_dir){
               while ((directory = readdir(input_dir)) != NULL){
                    string tmp(directory->d_name);

                    if(tmp.find(DATASET_FORMAT_EXTENSION) != string::npos){
                         found_files.push_back(actual_position + "/" + tmp);
                    }else{
                         if(tmp[0] != '.' && tmp.length() > 2){
                              find_all_dataset_files_in_dir(actual_position + "/" + tmp);
                         }
                    }
               }
               closedir(input_dir);
          }else{
               cout << "Error: Input directory " << actual_position << ", not found." << '\n';
          }
     }

     /*
          Set maximum number of dataset images in folder. Depends on the level of
          used augmentation.
     */
     void set_file_count_limits(int augmentation_8)
     {
          if(augmentation_8 == 1){
               max_picture_number_in_folder = max_picture_number_in_folder / 4;
          }
          if(augmentation_8 == 2){
               max_picture_number_in_folder = max_picture_number_in_folder / 8;
          }
     }

     /*
          Place handicap moves on board. Moves are parsed from sgf_parser.
     */
     bool place_handicap_stones(bool black_player, go_game &game)
     {
          string move_position;
          bool handicap_stone_placed = false;

          move_position = parser.find_handicap_moves(black_player);

          while(move_position.compare("") != 0){
               if(game.move(move_position, black_player)){
                    handicap_stone_placed = true;
               }else{
                    break;
               }

               move_position = parser.find_handicap_moves(black_player);
          }

          parser.reset_handicap_index();

          return handicap_stone_placed;
     }

     bool check_dir_exists(string path, string dir_name)
     {
          struct stat statbuf;
          if (stat((path + "/" + dir_name).c_str(), &statbuf) != -1) {
               if (S_ISDIR(statbuf.st_mode)) {
                    return true;
               }
          }
          return false;
     }

     void create_dir(string path, string dir_name)
     {
          mkdir((path + "/" + dir_name).c_str(), 0777);
     }

     /*
          Create image based on parametred board position and return path to created image.
     */
     string create_dataset_image(int iterator, string original_file_name, vector< vector< vector<int> > > game_position)
     {
          if(actual_picture_number_in_file == 0){
               if(!check_dir_exists(DATASET_FOLDER_PATH + "/" + DATASET_FOLDER_NAME, to_string(actual_dataset_file_index))){
                    create_dir(DATASET_FOLDER_PATH + "/" + DATASET_FOLDER_NAME, to_string(actual_dataset_file_index));
               }
          }

          string result = DATASET_FOLDER_NAME + "/" + to_string(actual_dataset_file_index) + "/" + dataset_image_creator.save_dataset_image(DATASET_FOLDER_PATH
               + "/" + DATASET_FOLDER_NAME + "/" + to_string(actual_dataset_file_index),
               original_file_name + "_" + DATASET_FILE_NAME_TEMPLATE + "_" + to_string(iterator), SIZE_OF_BOARD, game_position);

          actual_picture_number_in_file++;
          if(actual_picture_number_in_file == max_picture_number_in_folder){
               actual_picture_number_in_file = 0;
               actual_dataset_file_index++;
          }

          return result;
     }

     /*
          Parse dataset file name from path to file.
     */
     string parse_file_name(string file_path)
     {
          size_t file_name = file_path.find_last_of("/");
          return file_path.substr (file_name + 1, file_path.length() - file_name - DATASET_FORMAT_EXTENSION.length() - 1);
     }

     /*
          Reset existing dataset file. Thus prepare dataset file for new dataset.
     */
     void reset_dataset_txt_file()
     {
          ofstream overwrite_file;
          overwrite_file.open(DATASET_FOLDER_PATH + "/" + DATASET_TXT_FILE_NAME);
          overwrite_file.close();
     }

     /*
          Check number of moves and their right moves. It should be same, actual
          sgf file is ignored, if it is not same.
     */
     bool check_picture_moves_count()
     {
          if(dataset_picture_paths.size() != right_moves.size()){
               if(dataset_picture_paths.size() - 1 == right_moves.size()){
                    dataset_picture_paths.erase(dataset_picture_paths.end() - 1, dataset_picture_paths.end());
               }else{
                    return false;
               }
          }
          return true;
     }

     /*
          Add records of currently processed sgf file to global dataset list.
     */
     void add_records_to_global_list()
     {
          for(size_t i = 0; i < dataset_picture_paths.size(); i++){
               map_dataset_list[right_moves[i]].push_back(dataset_picture_paths[i] + " " + to_string(right_moves[i]));
          }
     }

     /*
          Collapse records of all right move classes into one vector and shuffle them.
     */
     void collapse_classes_and_shuffle(vector<string> &collapsed_vector)
     {
          for(auto && record : map_dataset_list){
               collapsed_vector.insert(collapsed_vector.end(), record.begin(), record.end());
          }

          random_shuffle(collapsed_vector.begin(), collapsed_vector.end());
     }

     /*
          Add records to final dataset file.
     */
     void append_to_dataset_txt_file(vector<string> &collapsed_vector)
     {
          ofstream dataset_file;

          dataset_file.open(DATASET_FOLDER_PATH + "/" + DATASET_TXT_FILE_NAME, std::ios_base::app);

          for(size_t i = 0; i < collapsed_vector.size(); i++){
               dataset_file << collapsed_vector[i] << '\n';
          }

          dataset_file.close();
     }

     /*
          Copy classes which have not same number of inputs. This way all classes
          have equal number of inputs.
     */
     void dataset_classes_to_same_number(){
          size_t max = 0;
          for(int i = 0; i < go_game::maxClassNumbers(SIZE_OF_BOARD); i++){
               if(max < map_dataset_list[i].size()){
                    max = map_dataset_list[i].size();
               }
          }

          for(int i = 0; i < go_game::maxClassNumbers(SIZE_OF_BOARD); i++){
               vector <string> tmp(map_dataset_list[i]);
               if(tmp.size() != 0){
                    for(size_t j = 0; j < (max/tmp.size()) - 1; j++){
                         map_dataset_list[i].insert(map_dataset_list[i].end(), tmp.begin(), tmp.end());
                    }

                    map_dataset_list[i].insert(map_dataset_list[i].end(), tmp.begin(), tmp.begin() + (max - map_dataset_list[i].size()));
               }
          }
     }

     /*
          Create full path to created dataset image.
     */
     string get_path_to_image(string image_name)
     {
          string output = DATASET_FOLDER_PATH;
          output.append("/").append(image_name);
          return output;
     }

     /*
          Manage all acctions necessary to create dataset- init sgf_parser, init
          game, setting moves, gathering bard positions data, creating dataset
          images and dataset file.
     */
     bool create_dataset(string setPath, int augmentation_8, bool unique, bool normalisation01)
     {
          go_game game(SIZE_OF_BOARD);
          bool succesful_move;
          string move_position;
          vector< vector< vector<int> > > actual_train_position;
          string file_name = parse_file_name(setPath);
          int iterator = 0;
          bool not_unique = false;

          dataset_picture_paths.clear();
          right_moves.clear();
          blacks_move = parser.is_black_first_player();

          place_handicap_stones(true, game);
          place_handicap_stones(false, game);

          if(blacks_move == parser.is_black_winner()){
               if(unique){
                    actual_train_position = game.create_trainset(parser.is_black_winner());
                    if(unique_check.check_image_is_uniqe_in_local_vector(dataset_image_creator.make_openCV_mat_of_image(actual_train_position, SIZE_OF_BOARD))){
                         dataset_picture_paths.push_back(create_dataset_image(iterator, file_name, actual_train_position));
                         unique_check.add_image_to_list(get_path_to_image(dataset_picture_paths.back()));
                         iterator++;
                    }else{
                         not_unique = true;
                         not_unique_images++;
                    }
               }else{
                    dataset_picture_paths.push_back(create_dataset_image(iterator, file_name, game.create_trainset(parser.is_black_winner())));
                    iterator++;
               }
          }

          do{
               if(!parser.check_next_move_index(blacks_move)){
                    cout << "Colour mishmash " << setPath << '\n';
                    return false;
               }

               move_position = parser.get_next_move(blacks_move);
               succesful_move = game.move(move_position, blacks_move);

               if(succesful_move){
                    if(blacks_move == parser.is_black_winner()){
                         if(unique){
                              if(not_unique){
                                   not_unique = false;
                              }else{
                                   right_moves.push_back(game.code_number_of_last_move(go_game::ROTATION_DEGREE_0));
                              }
                         }else{
                              right_moves.push_back(game.code_number_of_last_move(go_game::ROTATION_DEGREE_0));
                         }
                    }else{
                         if(unique){
                              actual_train_position = game.create_trainset(parser.is_black_winner());
                              if(unique_check.check_image_is_uniqe_in_local_vector(dataset_image_creator.make_openCV_mat_of_image(actual_train_position, SIZE_OF_BOARD))){
                                   dataset_picture_paths.push_back(create_dataset_image(iterator, file_name, actual_train_position));
                                   unique_check.add_image_to_list(get_path_to_image(dataset_picture_paths.back()));
                                   iterator++;
                              }else{
                                   not_unique = true;
                                   not_unique_images++;
                              }
                         }else{
                              dataset_picture_paths.push_back(create_dataset_image(iterator, file_name, game.create_trainset(parser.is_black_winner())));
                              iterator++;
                         }
                    }

                    blacks_move = !blacks_move;
               }else{
                    cout << "Unsuccessful move "  << setPath << " " << move_position << '\n';
                    return false;
               }
          }while(parser.is_end_of_file());

          if(!check_picture_moves_count()){
               cout << "Dataset pictures number not equal to right moves number. " << setPath << '\n';
               return false;
          }else{
               add_records_to_global_list();
          }

          all_images += iterator;
          return true;
     }

public:

     /*
          Prepare all components to create dataset composed of more sgf records-
          find all record files, call dataset_create method for each of them and
          compute statistics.
     */
     data_preparation(int augmentation_8, bool unique, bool normalisation01)
     :dataset_image_creator(normalisation01)
     {
          set_file_count_limits(augmentation_8);

          find_all_dataset_files_in_dir(INPUT_FOLDER);
          correct_datasets = 0;
          int iterator = 0;

          reset_dataset_txt_file();
          if(!check_dir_exists(DATASET_FOLDER_PATH, DATASET_FOLDER_NAME)){
               create_dir(DATASET_FOLDER_PATH, DATASET_FOLDER_NAME);
          }

          for(int i = 0; i < go_game::maxClassNumbers(SIZE_OF_BOARD); i++){
               vector<string> tmp;
               map_dataset_list.push_back(tmp);
          }

          for(auto &path : found_files){
               iterator++;

               if(iterator % 100000 == 0){
                    cout << iterator << '\n';
               }

               if(parser.open_new_sgf_file(path, SIZE_OF_BOARD)){
                    if(create_dataset(path, augmentation_8, unique, normalisation01)){
                         correct_datasets++;
                    }
               }
          }

          vector<string> allRecords;
          dataset_classes_to_same_number();
          collapse_classes_and_shuffle(allRecords);
          append_to_dataset_txt_file(allRecords);

          int recors_number = allRecords.size();

          if(augmentation_8 != 0){
               string compose_dataset_text_file_path = DATASET_FOLDER_PATH;
               compose_dataset_text_file_path.append("/").append(DATASET_TXT_FILE_NAME);

               if(augmentation_8 == 1){
                    to_rotate_image.rotate_all_images(DATASET_FOLDER_PATH, false, compose_dataset_text_file_path, SIZE_OF_BOARD);
                    recors_number *= 4;
                    all_images *= 4;
               }
               if(augmentation_8 == 2){
                    to_rotate_image.rotate_all_images(DATASET_FOLDER_PATH, true, compose_dataset_text_file_path, SIZE_OF_BOARD);
                    recors_number *= 8;
                    all_images *= 8;
               }
          }

          cout << '\n' << "Datasets checked: " << found_files.size() << ", datasets correct: " << correct_datasets << '\n';
          cout << "Dataset has " << recors_number << " records and " << all_images << " images." << '\n';

          if(unique){
               cout << "Not unique images found: " << not_unique_images << '\n';
          }
     }
};
