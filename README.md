# Caffe framework installation

Caffe framework was used for neural network managing - it is required software for program usage. All programs were developed on Linux system with C++ version 11. Thus, I used the following Caffe installation guides: [Standard instalation](http://caffe.berkeleyvision.org/installation.html) and [Ubuntu instalation](https://github.com/BVLC/caffe/wiki/Ubuntu-16.04-or-15.10-Installation-Guide)

# Program manual

Both programs mentioned in the thesis use Caffe framework. For programs compilation is necessary to have installed Caffe framework and all required software (OpenCV, etc.) properly. Moreover, programs need system variable CAFFE_PATH pointing to the Caffe folder (example: export CAFFE_PATH=/home/user/caffe).

In the "sgf_records" folder are compressed sgf records. It contains all sgf records gathered for this thesis. In the "trained_models" folder are all bigger trained networks. In the "dataset_test" subfolder are trained networks, which were used to test of dataset creating process. It checked if network can use created datasets at all. In the "tested" subfolder are all trained networks, which were evaluated in the thesis. In the "others" subfolder are all other networks, trained for the next Go game move suggestion.

In the "datasets" folder are compressed dataset files, used for networks training. For dataset usage is necessary to extract these files in the "datasets" folder. Extracted datasets have size of several GB.

Both programs are located in the "programs" folder. They are controlled by command line. Programs can be compiled by makefile attached in the "programs" folder. Command "make all" compiles programs and command "make clean" removes all created compile files.

In the "programs" folder are bash scripts for desired program function run. Scripts are named by program run purpose (example: create_dataset.sh). Programs require input data: paths to specific folders, datasets, etc. All scripts should be started from the terminal located to the “programs” folder.

# Prepared scripts

## Dataset creating

Script named "create_dataset.sh". It processes all sgf records from the “execute_sgf” subfolder and save created dataset to the “new_dataset" subfolder. The program cleans “new_dataset" subfolder at the beginning of the program run. It is necessary to copy sgf records from the “sgf_records” subfolder to the “execute_sgf” subfolder. Only these records are processed to dataset. Dataset consists of image inputs and dataset text file. Dataset text file contains paths to dataset images and right, expected move for each of them. Script requires basic dataset information: augmentation level, unique of dataset images, normalization of dataset to 0 and 1 or to 0 and 255.

## Network training

Script named "train_cnn.sh". Script requires path to Caffe network solver configuration file, which contains path to network model. These files have extension “.prototxt” and are described on the official Caffe websites ([solver file](https://github.com/BVLC/caffe/wiki/Solver-Prototxt) and example of [model file](https://github.com/BVLC/caffe/blob/master/models/bvlc_reference_caffenet/train_val.prototxt)). In the Caffe model is necessary to set right path to the training and testing dataset, otherwise program ends with run error. In the subfolder “logfiles” are saved logfiles from the training process and in the subfolder "snapshots" are saved snapshots of trained networks.

## Network testing

Script named “test_cnn.sh”. Script requires path to trained network snapshot (.caffemodel), path to trained network model (.prototxt), network batch size of test iteration and number of image inputs in the test dataset. Model should contain input layer with TEST phase and with path to test dataset properly defined. Program output is percentage accuracy of trained network - how many input images were solved with right, expected output.

## Go game with prediction

Script named “go_with_prediction.sh”. Script requires path to trained network snapshot (.caffemodel) and path to trained network model (.prototxt). Program allows playing Go game match on the board displayed in the console, where '1' is white stone and '5' is black stone. Player's next move is played by coordinates written to the console. When is written invalid input value (only enter button or not correct position format), move suggested by neural network is displayed.
