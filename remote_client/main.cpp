// File : 			main.cpp
// Authors: 		Pavan Dhareshwar, Omkar Purandare, Virag Gada, Shreyas Vasanthkumar 
// Description : 	This is a single cpp source file that reads the image file from a 
// 					serial connection, processes the image to count the number of faces
// 					and sends this information to the non-remote freewave device from 
// 					which it is uploaded to cloud via AWS.

// This code is developed for HackCU_RemoteDataProcessing_Zumilink as part of HackCU IV.
// Parts of the code is referenced from official OpenCV website and academic projects of
// collaborators. 


#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>

#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>

#define POLL_RESP_LENGTH	(0x19)


struct termios *configure;
void tty_config(struct termios *con, int descriptor);

char *device = "/dev/ttyO5";
int fd;
uint8_t len = POLL_RESP_LENGTH, c;
FILE *fp_write = NULL;

//Create the classifiers
cv::CascadeClassifier faceCascade;

void tty_config(struct termios *con, int descriptor)
{
	tcgetattr(descriptor, con);
	con->c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON);
	con->c_oflag = 0;
	con->c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);
	con->c_cc[VMIN] = 1;
	con->c_cc[VTIME] = 0;
	if(cfsetispeed(con, B115200) || cfsetospeed(con, B115200))
	{
		perror("ERROR in baud set\n");
	}
	if(tcsetattr(descriptor, TCSAFLUSH, con) < 0)
	{
		perror("ERROR in set attr\n");
	}
}

// Function to poll for data on the serial comm port 
void poll_for_data()
{
	bool check_next_val = false;
	int count = 0;
	while(1)
	{
		if(read(fd, (char *)&c, 1) > 0)
		{
			fwrite(&c, sizeof(char), 1, fp_write);
			//printf("0x%x", c);
			
			// The JPEG file format has 0xFFD9 as an end of image indicator which is
			// checked to break this loop that reads data from the serial comm port.
			if ((char)c == 0xFF )
			{
				check_next_val = true;
				fflush(stdout);
				len--;
				continue;
			}
			if (check_next_val == true && (char)c == 0xD9)
			{
				fflush(stdout);
				len--;
				break;
			}
			else
			{
				check_next_val = false;
			}
			fflush(stdout);
			len--;
		}
	}
}

int countNumFaces()
{
	std::string image_file = "image_out.jpg";
	std::string faceCascadeName = "/usr/share/opencv/haarcascades/haarcascade_frontalface_alt2.xml";

  //Load the classifiers
  if (!faceCascade.load( faceCascadeName ))
  {
    std::cerr << "Could not load face classifier" << std::endl;
    return; 
  }
   
  //The captured frame
  cv::Mat frame;
  frame = cv::imread(image_file);
  if(frame.empty())
  {
    std::cerr << "Could not load image " << "image_out.jpg" << std::endl;
    return -1;
  }

  //This will contain the output of the face detector
  std::vector<cv::Rect> faces;

  //Preprocess the image
  cv::Mat frameGray;
  cv::cvtColor( frame, frameGray, CV_BGR2GRAY );
  cv::equalizeHist( frameGray, frameGray );

  //Detect the face
  //faceCascade.detectMultiScale( frameGray, faces, 1.1, 2, 0, cv::Size(80,80));
  faceCascade.detectMultiScale( frameGray, faces);

  //std::cout << "Number of detected faces: " << faces.size() << std::endl;
  
  if (faces.size() == 0) 
	  return 0;
  else
	  return faces.size();
}

void sendToServer(int num)
{
	const char *server_name = "192.168.111.101";
	
	const int server_port = 8080;

	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));
	
	// open a stream socket
	int sock;
	if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		printf("could not create socket\n");
		return;
	}
	
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(server_port);

	if(inet_pton(AF_INET, server_name, &server_address.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return;
    } 

	if (connect(sock, (struct sockaddr*)&server_address,
	            sizeof(server_address)) < 0) {
		printf("could not connect to server\n");
		return;
	}

	// send
	send(sock, &num, sizeof(int), 0);
}

int main( int argc, char **argv )
{	
	fp_write = fopen("image_out.jpg", "w");
	if (!fp_write)
	{
		printf("File %s open failed\n", "image_out.jpg");
		exit(1);	
	}

	fd = open(device, O_RDWR | O_NONBLOCK);
	if(fd == -1)
	{
		perror("ERROR opening file descriptor\n");
	}

	configure = (struct termios*)malloc(sizeof(struct termios));
	tty_config(configure, fd);
	poll_for_data();
	fclose(fp_write);
	close(fd);

	printf("Image received over serial. Processing the image to count the number of faces\n");
	
	int numFaces = countNumFaces();
	if (-1 != numFaces)
	{
		printf("Number of faces : %d\n", countNumFaces());
	}
	else
	{
		printf("countNumFaces function returned error\n");
	}
	
	printf("Sending data to server\n");
	sendToServer(numFaces);
	
	return 0;
}
