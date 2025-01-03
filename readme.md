## Heartrate Monitor Simulator

Simulating the data flow of a wearable heartrate monitor to something like a fitness tracker app on your computer or phone.

Heartrate data is randomly generated through the HRM.c logic. Data is formatted loosely following the ANT protocol, utilizing a structure of 8 bytes, page number, and heart rate data field. Other fields are reserved for further data implementations.

Each message is processed by the slave(FitnessTracker) and upon successful reception, is returned with an acknowledgement message. This is received and validated by the master(HRM).

Heartrate data is sent periodically (4hz).

## Usage:

To build: run the command "make" in your terminal. Run the ./FitnessTracker exec in a terminal, then run ./HRM in another terminal in order to simulate heartrate data collected using ANT protocol style packets.

Run make clean to clean up build artifacts.