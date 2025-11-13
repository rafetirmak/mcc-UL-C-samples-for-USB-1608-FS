/*DaqDevDiscovery01.C****************************************************************

File:                         DaqDevDiscovery01.C

Library Calls Demonstrated:   cbGetDaqDeviceInventory()
							  cbCreateDaqDevice()
							  cbReleaseDaqDevice()
							  
Purpose:                      Discovers DAQ devices and assigns 
							  board number to the detected devices
							  
Demonstration:                Displays the detected DAQ devices
							  and flashes the LED of the selected device
                              
Other Library Calls:          cbIgnoreInstaCal()
							  cbErrHandling()


Copyright (c) 1993-2020, Measurement Computing Corp.
All Rights Reserved.
***************************************************************************/

/* Include files */
#include "..\cbw.h"
#include "Utilities.h"

#define MAXNUMDEVS 20

 int main(void)
{
	/* Variable Declarations */
    int		BoardNum = 0;
    int		ULStat = NOERRORS;
	float   RevLevel = (float)CURRENTREVNUM;
	int		dummy;
	int		numberOfDevices = MAXNUMDEVS;

	DaqDeviceDescriptor inventory[MAXNUMDEVS];
	DaqDeviceDescriptor DeviceDescriptor;

	/* Declare UL Revision Level */
	ULStat = cbDeclareRevision(&RevLevel);

	InitUL();	// Set up error handling

	/* Ignore InstaCal device discovery */
	cbIgnoreInstaCal();

	 /* set up the screen */
    ClearScreen();
    printf("Demonstration of cbGetDaqDeviceInventory() and cbCreateDaqDevice()\n\n");

	printf("Press ENTER to Discover DAQ devices\n\n");
	dummy = _getch();

	/* Discover DAQ devices with cbGetDaqDeviceInventory()
	Parameters:
            InterfaceType   :interface type of DAQ devices to be discovered
            inventory[]		:array for the discovered DAQ devices
            numberOfDevices	:number of DAQ devices discovered */

	ULStat = cbGetDaqDeviceInventory(ANY_IFC, inventory, &numberOfDevices);

	if(numberOfDevices > 0) {
		printf("Discovered %d DAQ device(s).\n", numberOfDevices);

		for(BoardNum = 0; BoardNum < numberOfDevices; BoardNum++) {
			DeviceDescriptor = inventory[BoardNum];

			printf("\nDevice Name: %s\n", DeviceDescriptor.ProductName);
			printf("Device Identifier: %s\n", DeviceDescriptor.UniqueID);
			printf("Assigned Board Number: %d\n\n", BoardNum);

			/* Creates a device object within the Universal Library and 
			   assign a board number to the specified DAQ device with cbCreateDaqDevice()

			Parameters:
				BoardNum			: board number to be assigned to the specified DAQ device
				DeviceDescriptor	: device descriptor of the DAQ device */

			ULStat = cbCreateDaqDevice(BoardNum, DeviceDescriptor);
		}

		/* Flash the LED of the selected device */

		printf("Select a DAQ device from the discovered devices above to flash the LED\n\n");
		while (1) {
			// select a DAQ from the discovered devices above
			printf("Enter a board number or (-1) to exit: ");
			dummy = scanf("%i", &BoardNum);

			if(BoardNum >=0 && BoardNum < numberOfDevices) {
				/* Flash the LED of the specified DAQ device with cbFlashLED()

				Parameters:
				BoardNum			: board number assigned to the DAQ  */

				ULStat = cbFlashLED(BoardNum);
			} else {
				if(BoardNum == -1)
					break;
				else
					printf("Invalid device number\n");
			}
        }

		for(BoardNum = 0; BoardNum < numberOfDevices; BoardNum++)
		{
			/* Release resources associated with the specified board number within the Universal Library with cbReleaseDaqDevice()
			Parameters:
				BoardNum			: board number assigned to the DAQ  */

			ULStat = cbReleaseDaqDevice(BoardNum);
		}
	}
	return 0;
}

