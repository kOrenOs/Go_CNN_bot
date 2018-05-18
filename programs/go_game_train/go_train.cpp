#include <caffe/caffe.hpp>
#include "caffe/util/io.hpp"
#include "caffe/blob.hpp"
#include "config_changer.cpp"
#include "logfile_parser.cpp"
#include <memory>
#include <vector>
#include <string>
#include <cmath>

using namespace std;

/*
	Main class for training and testing neural networks
*/
class go_train
{
	logfile_parser parser;
	int dimension = 19;
	vector <double> distances;

	void run_training_CNN(string solver_path)
	{
		string actual_logfile_name = parser.get_next_origin_log_name();
		freopen (actual_logfile_name.c_str(),"w",stderr);

		caffe::Caffe::set_mode(caffe::Caffe::GPU);
		caffe::SolverParameter solver_param;
		caffe::ReadSolverParamsFromTextFileOrDie(solver_path, &solver_param);
		boost::shared_ptr<caffe::Solver<float> > solver(caffe::SolverRegistry<float>::CreateSolver(solver_param));
		solver->Solve();

		fclose (stdout);
		parser.iterate_logfile_iterator();

		parser.parse_logfile(actual_logfile_name);
	}

	/*
		Reset distances while computing distance metrics.
	*/
	void reset_distances()
	{
		distances.clear();
	}

	/*
		Get distance metrics for actual computed inputs.
	*/
	double get_distances_average_value()
	{
		double sum;
		for(int i = 0; i < (int)distances.size(); i++){
			sum += distances[i];
		}
		return sum/distances.size();
	}

	/*
		Compute distance metrics for single input.
	*/
	double compute_distance(int first_board_position, int second_board_bosition)
	{
		double output;

		vector<int> tmp = get_real_board_position(first_board_position);
		int x = tmp[0];
		int y = tmp[1];

		tmp = get_real_board_position(second_board_bosition);
		x = x - tmp[0];
		y = y - tmp[1];

		output = sqrt(pow(x, 2) + pow(y, 2));
		distances.push_back(output);

		return output;
	}

	/*
		Get real go game board position from code number of position.
	*/
	vector<int> get_real_board_position(int board_position)
	{
		vector <int> output;
		int y;
		int x;
		if(dimension * dimension + 1 == board_position){
			y = dimension+1;
			x = dimension+1;
		}else{
			y = ((board_position - 1) / dimension) + 1;
			x = ((board_position - 1) % dimension) + 1;

		}
	     output.push_back(x);
		output.push_back(y);

		return output;
	}

public:
	go_train()
	{
		parser.init_iterators();
	}

	void train_CNN(string solver_path, string config_change_file)
	{
		if(config_change_file.compare("") == 0){
			run_training_CNN(solver_path);
		}else{
			config_changer changer(solver_path, config_change_file);
			if(!changer.wasinit_successful()){
				return;
			}

			int size = changer.number_of_loaded_configs();

			run_training_CNN(solver_path);

			for(int i = 0; i < size; i++){
				cout << "Configuration changed to record " << i << ", from configuration change file " << config_change_file << "." << '\n';
				changer.find_and_change_configuration(i);
				run_training_CNN(solver_path);
			}
		}
	}

	/*
		Resume network training after process is aborted.
	*/
	void resume_training_CNN(string solver_path, string solverstats)
	{
		string actual_logfile_name = parser.get_next_origin_log_name();
		freopen (actual_logfile_name.c_str(),"w",stderr);

		const char * solverstats_char = solverstats.c_str();

		caffe::Caffe::set_mode(caffe::Caffe::GPU);
		caffe::SolverParameter solver_param;
		caffe::ReadSolverParamsFromTextFileOrDie(solver_path, &solver_param);
		boost::shared_ptr<caffe::Solver<float> > solver(caffe::SolverRegistry<float>::CreateSolver(solver_param));
		solver.get()->Restore(solverstats_char);
		solver->Solve();

		fclose (stdout);
		parser.iterate_logfile_iterator();

		parser.parse_logfile(actual_logfile_name);
	}

	/*
		Test trained network on test dataset. Output is percentage of successfully
		predicted moves.
	*/
	void test_CNN(string test_model, string caffe_model_snapshot, int number_of_inputs, int batch_size)
	{
		string actual_logfile_name = parser.get_next_origin_log_name();
		freopen (actual_logfile_name.c_str(),"w",stderr);

		reset_distances();

		caffe::Caffe::set_mode(caffe::Caffe::GPU);
		boost::shared_ptr<caffe::Net<float> > net_;
		net_.reset(new caffe::Net<float>(test_model, caffe::Phase::TEST));
		net_.get()->CopyTrainedLayersFrom(caffe_model_snapshot);

		float loss = 0.0;

		int positive = 0;
		int all = 0;

		for(int i = 0; i < ceil(1.0 * number_of_inputs / batch_size); i++){
			vector<caffe::Blob<float>*> results = net_.get()->Forward(&loss);

			const boost::shared_ptr<caffe::Blob<float> >& labels = net_.get()->blob_by_name("label");
			const float* labels_data = labels->cpu_data();

			const boost::shared_ptr<caffe::Blob<float> >& class_predict = net_.get()->blob_by_name("output_class");
			const float* class_data = class_predict->cpu_data();

			const boost::shared_ptr<caffe::Blob<float> >& accuracy = net_.get()->blob_by_name("test_accuracy");

			for (int i = 0; i < class_predict->num(); i++)
			{
				if(labels_data[i] == class_data[i]){
					positive++;
				}
				all++;

				compute_distance(labels_data[i], class_data[i]);
			}

			cout << "Iteration: " << i << ", " << positive*1.0/all << '\n';
		}

		cout << "Final probability: " << positive*1.0/all <<'\n';

		fclose (stdout);
		parser.iterate_logfile_iterator();
	}

	/*
		Method for gathering input parameters and run test process with specific
		inputs.
	*/
	void run_test()
	{
		string line;
		string snapshot, model;
		int batch_size = -1;
		int dataset_inputs_number = -1;
		cout << "Train settings:" << '\n' << '\n';

		cout << "Path to trained network snapshot (.caffemodel):" << '\n';
          std::getline (std::cin,line);
          snapshot = line;

		cout << "Path to trained network model (.prototxt):" << '\n';
          std::getline (std::cin,line);
          model = line;

		do{
               cout << "Set batch size of testing model:" << '\n';
               std::getline (std::cin,line);
			try{
	              batch_size = stoi(line);
	          }catch(const std::invalid_argument& ia){
	               cout << "Not allowed choice." << '\n';
               }
          }while(batch_size == -1);

		do{
               cout << "Set number of testing dataset inputs:" << '\n';
               std::getline (std::cin,line);
			try{
	              dataset_inputs_number = stoi(line);
	          }catch(const std::invalid_argument& ia){
	               cout << "Not allowed choice." << '\n';
               }
          }while(dataset_inputs_number == -1);

		cout << "Reports of errors is in folder \"logfiles\"." << '\n';

		test_CNN(model, snapshot, dataset_inputs_number, batch_size);
	}

	/*
		Method for gathering input parameters and run train process with specific
		inputs.
	*/
	void run_train()
	{
		string solver, line;
		cout << "Test settings:" << '\n' << '\n';

		cout << "Path to Caffe network solver (.prototxt):" << '\n';
		std::getline (std::cin,line);
		solver = line;

		cout << "Training can take very long time. Report of training process and errors is in folder \"logfiles\"." << '\n';

		run_training_CNN(solver);
	}
};

int main(int argc, char *argv[])
{
	if(argc != 2) {
	    cout << "Not appropriate number of parameters!!" << '\n';

	    return 0;
	}

	go_train controller;
	if(strcmp(argv[1],"train") == 0){
		controller.run_train();
	}else{
		if(strcmp(argv[1],"test") == 0){
			controller.run_test();
		}else{
			cout << "Not appropriate parameter" << '\n';
		}
	}

	return 0;
}
