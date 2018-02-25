Description - 
```
File to receive data over BSD sockets from the remote client and publish the data over MQTT to the AWS IoT Framework
```

Setup -
'''
1. Create a thing on the AWS IoT and attach correct policies and certificates
2. Download the AWS IoT Embedded C SDK
3. Download the mbedTLS files into the correct folder - /home/devuser/aws-iot-device-sdk-embedded-C/external_libs/mbedTLS
4. Copy this folder to - aws-iot-device-sdk-embedded-C/samples/linux/
'''

Execution - 
```
run make
```
