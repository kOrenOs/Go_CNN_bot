#define USE_OPENCV 1
#ifndef go_game
#include "go_game.cpp"
#endif

#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <caffe/caffe.hpp>
#include <caffe/util/io.hpp>
#include <caffe/blob.hpp>
#include <memory>
#include "caffe/layers/memory_data_layer.hpp"
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;

/*
     Class creates new go game and play inserted moves. Connected trained neural
     network can predict next move for player.
*/
class prediction
{
     go_game game;

     boost::shared_ptr<caffe::Net<float>> net_;
     int board_dimension;

     /*
          Control palyer's inputs and make action depends on it.
     */
     void game_controller()
     {
          string input;
          vector<int> position;

          get_player_move_notice();

          while(getline(cin, input)) {
               position = parse_move(input);
               if(position.size() < 2){
                    call_prediction();
               }else{
                    if(make_move(position[0], position[1])){
                         game.print_board();
                         get_player_move_notice();
                    }else{
                         cout << "Move not allowed." << '\n';
                    }
               }

               if(game.is_game_finished()){
                    cout << "Game is over!" << '\n';
                    break;
               }
          }
     }

     /*
          Write notice, which player is on turn.
     */
     void get_player_move_notice()
     {
          if(game.is_blacks_turn()){
               cout << "Black player's turn." << '\n';
          }else{
               cout << "White player's turn." << '\n';
          }
     }

     bool make_move(int x, int y)
     {
          return game.move(x, y);
     }

     /*
          Parse player's input. Trying to find move position.
     */
     vector<int> parse_move(string input)
     {
          vector<int> output;

          int x, y;
          size_t position = input.find(" ");

          if(position == string::npos){
               return output;
          }

          try{
               x = stoi(input.substr(0, position)) - 1;
               y = stoi(input.substr(position)) - 1;
          }catch(const std::invalid_argument& ia){
               return output;
          }
          output.push_back(x);
          output.push_back(y);

          return output;
     }

     /*
          Write out adviced predictioned move by neural network.
     */
     void call_prediction()
     {
          vector<int> prediction_moves = get_next_move(game.get_openCV_compatible_board_array(game.is_blacks_turn()), 360);
          vector<int> position;

          for(size_t i = 0; i < prediction_moves.size(); i++){
               position = game.get_real_position(prediction_moves[i]);

               if(game.check_move_is_legal(position[0], position[1])){
                    cout << "Suggested move is: [" << position[0] + 1 << ", " << position[1] + 1 << "]" << '\n';
                    return;
               }
          }
          cout << "No good legal move suggested." << '\n';
     }

     /*
          Call predictioned move from connected neural network.
     */
     vector <int> get_next_move(vector<unsigned char> board_position, int top_k_numbers)
     {
          vector <int> output;

          Mat matrix(board_dimension, board_dimension, CV_8UC3);
          memcpy(matrix.data, board_position.data(), board_position.size()*sizeof(unsigned char));

          std::vector<cv::Mat> dv = {matrix};
          std::vector<int> dvl = {0};

          float loss = 0.0;

          boost::dynamic_pointer_cast<caffe::MemoryDataLayer<float>>(net_->layers()[0])->AddMatVector(dv, dvl);
          vector<caffe::Blob<float>*> results = net_.get()->Forward(&loss);

		const boost::shared_ptr<caffe::Blob<float> >& class_predict = net_.get()->blob_by_name("output_class");
		const float* classData = class_predict->cpu_data();

          for(int i = 0; i < top_k_numbers; i++){
               output.push_back(classData[i]);
          }

          return output;
     }

public:
     prediction(string trained_data_file, string trained_model, int game_dimension)
     : game(game_dimension)
     {
          net_.reset(new caffe::Net<float>(trained_model, caffe::Phase::TEST));
		net_.get()->CopyTrainedLayersFrom(trained_data_file);

          board_dimension = game_dimension;

          game_controller();
     }
};
