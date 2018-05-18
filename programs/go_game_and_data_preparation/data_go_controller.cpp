#include "data_preparation.cpp"
#include "prediction.cpp"

using namespace std;

/*
     Class contains main function. Gathering all neseccary input parameters to run appropriate program.
*/
class data_go_controller
{
     /*
          Method with parameter tasks and run of data preparation program.
     */
     void run_data_praparation()
     {
          string line;
          bool unique = false;
          bool normalisation01 = false;
          int augmentation_8 = -1;

          cout << "Data preparation settings:" << '\n' << '\n';
          do{
               cout << "Augmentation of dataset (0 = no augmentation, 1 = 4 multiplying augmentation, 2 = 8 multiplying augmentation):" << '\n';
               std::getline (std::cin,line);
               augmentation_8 = stoi(line);
               if(!(augmentation_8 == 0 || augmentation_8 == 1 || augmentation_8 == 2)){
                    cout << "Not allowed choice." << '\n';
               }
          }while(!(augmentation_8 == 0 || augmentation_8 == 1 || augmentation_8 == 2));

          do{
               cout << "Unique dataset (Y/N):" << '\n';
               std::getline (std::cin,line);
               if(line.compare("Y") == 0){
                    unique = true;
               }else{
                    if(line.compare("N") != 0){
                         cout << "Not allowed choice." << '\n';
                    }
               }
          }while(!(line.compare("Y")==0 || line.compare("N")==0));

          do{
               cout << "Dataset normalised on 0-1 (Y/N):" << '\n';
               std::getline (std::cin,line);
               if(line.compare("Y") == 0){
                    normalisation01 = true;
               }else{
                    if(line.compare("N") != 0){
                         cout << "Not allowed choice." << '\n';
                    }
               }
          }while(!(line.compare("Y")==0 || line.compare("N")==0));

          line = "Creating ";
          if(augmentation_8 == 0){
               line.append("not augmented, ");
          }else{
               if(augmentation_8 == 1){
                    line.append("4 times augmented, ");
               }else{
                    line.append("8 times augmented, ");
               }
          }

          if(unique){
               line.append("unique, ");
          }else{
               line.append("not unique, ");
          }

          if(normalisation01){
               line.append("0 1 normalised dataset. ");
          }else{
               line.append("0 255 normalised dataset. ");
          }

          line.append("Please wait, this proces can take long time. \n");

          cout << line << '\n';

          data_preparation create_dataset(augmentation_8, unique, normalisation01);
     }

     /*
          Method with parameter tasks and run of go prediction program.
     */
     void run_play()
     {
          string line;
          string snapshot;
          string model;
          int board_dimension = 19;

          cout << "Prediction settings:" << '\n' << '\n';

          cout << "Path to trained network snapshot (.caffemodel):" << '\n';
          std::getline (std::cin,line);
          snapshot = line;

          cout << "Path to trained network model (.prototxt):" << '\n';
          std::getline (std::cin,line);
          model = line;

          prediction predict_game(snapshot, model, board_dimension);
     }

public:
     data_go_controller(int choice)
     {
          switch(choice)
          {
               case 1: run_data_praparation();
                    break;
               case 2: run_play();
                    break;
          }
     }
};

int main(int argc, const char * argv[])
{
     if(argc != 2) {
         cout << "Not appropriate number of parameters!!" << '\n';

         return 0;
     }
     if(strcmp(argv[1],"create_dataset") == 0){
          data_go_controller controller(1);
     }else{
          if(strcmp(argv[1],"play") == 0){
               data_go_controller controller(2);
          }else{
               cout << "Not appropriate parameter" << '\n';
          }
     }
}
