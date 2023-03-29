#include <unistd.h>
#include <time.h>
#include "can_driver.h" 

// Register definitions
volatile uint16_t *CTRLREG = (uint16_t*) 0x0000;
volatile uint16_t *DATA_RX = (uint16_t*) 0x0100;
volatile uint16_t *DATA_TX = (uint16_t*) 0x0FF0;

//Global variables
uint8_t send_counter = 0;
bool flag2_state = false;

// Define the message ID
#define RX_MESSAGE_ID 0x200
#define TX_MESSAGE_ID 0x300

//Define the DLC
#define RX_DLC 4
#define TX_DLC 1

// Define the signal locations
#define SEND_COUNTER_POS 0

// Define signal values
// The SEND_COUNTER_MAX is defined as 15 because it is a 4-bit counter
#define SEND_COUNTER_MAX 15

// Define the CTRL register bit masks
#define ACT_RX_IRQ_MASK (1 << 0)
#define ACT_TX_IRQ_MASK (1 << 1)
#define TX_AVL_MASK (1 << 2)
#define RX_AVL_MASK (1 << 3)
#define START_STOP_MASK (1 << 4)


void CAN_Init()
{
    // Set up the CTRL register
    uint16_t ctrlReg = 0;
    ctrlReg |= ACT_RX_IRQ_MASK;
    ctrlReg |= ACT_TX_IRQ_MASK;
    ctrlReg |= TX_AVL_MASK;
    ctrlReg |= RX_AVL_MASK;
    ctrlReg |= START_STOP_MASK;
    *(volatile uint16_t *)CTRLREG = ctrlReg;
}

void CAN_Start()
{
    // Set start/stop bit to start CAN module
    *CTRLREG |= START_STOP_MASK;
}

void CAN_Stop()
{
    // Set start/stop bit to stop CAN module
    *CTRLREG &= ~START_STOP_MASK;
}

void CAN_Configure()
{
    // Set ACT/RX IRQ and ACT/TX IRQ bits
    *CTRLREG |= (ACT_RX_IRQ_MASK | ACT_TX_IRQ_MASK);
}

void CAN_Send()
{
    // The TX AVL bit is set to 1 by the controller when it has finished transmitting a message.
    // The bit is cleared to 0 when the controller is in the process of transmitting a message.
    while (!(*CTRLREG & TX_AVL_MASK)) {
        usleep(100);
    }

    // Update the send_counter signal
    *(DATA_TX + SEND_COUNTER_POS) = send_counter;

    // Set the DLC and ID in the TX buffer
    *(DATA_TX + 8) = TX_DLC;
    *(DATA_TX + 9) = TX_MESSAGE_ID & 0xFF;
    *(DATA_TX + 10) = TX_MESSAGE_ID >> 8;

    // Copy data to the TX buffer
    for (int i = SEND_COUNTER_POS + 1; i < 8; i++) {
        *(DATA_TX + i) = 0x00;
    }

    // Set the TX IRQ bit in CTRL register
    *CTRLREG |= ACT_TX_IRQ_MASK;

    // Call the send message TX interrupt service routine
    IRQ_CAN_SEND_MESSAGE_TX();
}

bool CAN_Receive(uint16_t* id, uint8_t* dataLength, uint8_t data[])
{
    // Check if RX AVL is set by the CAN controller to indicate that a message is available in the receive buffer 
    if (!(*CTRLREG & RX_AVL_MASK)) {
        return false;
    }

    // Get the DLC and ID from the RX buffer
    *dataLength = *(DATA_RX + 8);
    *id = (*(DATA_RX + 10) << 8) | *(DATA_RX + 9);

    // Copy data from the RX buffer
    for (int i = 0; i < *dataLength; i++) {
        data[i] = *(DATA_RX + i);
    }

    //Process message ID
    if(*id == RX_MESSAGE_ID)
    {
        if (((data[3] >> 2) & 0x01) != flag2_state)
        {
            // increments the SEND_COUNTER signal by one every time the FLAG2 signal changes
            send_counter = (send_counter + 1) % SEND_COUNTER_MAX;

            // Update flag2_state
            flag2_state = !flag2_state;
        }
    }

    // Once the message has been read from the DATA_RX register, clear the RX AVL bit in the CTRLREG register
    *CTRLREG &= ~RX_AVL_MASK;

    // Call the interrupt to notify the application that a new message has been received.
    IRQ_MESSAGE_AVAILABLE_RX();
}

void CAN_Send_Periodic()
{
    static uint32_t last_send_time = 0;
    uint32_t current_time = clock();

    if ((current_time - last_send_time)/CLOCKS_PER_SEC >= 0.01)
    {
        CAN_Send();
        last_send_time = current_time;
    }
}
