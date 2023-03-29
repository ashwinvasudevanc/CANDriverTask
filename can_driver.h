#ifndef CAN_DRIVER_H
#define CAN_DRIVER_H

#include <stdint.h>
#include <stdbool.h>


// Function prototypes
// Function initializes the CAN by setting the required bits
void CAN_Init ();

// Function to start CAN driver
void CAN_Start ();

// Function to stop CAN driver
void CAN_Stop ();

// Function to configure CAN driver by setting ACT/RX IRQ and ACT/TX IRQ bits
void CAN_Configure ();

// Function to send a CAN message
void CAN_Send ();

// Function to receive CAN message from the DATA_RX register. */
bool CAN_Receive(uint16_t* id, uint8_t* dataLength, uint8_t data[]);

// Function to send a CAN message every 10ms in interrupt mode
void CAN_Send_Periodic();

#endif /* CAN_DRIVER_H */
