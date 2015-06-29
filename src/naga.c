#include <libusb-1.0/libusb.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commands.h"

// Device IDs
#define DEV_VID 0x1532		//Razer
#define DEV_PID_Wireless 0x0021	//Naga Epic
#define DEV_PID_Wired 0x001f	//Naga Epic Dock

// USB parameters
#define DEV_INTF 0

// Version
#define VERSION 1

libusb_device_handle * handle;
int verbose = 0;

/* Getting device handle */
int init () {
	libusb_init(NULL);
	libusb_set_debug(NULL, 3);

	// Wireless mode
	handle = libusb_open_device_with_vid_pid(NULL, DEV_VID, DEV_PID_Wireless);
	if (handle == NULL) {
		// Wired mode
		handle = libusb_open_device_with_vid_pid(NULL, DEV_VID, DEV_PID_Wired);
		
		if (handle == NULL) {
			printf("[Error] Naga Epic not found!\n");
			libusb_exit(NULL);
			return 1;
		} else {
			printf("[Info] Naga Epic is connected\n");
		}
	} else {
		printf("[Info] Naga Epic Dock is connected\n");
	}
	
	// Detaching mouse from kernel
	if (libusb_kernel_driver_active(handle,DEV_INTF))
		libusb_detach_kernel_driver(handle, DEV_INTF);
		
	if (libusb_claim_interface(handle,  DEV_INTF) < 0) {
		printf("[Error] IO exception\n");
		libusb_close(handle);
		libusb_exit(NULL);
		return 1;
	}
	
	return 0;
}

void closeHandle () {
	libusb_release_interface( handle, 0 );
	libusb_attach_kernel_driver(handle, DEV_INTF);
	libusb_close(handle);
	libusb_exit(NULL);
}

void printData (unsigned char * data) {
	int k = 0;
	for (int i = 0; i < 90; i++) {
		printf(" %02x", data[i]);
		if (k++ == 7) {
			printf("\n");
			k = 0;
		}
	}
	printf("\n");
}

/* Send data to mouse */
int sendcmd (unsigned char * data) {
	if (verbose) {
		printf("[Debug] Sending data:\n");
		printData(data);
	}

	int res;
	res = libusb_control_transfer(handle,
			LIBUSB_REQUEST_TYPE_CLASS,
			0x9, 0x300, 0, data, 90, 1000);

	if (verbose) {
		printf("[Debug] Transmitted: %d\n", res);
	}
	return res;
}

/* Read data from mouse */
unsigned char * recvcmd() {
	if (verbose) {
		printf("[Debug] Receiving data:\n");
	}

	static unsigned char buffer[90];
	libusb_control_transfer(handle,
			LIBUSB_ENDPOINT_IN |
			LIBUSB_REQUEST_TYPE_CLASS |
			LIBUSB_RECIPIENT_INTERFACE,
			0x1, 0x300, 0,
			buffer, sizeof(buffer), 1000);

	if (verbose) {
		printData(buffer);
	}

	return buffer;
}

/* Parsing console arguments */
int scanArgs (char* arg, char* argv[], int argc) {
	int i = 0;
	for (i = 1; i < argc; i++) {
		if (strcmp(arg, argv[i]) == 0) {
			return i;
		}
	}
	return 0;
}

int main(int argc, char * argv[]) {
	printf("Naga Epic Control v%d\n", VERSION);

	int retry = 0;
	int count = 0;

	while ((retry == 1 && count < 3) || count == 0) {
		count++;

		if (init() != 0) return 1;

		// Verbose mode
		if (scanArgs("-v", argv, argc)) {
			verbose = 1;
		}
		
		// Brightness control
		unsigned char brightness = 255;
		int tmp = scanArgs("-l", argv, argc);
		if (tmp) {
			if (tmp < argc - 1) tmp++;
			// Looking for number after -l
			brightness = atoi(argv[tmp]);
			// If number not found, looking for on or off
			if (strcmp(argv[tmp], "on") == 0) {
				brightness = 255;
			} else if (strcmp(argv[tmp], "off") == 0) {
				brightness = 0;
			}
			if (verbose) printf("[Debug] Brightness: %d\n", brightness);

			// Building command for mouse
			Lights_Brightness[10] = brightness;
			Lights_Brightness[11] = brightness ^ 3;

			sendcmd(Lights_Brightness);

			// Receiving and checking data
			if (recvcmd()[0] != 1) {
				printf("[Error] Command failed!\n");
				retry = 1;
			} else {
				retry = 0;
			}
		}

		closeHandle();
	}
	return 0;
}
