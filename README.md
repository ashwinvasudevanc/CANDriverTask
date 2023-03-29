# UNO MINDA Software Interview task

### Task:
A simple CAN driver is to be developed as per requirements.

### My Implementation:
Implemented certain functions which satisfies the requirements of the task.
The implemented functions are:
- Initializing the CAN by setting the required bits.
- Function to start CAN driver.
- Function to stop CAN driver.
- Configuring CAN driver by setting ACT/RX IRQ and ACT/TX IRQ bits.
- Send a CAN message with the Message ID: 0x300
- Receive the CAN message from the DATA_RX register. 
- Send a CAN message every 10ms in interrupt mode.


### Available files:
This folder contains the ``can_driver.c`` and ``can_driver.h`` files.<br>
The ``can_driver.c`` contains all the functions that satisifes each requirement specified in the task and the ``can_driver.h`` contains the function decleration.
