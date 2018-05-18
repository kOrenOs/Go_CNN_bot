all:
	g++ -std=c++11 -I$(CAFFE_PATH)/include -I$(CAFFE_PATH)/distribute/include -I/usr/local/include/opencv -I/usr/local/include/opencv2 -L/usr/local/lib/ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"go_game_and_data_preparation/sgf_parser.d" -MT"go_game_and_data_preparation/sgf_parser.d" -o "go_game_and_data_preparation/sgf_parser.o" "go_game_and_data_preparation/sgf_parser.cpp"

	g++ -std=c++11 -I$(CAFFE_PATH)/include -I$(CAFFE_PATH)/distribute/include -I/usr/local/include/opencv -I/usr/local/include/opencv2 -L/usr/local/lib/ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"go_game_and_data_preparation/data_preparation.d" -MT"go_game_and_data_preparation/data_preparation.d" -o "go_game_and_data_preparation/data_preparation.o" "go_game_and_data_preparation/data_preparation.cpp"

	g++ -std=c++11 -I$(CAFFE_PATH)/include -I$(CAFFE_PATH)/distribute/include -I/usr/local/include/opencv -I/usr/local/include/opencv2 -L/usr/local/lib/ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"go_game_and_data_preparation/board.d" -MT"go_game_and_data_preparation/board.d" -o "go_game_and_data_preparation/board.o" "go_game_and_data_preparation/board.cpp"

	g++ -std=c++11 -I$(CAFFE_PATH)/include -I$(CAFFE_PATH)/distribute/include -I/usr/local/include/opencv -I/usr/local/include/opencv2 -L/usr/local/lib/ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"go_game_and_data_preparation/go_game.d" -MT"go_game_and_data_preparation/go_game.d" -o "go_game_and_data_preparation/go_game.o" "go_game_and_data_preparation/go_game.cpp"

	g++ -std=c++11 -I$(CAFFE_PATH)/include -I$(CAFFE_PATH)/distribute/include -I/usr/local/include/opencv -I/usr/local/include/opencv2 -L/usr/local/lib/ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"go_game_and_data_preparation/image_creator.d" -MT"go_game_and_data_preparation/image_creator.d" -o "go_game_and_data_preparation/image_creator.o" "go_game_and_data_preparation/image_creator.cpp"

	g++ -std=c++11 -I$(CAFFE_PATH)/include -I$(CAFFE_PATH)/distribute/include -I/usr/local/include/opencv -I/usr/local/include/opencv2 -L/usr/local/lib/ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"go_game_and_data_preparation/group.d" -MT"go_game_and_data_preparation/group.d" -o "go_game_and_data_preparation/group.o" "go_game_and_data_preparation/group.cpp"

	g++ -std=c++11 -I$(CAFFE_PATH)/include -I$(CAFFE_PATH)/distribute/include -I/usr/local/include/opencv -I/usr/local/include/opencv2 -L/usr/local/lib/ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"go_game_and_data_preparation/rotate_image.d" -MT"go_game_and_data_preparation/rotate_image.d" -o "go_game_and_data_preparation/rotate_image.o" "go_game_and_data_preparation/rotate_image.cpp"

	g++ -std=c++11 -I$(CAFFE_PATH)/include -I$(CAFFE_PATH)/distribute/include -I/usr/local/include/opencv -I/usr/local/include/opencv2 -L/usr/local/lib/ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"go_game_and_data_preparation/data_go_controller.d" -MT"go_game_and_data_preparation/data_go_controller.d" -o "go_game_and_data_preparation/data_go_controller.o" "go_game_and_data_preparation/data_go_controller.cpp"

	g++ -std=c++11 -I$(CAFFE_PATH)/include -I$(CAFFE_PATH)/distribute/include -I/usr/local/include/opencv -I/usr/local/include/opencv2 -L/usr/local/lib/ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"go_game_and_data_preparation/prediction.d" -MT"go_game_and_data_preparation/prediction.d" -o "go_game_and_data_preparation/prediction.o" "go_game_and_data_preparation/prediction.cpp"

	g++ -std=c++11 -I$(CAFFE_PATH)/include -I$(CAFFE_PATH)/distribute/include -I/usr/local/include/opencv -I/usr/local/include/opencv2 -L/usr/local/lib/ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"go_game_and_data_preparation/dataset_image_unique.d" -MT"go_game_and_data_preparation/dataset_image_unique.d" -o "go_game_and_data_preparation/dataset_image_unique.o" "go_game_and_data_preparation/dataset_image_unique.cpp"

	g++ -L$(CAFFE_PATH)/build/lib -o "go_game_and_data_preparation/go_game_and_data_preparation" go_game_and_data_preparation/prediction.o go_game_and_data_preparation/data_go_controller.o go_game_and_data_preparation/dataset_image_unique.o go_game_and_data_preparation/rotate_image.o go_game_and_data_preparation/sgf_parser.o go_game_and_data_preparation/data_preparation.o go_game_and_data_preparation/image_creator.o go_game_and_data_preparation/board.o go_game_and_data_preparation/go_game.o go_game_and_data_preparation/group.o $(CAFFE_PATH)/build/lib/libcaffe.so -lcaffe -lglog -lboost_filesystem -lboost_system -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_contrib -lopencv_legacy -lopencv_stitching

	g++ -std=c++11 -I$(CAFFE_PATH)/include -I$(CAFFE_PATH)/distribute/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"go_game_train/go_train.d" -MT"go_game_train/go_train.d" -o "go_game_train/go_train.o" "go_game_train/go_train.cpp"
	
	g++ -std=c++11 -I$(CAFFE_PATH)/include -I$(CAFFE_PATH)/distribute/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"go_game_train/config_changer.d" -MT"go_game_train/config_changer.d" -o "go_game_train/config_changer.o" "go_game_train/config_changer.cpp"

	g++ -std=c++11 -I$(CAFFE_PATH)/include -I$(CAFFE_PATH)/distribute/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"go_game_train/logfile_parser.d" -MT"go_game_train/logfile_parser.d" -o "go_game_train/logfile_parser.o" "go_game_train/logfile_parser.cpp"

	g++ -L$(CAFFE_PATH)/build/lib -o "go_game_train/Go_game_CNN" go_game_train/go_train.o go_game_train/config_changer.o go_game_train/logfile_parser.o $(CAFFE_PATH)/build/lib/libcaffe.so -lcaffe -lglog -lboost_filesystem -lboost_system

clean:	
	rm -rf  go_game_and_data_preparation/go_game_and_data_preparation go_game_and_data_preparation/prediction.o go_game_and_data_preparation/data_go_controller.o go_game_and_data_preparation/dataset_image_unique.o go_game_and_data_preparation/rotate_image.o go_game_and_data_preparation/sgf_parser.o go_game_and_data_preparation/data_preparation.o go_game_and_data_preparation/image_creator.o go_game_and_data_preparation/board.o go_game_and_data_preparation/go_game.o go_game_and_data_preparation/group.o go_game_and_data_preparation/prediction.d go_game_and_data_preparation/data_go_controller.d go_game_and_data_preparation/dataset_image_unique.d go_game_and_data_preparation/rotate_image.d go_game_and_data_preparation/sgf_parser.d go_game_and_data_preparation/data_preparation.d go_game_and_data_preparation/image_creator.d go_game_and_data_preparation/board.d go_game_and_data_preparation/go_game.d go_game_and_data_preparation/group.d
	
	rm -rf  go_game_train/go_train.o  go_game_train/go_train.d  go_game_train/config_changer.o  go_game_train/config_changer.d go_game_train/logfile_parser.o  go_game_train/logfile_parser.d go_game_train/Go_game_CNN

	
