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
	Secondary class, rotate and make augmentation of input dataset.
*/
class rotate_image
{
	string DATASET_FORMAT_EXTENSION = ".png";
	string MIRROR_FILE_LABEL = "mirrored";
	vector<string> new_paths;
	vector<int> new_labels;
	int dimension = 19;

	/*
		Rotate image in parameters by 90 degrees
	*/
	void rotate_90(Mat origin, Mat &rotation)
	{
		transpose(origin, rotation);
		flip(rotation, rotation, +1);
	}

	/*
		Rotate image in parameters by 180 degrees
	*/
	void rotate_180(Mat origin, Mat &rotation)
	{
		flip(origin, rotation, -1);
	}

	/*
		Rotate image in parameters by 270 degrees
	*/
	void rotate_270(Mat origin, Mat &rotation)
	{
		transpose(origin, rotation);
		flip(rotation, rotation, 0);
	}

	/*
		Flip image by y axis.
	*/
	void mirror_effect(Mat origin, Mat &rotation)
	{
		flip(origin, rotation, 0);
	}

	/*
		Save image and return path to image.
	*/
	string save_image(Mat image, string path, int rotation, bool mirror_effect)
	{
		string rotated_name = create_name(path, rotation, mirror_effect);
		imwrite(rotated_name, image);

		return rotated_name;
	}

	/*
		Create name for new image.
	*/
	string create_name(string path, int rotation, bool mirror_effect)
	{
		size_t extension = path.find_last_of(".");
		string output = path.substr(0, extension);
		if(mirror_effect){
			output += "_" + MIRROR_FILE_LABEL + "_" + to_string(rotation) + DATASET_FORMAT_EXTENSION;
		}else{
			output += "_" + to_string(rotation) + DATASET_FORMAT_EXTENSION;
		}

		return output;
	}

	/*
		Parse every line of dataset file and create new image's names.
	*/
	vector<string> parse_dataset_file_line(string line)
	{
		vector <string> output;
		size_t label_position = line.find(' ');
		if(label_position != string::npos) {
			output.push_back(line.substr(0,label_position));
			output.push_back(line.substr(label_position));
		}

		return output;
	}

	int computePositionOfMoveInMatrix(int x, int y){
		return y * dimension + x;
	}

	/*
		Rotate right move code number for rotated images.
	*/
	int rotate_label(int label, int rotation, bool mirror_effect)
	{
		if(label == dimension*dimension){
			return dimension*dimension;
		}

		int tmp, x, y;
		if(mirror_effect){
			y = dimension - label / dimension - 1;
		}else{
			y = label / dimension;
		}
		x = label % dimension;

		switch(rotation){
			case 90:
				tmp = x;
				x = dimension - y - 1;
				y = tmp;
				break;
			case 180:
				x = dimension - x - 1;
				y = dimension - y - 1;
			    	break;
			case 270:
				tmp = x;
				x = y;
				y = dimension - tmp -1;
			    	break;
		}

		return computePositionOfMoveInMatrix(x, y);
	}

	/*
		Create vector of all new images.
	*/
	void get_images_to_new_paths(string path, bool full_rotation)
	{
		new_paths.push_back(path);
		new_paths.push_back(create_name(path, 90, false));
		new_paths.push_back(create_name(path, 180, false));
		new_paths.push_back(create_name(path, 270, false));

		if(full_rotation){
			new_paths.push_back(create_name(path, 0, true));
			new_paths.push_back(create_name(path, 90, true));
			new_paths.push_back(create_name(path, 180, true));
			new_paths.push_back(create_name(path, 270, true));
		}
	}

	/*
		Create vector of labels of new images.
	*/
	void get_labels_to_new_path(int label, bool full_rotation)
	{
		new_labels.push_back(label);
		new_labels.push_back(rotate_label(label,90, false));
		new_labels.push_back(rotate_label(label,180, false));
		new_labels.push_back(rotate_label(label,270, false));

		if(full_rotation){
			new_labels.push_back(rotate_label(label,0, true));
			new_labels.push_back(rotate_label(label,90, true));
			new_labels.push_back(rotate_label(label,180, true));
			new_labels.push_back(rotate_label(label,270, true));
		}
	}

	/*
		Read and parse data from old dataset file.
	*/
	void read_and_update_dataset_file(string dataset_file_path, bool full_rotation)
	{
		string line;
		vector<string> parsed_line;
          ifstream dataset_file (dataset_file_path);
          if (dataset_file.is_open()){
               while ( getline (dataset_file,line) ){
				parsed_line = parse_dataset_file_line(line);

				get_images_to_new_paths(parsed_line[0], full_rotation);
				get_labels_to_new_path(stoi(parsed_line[1]), full_rotation);
			}
			dataset_file.close();
		}
	}

	/*
		Save updated data to new dataset file.
	*/
	void save_datafile(string datafile_name)
     {
          ofstream output_file (datafile_name);
          if (output_file.is_open()){
              for(size_t i = 0; i < new_paths.size(); i++){
                   output_file << new_paths[i] << " " <<new_labels[i] << '\n';
              }
              output_file.close();
          }
     }

	/*
		Find all files with .png extension in directory and subdirectories.
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

               closedir(inputDir);
          }

          return found_files;
     }

public:

	/*
		Manage rotation process of all images in dataset.
	*/
	void rotate_all_images(string path, bool full_rotation, string dataset_txt_for_update, int board_dimension)
	{
		dimension = board_dimension;
		vector<string> image_paths = find_all_images_in_dir(path);

		Mat for_rotation, rotate_90_image, rotate_180_image, rotate_270_image, mirrored_origin;

		for(size_t i = 0; i < image_paths.size(); i++){
			for_rotation = imread(image_paths[i], CV_LOAD_IMAGE_UNCHANGED);

			rotate_90(for_rotation, rotate_90_image);
			rotate_180(for_rotation, rotate_180_image);
			rotate_270(for_rotation, rotate_270_image);

			save_image(rotate_90_image, image_paths[i], 90, false);
			save_image(rotate_180_image, image_paths[i], 180, false);
			save_image(rotate_270_image, image_paths[i], 270, false);

			if(full_rotation){
				mirror_effect(for_rotation, mirrored_origin);
				save_image(mirrored_origin, image_paths[i], 0, true);

				rotate_90(mirrored_origin, rotate_90_image);
				rotate_180(mirrored_origin, rotate_180_image);
				rotate_270(mirrored_origin, rotate_270_image);

				save_image(rotate_90_image, image_paths[i], 90, true);
				save_image(rotate_180_image, image_paths[i], 180, true);
				save_image(rotate_270_image, image_paths[i], 270, true);
			}
		}

		read_and_update_dataset_file(dataset_txt_for_update, full_rotation);
		save_datafile(dataset_txt_for_update);
	}
};
