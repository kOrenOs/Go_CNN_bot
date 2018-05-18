#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <sstream>
#include "stdlib.h"

using namespace std;

/*
     Allow to change solver data of nwural network.
*/
class config_changer{

     vector < string > labels_for_find;
     vector < vector < double > > params_for_change;
     string config_file_path;
     bool init_successful = false;

     /*
          Reading configuration file and save settings. It will change solver by
          gathered data.
     */
     bool set_up_new_params(string new_param_file_path)
     {
          cout << "----Reading new config param file." << '\n' << '\n';
          string line;
          ifstream new_config_file (new_param_file_path.c_str());

          if (new_config_file.is_open()){
               getline (new_config_file,line);
               labels_for_find = parse_all_by_delimiter(line, " ");

               init_param_vector(labels_for_find.size());

               while ( getline (new_config_file,line) ){
                    vector < string > tmp = parse_all_by_delimiter(line, " ");
                    add_from_vector_to_groups(tmp);
               }
               new_config_file.close();
          }
          else{
               cout << "Error: File with new configurations not found." << '\n' << '\n';
               return false;
          }

          cout << "----Finished- reading new config param file." << '\n' << '\n';
          return true;
     }

     void init_param_vector(int size)
     {
          for(int i = 0; i < size; i++){
               vector < double > tmp;
               params_for_change.push_back(tmp);
          }
     }

     /*
          Save data to appropriate settings groups.
     */
     void add_from_vector_to_groups(vector < string > parsed_values)
     {
          for(unsigned int i = 0; i < parsed_values.size(); i++){
               params_for_change[i].push_back(atof(parsed_values[i].c_str()));
          }
     }

     /*
          Parse specific settings from configuration file.
     */
     vector < string > parse_all_by_delimiter(string line, string delimiter)
     {
          size_t position = 0;
          vector < string > output;
          while ((position = line.find(delimiter)) != string::npos) {
              output.push_back(line.substr(0, position));
              line.erase(0, position + delimiter.length());
          }

          output.push_back(line);
          return output;
     }

     int check_and_find_label(string line)
     {
          for(unsigned int i = 0; i < labels_for_find.size(); i++){
               size_t position = line.find(labels_for_find[i]);

               if(position !=  string::npos){
                    return i;
               }
          }

          return -1;
     }

     string change_line(int group_index, int config_change_number)
     {
          string output (labels_for_find[group_index]);

          stringstream stream;
          stream << params_for_change[group_index][config_change_number];
          return output + " " + stream.str();
     }

     void save_to_file(string data_to_save)
     {
          ofstream config_file (config_file_path.c_str());
          if (config_file.is_open()){
               config_file << data_to_save;
               config_file.close();
          }
     }

public:
     config_changer(string configuration_file_path, string new_param_file_path)
     {
          config_file_path = configuration_file_path;

          if(!set_up_new_params(new_param_file_path)){
               cout << "Error: New config param file reading error." << '\n';
               init_successful = false;
          }else{
               init_successful = true;
          }
     }

     bool wasinit_successful(){
          return init_successful;
     }

     int number_of_loaded_configs()
     {
          return params_for_change[0].size();
     }

     bool find_and_change_configuration(unsigned int config_change_number)
     {
          if(config_change_number >= params_for_change[0].size()){
               cout << "Error: There is not this config change number. Max change number is: " << (number_of_loaded_configs() - 1) << '\n';
               return false;
          }

          string line;
          string final_text;
          int label;
          ifstream config_file (config_file_path.c_str());

          if (config_file.is_open()){
               while ( getline (config_file,line) ){
                    label = check_and_find_label(line);

                    if(label == -1){
                         final_text = final_text + line + '\n';
                    }else{
                         final_text = final_text + change_line(label, config_change_number) + '\n';
                    }
               }
               config_file.close();

               cout << final_text << '\n';

               save_to_file(final_text);

               return true;
          }
          else{
               cout << "Error: Config file not found." << '\n';
               return false;
          }
     }
};
