# B.Sc_Final_Project_WiFi_AC_Remote

## Abstract

The main idea behind this project is to make our home smarter. That means that every element we have can be connected to an app and can be controlled from distance using the internet. This project starts with the AC, because it contains sending an IR codes and using database and much more complex than just turn switch on or off, but the idea is to continue and make all elements at home able to connect to the internet and therefore be controlled with a smartphone app from anywhere around the world.

## Introduction

The way we communicate with the AC is using IR codes. We’re going to use an IR LED and a special ability of the Arduino’s I/O pins to draw current through the LED for a very specific segments of time. It’s called PWM, which stands for Pulse Width Modulation. This code will be received in the AC receiver and command the AC to switch its mode.
The transmitter circuit will be operated by a micro controller called ESP that also able to connect to WiFi. The ESP will sample the Firebase database every 500 milliseconds to see if a new order from the user has received.
The user will operate the system from an app on a smartphone. Using the app, the user can create as much AC’s profile as needed and record codes for them. When the user sends an order to the AC, the app connects to Firebase and change a set of values. Then, the micro-controller, at a sample time, will see the current order and operate the circuit to execute the user’s command.
![image](https://github.com/LiorYaacov/B.Sc_Final_Project_WiFi_AC_Remote/assets/70516072/caefb5b9-e9b9-4aa9-9ef0-065e6a4d1896)
