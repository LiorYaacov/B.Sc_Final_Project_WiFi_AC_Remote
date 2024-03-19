# B.Sc_Final_Project_WiFi_AC_Remote

## Abstract

The main idea behind this project is to make our home smarter. That means that every element we have can be connected to an app and can be controlled from distance using the internet. This project starts with the AC, because it contains sending an IR codes and using database and much more complex than just turn switch on or off, but the idea is to continue and make all elements at home able to connect to the internet and therefore be controlled with a smartphone app from anywhere around the world.

## Introduction

The way we communicate with the AC is using IR codes. We’re going to use an IR LED and a special ability of the Arduino’s I/O pins to draw current through the LED for a very specific segments of time. It’s called PWM, which stands for Pulse Width Modulation. This code will be received in the AC receiver and command the AC to switch its mode.
The transmitter circuit will be operated by a micro controller called ESP that also able to connect to WiFi. The ESP will sample the Firebase database every 500 milliseconds to see if a new order from the user has received.
The user will operate the system from an app on a smartphone. Using the app, the user can create as much AC’s profile as needed and record codes for them. When the user sends an order to the AC, the app connects to Firebase and change a set of values. Then, the micro-controller, at a sample time, will see the current order and operate the circuit to execute the user’s command.
![image](https://github.com/LiorYaacov/B.Sc_Final_Project_WiFi_AC_Remote/assets/70516072/caefb5b9-e9b9-4aa9-9ef0-065e6a4d1896)


## The System

The WiFi AC system contains 4 sub-systems:
WiFi unit – based on ESP8266
Transmitter unit – based on Arduino Nano 
Smartphone app – based on MIT App Inventor 2
Storage database – running on Google’s Firebase

All those system are operating together to receive an order from the user and send the requested code to the AC as fast as possible while maintaining reliability.

![image](https://github.com/LiorYaacov/B.Sc_Final_Project_WiFi_AC_Remote/assets/70516072/c6687938-0a5b-45e2-9a69-17b83981e649)


## Results

- The system was tested on Tadiran AC. The results are:
- The command was sent from the smartphone app to Firebase DB
- The NodeMCU get the command and sent a request to Firebase
- Firebase sent the code to the NodeMCU microcontroller
- The NodeMCU delivered the code in parts to the Arduino Nano
- The Arduino Nano sent the code to the AC using the IR LED
 
![image](https://github.com/LiorYaacov/B.Sc_Final_Project_WiFi_AC_Remote/assets/70516072/388c9549-4836-4624-9deb-3d1120f4521c)

![image](https://github.com/LiorYaacov/B.Sc_Final_Project_WiFi_AC_Remote/assets/70516072/520ec827-c8c9-458f-9200-859ce0e58365)

![image](https://github.com/LiorYaacov/B.Sc_Final_Project_WiFi_AC_Remote/assets/70516072/3b938212-6b91-4d1f-903c-1bb0cb05c704)


## Summary and Conclusions 
 
In our days, everything can be smart and exchange data with other devices. IoT make it simple and cheap and opening a new world of automation and control
This project is just the beginning of a whole automation of homes, cars, cities and everything else
![image](https://github.com/LiorYaacov/B.Sc_Final_Project_WiFi_AC_Remote/assets/70516072/33c425c5-5a45-4db7-97be-7dfee0122064)
