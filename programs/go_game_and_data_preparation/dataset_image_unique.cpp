#include <iostream>
#include <vector>
#include <dirent.h>
#include <fstream>
#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

/*
     Side class for checking new potential dataset image. Check image with all another
     images in dataset.
*/
class dataset_image_unique
{
     const string DATASET_FORMAT_EXTENSION = ".png";
     vector <int> dataset_images_in_class;
     vector <string> complete_dataset_images;
     vector <int> complete_dataset_labels;

     bool compare_images(Mat pivot, Mat for_comapre)
     {
          return (sum(pivot != for_comapre) == Scalar(0,0,0,0));
     }

     /*
          Initialise vector, which will count images for every output class.
     */
     void init_dataset_images_array(int class_number)
     {
          for(int i = 0; i < class_number; i++){
               dataset_images_in_class.push_back(0);
          }
     }

     /*
          Reset all used data structures.
     */
     void reset_data()
     {
          complete_dataset_images.clear();
          complete_dataset_labels.clear();
          dataset_images_in_class.clear();
     }

     /*
          Write out staticstics of class counts in the end of program run.
     */
     void write_out_statistics()
     {
          for(size_t i = 0; i < dataset_images_in_class.size(); i++){
               cout << "class " << i << " has " << dataset_images_in_class[i] << " images in dataset." << '\n';
          }
     }

     bool copy_file(string SRC, string DEST)
     {
         std::ifstream src(SRC, std::ios::binary);
         std::ofstream dest(DEST, std::ios::binary);
         dest << src.rdbuf();
         return src && dest;
     }

     /*
          Create new image full path.
     */
     string parse_complete_image_path(string actual_name, string complete_dataset_path)
     {
          string output = complete_dataset_path + "/";
          size_t position = actual_name.find_last_of("/\\");
          if(position != string::npos){
               output += actual_name.substr(position + 1);
          }else{
               output += actual_name;
          }

          return output;
     }

     /*
          Save all gathered data to dataset file.
     */
     void save_data_file(string datafile_name)
     {
          ofstream output_file (datafile_name);
          if (output_file.is_open()){
              for(size_t i = 0; i < complete_dataset_images.size(); i++){
                   output_file << complete_dataset_images[i] << " " <<complete_dataset_labels[i] << '\n';
              }
              output_file.close();
          }
     }

public:

     /*
          Find all images with .png extension in parametred directory and its
          subdirectories.
     */
     vector <string> find_all_images_in_dir(string actualPosition)
     {
          vector <string> found_files;
          DIR* inputDir = opendir(actualPosition.c_str());
          dirent* directory;

          if (inputDir){
               while ((directory = readdir(inputDir)) != NULL){
                    string tmp(directory->d_name);
                    if(tmp.find(DATASET_FORMAT_EXTENSION) != string::npos){
                         found_files.push_back(actualPosition + "/" + tmp);
                    }else{
                         if(tmp[0] != '.'){
                              vector <string> tmp_vector = find_all_images_in_dir(actualPosition + "/" + tmp);
                              found_files.insert(found_files.end(), tmp_vector.begin(), tmp_vector.end());
                         }
                    }
               }

               cout << "Images found: " << found_files.size() << '\n';

               closedir(inputDir);
          }

          return found_files;
     }

     /*
          Check image is unique in parametred vector of images.
     */
     bool check_one_unique_image(vector <string> &images, Mat pivot, string is_in_images)
     {
          Mat for_comapre;

          for(size_t i = 0; i < images.size(); i++){
               if(images[i] == is_in_images){
                    continue;
               }

               for_comapre = imread(images[i], CV_LOAD_IMAGE_UNCHANGED);

               if(compare_images(pivot, for_comapre)){
                    return false;
               }
          }

          return true;
     }

     /*
          Check image is unique in parametred vector of images.
     */
     bool check_one_unique_image(vector <string> &images, string image_for_check)
     {
          Mat pivot;
          string is_in_images = "";

          for(string &path : images){
               if(path.compare(image_for_check) == 0){
                    is_in_images = path;
                    break;
               }
          }

          pivot = imread(image_for_check, CV_LOAD_IMAGE_COLOR);

          return check_one_unique_image(images, pivot, is_in_images);
     }

     /*
          Create new dataset which contains only unique images. Input is already
          existing dataset, where is method picking just unique images.
     */
     void make_unique_dataset(string dataset_txt_file, int images_per_class, int number_of_classes, string complete_dataset_path, string dataset_file_name)
     {
          init_dataset_images_array(number_of_classes);
          complete_dataset_images = find_all_images_in_dir(complete_dataset_path);

          string line;
          string new_position;
          ifstream dataset_file (dataset_txt_file);
          if (dataset_file.is_open()){
               while ( getline (dataset_file,line) ){

                    size_t label = line.find(' ');
                    if(label != string::npos) {
                         int class_label = stoi(line.substr(label));
                         string image_path = line.substr(0,label);

                         if(dataset_images_in_class[class_label] < images_per_class){
                              if(check_one_unique_image(complete_dataset_images, image_path)){
                                   new_position = parse_complete_image_path(image_path, complete_dataset_path);

                                   dataset_images_in_class[class_label] = dataset_images_in_class[class_label] + 1;
                                   if(copy_file(image_path, new_position)){
                                        complete_dataset_images.push_back(new_position);
                                        complete_dataset_labels.push_back(class_label);
                                   }else{
                                        cout << image_path << " not unique" << '\n';
                                   }
                              }
                         }
                    }
               }
               save_data_file(dataset_file_name);
               dataset_file.close();

               reset_data();
               write_out_statistics();
          }
     }

     /*
          Add image to local list of unique images.
     */
     void add_image_to_list(string image_path)
     {
          complete_dataset_images.push_back(image_path);
     }

     bool check_image_is_uniqe_in_local_vector(string image_path)
     {
          return check_one_unique_image(complete_dataset_images, image_path);
     }

     bool check_image_is_uniqe_in_local_vector(Mat image_path)
     {
          return check_one_unique_image(complete_dataset_images, image_path, "");
     }
};
