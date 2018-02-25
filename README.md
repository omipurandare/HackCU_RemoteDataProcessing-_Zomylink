# HackCU_RemoteDataProcessing_Zumilink #

Team Members- Omkar Purandare, Shreyas Vasanthkumar, Virag Gada, Pavan Dhareshwar 

Description-
```
This project was developed as a part of HackCU 2018. The aim of the project is to demostrate the processing capablity of a remote radio to reduce the load on main server and also reduce the data to be transmitted over RF link. The data is taken in serially over RS232(it is a image in this case) and an algorithm is performed using openCV to find the number of faces. After the data is processed remotely, only important data is transmitted over RF to the server which has a internet connectivity. The server after gathering the data uploads it on to the cloud (AWS) in a formatted way with timestamp.
```
Folder structure-

```
-----HackCU_RemoteDataProcessing_Zumilink
         |-----remote_client 
                    |----- README.txt
                    |----- main.cpp 
         |
         |-----server
                    |----- subscribe_publish_sample.c
                    |----- aws_iot_config.h
                    |----- Makefile
                    |----- readme.md
```


Compiling code-
```
Instructions to compile are inside respective the folders. 
```
