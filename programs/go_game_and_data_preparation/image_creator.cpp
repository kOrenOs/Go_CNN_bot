#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;

/*
     Class which create dataset image from serialised board data.
*/
class image_creator
{
     const int MIN_REPRESENTATION = 0;
     const int MAX_REPRESENTATION = 255;
     const int MAX_REPRESENTATION_NORM01 = 1;

     const string FORMAT_EXTENSION = ".png";

     bool normalisation01_required = false;

     /*
          Change values from serialised data into image appropriate values.
     */
     int change_to_representation_value(int original_value)
     {
          if(original_value == 0){
               return MIN_REPRESENTATION;
          }else{
               if(normalisation01_required){
                    return MAX_REPRESENTATION_NORM01;
               }
               return MAX_REPRESENTATION;
          }
     }

public:
     image_creator(bool normalisation01)
     {
          normalisation01_required = normalisation01;
     }

     /*
          Create openCV mat data structure containing serialised board data.
     */
     Mat make_openCV_mat_of_image(vector< vector< vector<int> > > game_position, int board_dimension)
     {
          Mat image(board_dimension, board_dimension, CV_8UC3, Scalar(0, 0, 0));

          for(int i = 0; i < board_dimension; i++){
               for(int j = 0; j < board_dimension; j++){
                    image.at<Vec3b>(i,j)[0] = change_to_representation_value(game_position[0][i][j]);
                    image.at<Vec3b>(i,j)[1] = change_to_representation_value(game_position[1][i][j]);
                    image.at<Vec3b>(i,j)[2] = change_to_representation_value(game_position[2][i][j]);
               }
          }

          return image;
     }

     /*
          Save image based on openCV mat data. Return path to created image.
     */
     string save_dataset_image(string path, string file_name, int board_dimension, vector< vector< vector<int> > > game_position)
     {
          Mat img = make_openCV_mat_of_image(game_position, board_dimension);

          if(path.compare("") != 0){
               imwrite(path+"/"+file_name + FORMAT_EXTENSION, img);
          }else{
               imwrite(file_name + FORMAT_EXTENSION, img);
          }
          return file_name + FORMAT_EXTENSION;
     }
};
