#include <stdio.h>
#include "can_driver.h"

int
main ()
{
  // This main function is a Example usage of the CAN driver
  CAN_Init ();
  
  CAN_Send ();

  uint16_t recv_id;
  uint8_t recv_dlc;
  uint8_t recv_data[8];
  
  if (CAN_Receive (&recv_id, &recv_dlc, recv_data))
    {
      printf ("Received message with ID %d and DLC %d\n", recv_id, recv_dlc);
      printf ("Data: ");
      for (int i = 0; i < recv_dlc; i++)
	{
	  printf ("%02X ", recv_data[i]);
	}
      printf ("\n");
    }
  else
    {
      printf ("No message received.\n");
    }

  return 0;
}
