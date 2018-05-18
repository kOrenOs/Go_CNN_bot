#include <iostream>
#include <fstream>

using namespace std;

/*
     Class for parsing caffe output testing reports.
*/
class logfile_parser
{
     int log_iterator;
     int parsed_iterator;

     string logfile_template_name = "logfile";
     string parsed_logfile_template_name = "parsed_logfile";
     string logfile_extension = ".txt";
     string logfiles_path = "logfiles/";

     string solver_param_keywords = "Initializing solver from parameters:";
     string model_param_keywords = "Initializing net from parameters:";
     string new_log_line_sign = "I";
     string loss_keywords = "loss =";
     string iteration_ketwords = "Iteration ";
     string learning_rate_keywords = "lr =";
     string testing_iteration_keywords = "Testing net";
     string testing_loss_keyword = "softmax_output_test =";

     string parsed_logfile = "";

     string solver_label = "----------------------------- Solver -----------------------------";
     string model_label = "----------------------------- Model -----------------------------";
     string data_label = "----------------------------- Log data -----------------------------";

     bool data_state = false;

     /*
          Find number of next logfile to avoid save file collision.
     */
     int find_next_logfile_number(string base_logfile_name, int &iterator)
	{
		int iter = 0;
		string tmp;
		while(iter < 1000000){
			tmp = get_logfile_name(iter, base_logfile_name);
	    		if(!check_file_exists(tmp)){
				iterator = iter;
				return iterator;
			}
			iter++;
		}
		cout << "Error: Sorry, too much logfiles." << '\n';
		return -1;
	}

     bool check_file_exists(string file_path)
     {
          ifstream file(file_path.c_str());
          if(!file.good()){
               file.close();
               return false;
          }
          file.close();
          return true;
     }

     /*
          Compose logfile name for next created logfile.
     */
     string get_logfile_name(int logfile_number, string base_logfile_name)
	{
		string output(logfiles_path);
		output += base_logfile_name + to_string(logfile_number) + logfile_extension;
		return output;
	}

     void saveparsed_logfile()
     {
          ofstream parsed_logfile_stream (get_next_parsed_log_name());
          if (parsed_logfile_stream.is_open()){
               parsed_logfile_stream << parsed_logfile;
               parsed_logfile_stream.close();
          }
          iterate_parsed_iterator();
     }

     /*
          Return one next line of processed report.
     */
     string get_block_until_next_log_line(ifstream &reader)
     {
          string output = "";
          string line;

          while ( getline (reader,line) ) {
               if(line.find(new_log_line_sign) != 0){
                    output += line + '\n';
               }else{
                    break;
               }
          }
          return output;
     }

     /*
          Method managing parsing of solver part of report.
     */
     bool extract_solver(string line, ifstream &reader)
     {
          if(line.find(solver_param_keywords) != string::npos){
               change_from_data_state();
               parsed_logfile += solver_label + "\n \n";
               parsed_logfile += get_block_until_next_log_line(reader);
               parsed_logfile += "\n\n";

               return true;
          }
          return false;
     }

     /*
          Method managing parsing of model part of report.
     */
     bool extract_model(string line, ifstream &reader)
     {
          if(line.find(model_param_keywords) != string::npos){
               change_from_data_state();
               parsed_logfile += model_label + "\n \n";
               parsed_logfile += get_block_until_next_log_line(reader);
               parsed_logfile += "\n\n";

               return true;
          }
          return false;
     }

     /*
          Change process state to data gathering.
     */
     void change_to_data_state()
     {
          if(!data_state){
               parsed_logfile += data_label + "\n \n";
               data_state = true;
          }
     }

     /*
          Change process state from data gathering.
     */
     void change_from_data_state()
     {
          if(data_state){
               parsed_logfile += "\n \n \n \n";
               data_state = false;
          }
     }

     /*
          Method managing parsing of iteration and loss data while data state.
     */
     void iteration_and_loss_extraction(string line)
     {
          size_t loss_position = line.find(loss_keywords);
          if(loss_position != string::npos){
               change_to_data_state();

               int iteration_position = line.find(iteration_ketwords);

               parsed_logfile += extract_data(iteration_position, iteration_ketwords.length(), line);
               parsed_logfile += "       ";
               parsed_logfile += extract_data(loss_position, loss_keywords.length(), line);
               parsed_logfile += "       ";
          }
     }

     /*
          Find actual learning rate in processed row of report.
     */
     void get_actual_learning_rate(string line)
     {
          size_t lr_rate_position = line.find(learning_rate_keywords);
          if(lr_rate_position != string::npos){
               parsed_logfile += extract_data(lr_rate_position, learning_rate_keywords.length(), line);
               parsed_logfile += '\n';
          }
     }

     /*
          Gather testing iteration data from report.
     */
     void get_testing_iteration(string line)
     {
          size_t testing_iter = line.find(testing_iteration_keywords);
          if(testing_iter != string::npos){
               change_to_data_state();

               int iter_position = line.find(iteration_ketwords);
               parsed_logfile += '\n';
               parsed_logfile += "Test Iter:  ";
               parsed_logfile += extract_data(iter_position, iteration_ketwords.length(), line);
               parsed_logfile += "       ";
          }
     }

     /*
          Gather testing loss data from report.
     */
     void get_testing_loss(string line)
     {
          size_t testing_loss = line.find(testing_loss_keyword);
          if(testing_loss != string::npos){
               parsed_logfile += extract_data(testing_loss, testing_loss_keyword.length(), line);
               parsed_logfile += "\n \n";
          }
     }

     /*
          Method managing all process of gathering data of learning phase - loss,
          iterations, lr_rate, testing phase data,...
     */
     void learning_process_data(string line)
     {
          iteration_and_loss_extraction(line);
          get_actual_learning_rate(line);
          //get_testing_iteration(line);
          //get_testing_loss(line);
     }

     /*
          Extrecting specific data from line of report.
     */
     string extract_data(int position, int keyword_string_length, string line)
     {
          int endOfDataExtraction = get_min_of_parsing_position(position, line);
          position = position + keyword_string_length;
          string tmp = line.substr(position, endOfDataExtraction - position);
          return tmp;
     }

     /*
          Find boundaries of of parsed data in processed line.
     */
     int get_min_of_parsing_position(int position, string line)
     {
          size_t bracket = line.find("(", position);
          size_t comma = line.find(",", position);

          if(bracket == string::npos && comma == string::npos) return line.length();
          if(bracket == string::npos) return comma;
          if(comma == string::npos) return bracket;
          if(comma < bracket) return comma;
          return bracket;
     }

public:

     /*
          Initialise next numbers of logfiles for create unique logfile name.
     */
     void init_iterators()
     {
          find_next_logfile_number(parsed_logfile_template_name, parsed_iterator);
          find_next_logfile_number(logfile_template_name, log_iterator);
     }

     int get_parsed_iterator()
     {
          return parsed_iterator;
     }

     int get_logfile_iterator()
     {
          return log_iterator;
     }

     void iterate_parsed_iterator()
     {
          parsed_iterator++;
     }

     void iterate_logfile_iterator()
     {
          log_iterator++;
     }

     string get_next_parsed_log_name()
     {
          string tmp = get_logfile_name(parsed_iterator ,parsed_logfile_template_name);
          if(!check_file_exists(tmp)){
               return tmp;
          }else{
               find_next_logfile_number(parsed_logfile_template_name, parsed_iterator);
               return get_logfile_name(parsed_iterator ,parsed_logfile_template_name);
          }
     }

     string get_next_origin_log_name()
     {
          string tmp = get_logfile_name(log_iterator ,logfile_template_name);
          if(!check_file_exists(tmp)){
               return tmp;
          }else{
               find_next_logfile_number(logfile_template_name, log_iterator);
               return get_logfile_name(log_iterator ,logfile_template_name);
          }
     }

     /*
          Main method, manage parsing of caffe report file inserted to parameter.
     */
     void parse_logfile(string logfile_path)
     {
          string line;
          parsed_logfile = "";

          ifstream reader (logfile_path);
          if (reader.is_open()){
               while ( getline (reader,line)){
                    if(extract_solver(line, reader))continue;
                    if(extract_model(line, reader))continue;
                    learning_process_data(line);
               }
               reader.close();
          }

          saveparsed_logfile();
     }
};
