#define DAEMON

//#define DEBUG
#define VERBOSE
#ifdef VERBOSE
	#include <stdio.h>
	#include <iostream>
#endif
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <libusb-1.0/libusb.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>


using namespace std;

uint16_t VENDOR = 0x1d57; /* 0451 */
uint16_t PRODUCT = 0xac01; /* 3211 */


const unsigned char ENDPOINT_DOWN = 0x1; /** from pc*/
const unsigned char ENDPOINT_UP = 0x81;	/** to pc */

#define UNIX_ERR(a) {fprintf(stderr,a);fflush(stderr);}

	//pointer to pointer of device
static libusb_device **devs; 
		//a device handle
static libusb_device_handle *dev_handle; 
	//a libusb session
static libusb_context *ctx = NULL;

int send_key(unsigned int key){
	Display *display;
	unsigned int keycode;
	display = XOpenDisplay(NULL);
	keycode = XKeysymToKeycode(display, key);
	XTestFakeKeyEvent(display, keycode, True, 0);
	XTestFakeKeyEvent(display, keycode, False, 0);
	XFlush(display);
	#ifdef VERBOSE
		cout<<"Event sent"<<endl;
	#endif
}

int work(){
		//for return values 
	int r; 
		//holding number of devices in list
	ssize_t cnt; 
		//initialize the library for the session we just declared
	r = libusb_init(&ctx); 
	if(r < 0) {
		
		#ifdef VERBOSE
			cout<<"Init Error "<<r<<endl; //there was an error
		#endif
		
		return 1;
	}
	libusb_set_debug(ctx, 3); //set verbosity level to 3, as suggested in the documentation

	cnt = libusb_get_device_list(ctx, &devs); //get the list of devices
	if(cnt < 0) {
		
		#ifdef VERBOSE
			cout<<"Get Device Error"<<endl; //there was an error
		#endif
		
		return 1;
	}
	#ifdef VERBOSE
		cout<<cnt<<" Devices in list."<<endl;
	#endif

	dev_handle = libusb_open_device_with_vid_pid(ctx, VENDOR, PRODUCT); //these are vendorID and productID I found for my usb device
	if(dev_handle == NULL){
		#ifdef VERBOSE
			cout<<"Cannot open device"<<endl;
		#endif
		return 1;
	}
	else{
		#ifdef VERBOSE
			cout<<"Device Opened"<<endl;
		#endif
	}
	libusb_free_device_list(devs, 1); 
		//free the list, unref the devices in it

/** ----------------------------------    */
	unsigned char *data = new unsigned char[10]; 

	int actual; 
		//used to find out how many bytes were written
	if(libusb_kernel_driver_active(dev_handle, 0) == 1) { //find out if kernel driver is attached
		
		#ifdef VERBOSE
			cout<<"Kernel Driver Active"<<endl;
		#endif
		
		if(libusb_detach_kernel_driver(dev_handle, 0) == 0) //detach it
		{	
			#ifdef VERBOSE	
				cout<<"Kernel Driver Detached!"<<endl;
			#endif
		}
	}
	r = libusb_claim_interface(dev_handle, 0); //claim interface 0 (the first) of device (mine had jsut 1)
	if(r < 0) {
		#ifdef VERBOSE
			cout<<"Cannot Claim Interface"<<endl;
		#endif
		return 1;
	}
	#ifdef VERBOSE
		cout<<"Claimed Interface"<<endl;
	#endif

/** ----------------------------------    */

	//cout<<"Data->"<<data<<"<-"<<endl; 
	do{
			memset(data,0,sizeof(data));
			r = libusb_bulk_transfer(dev_handle, ENDPOINT_UP, data, 8, &actual, 0); 
			
			#ifdef VERBOSE
				printf("%d\n",actual);
				for (int i=0;i<actual;++i){
					printf("0x%4x " ,data[i]);
					if (i%8==7)printf("\n");
				}
				printf("%d\n",actual);
				cout<<hex<<(int)data[2]<<endl;
			#endif
			
			switch(data[2]){
				case 0x4eu:
					send_key(XK_N);break;
				case 0x4bu:
					send_key(XK_P);break;
				case 0x05u:
					send_key(XK_B);break;
				case 0x3eu:
					send_key(XK_F5);break;
			}	
			
		#ifdef DEBUG
			fflush(stdout);
		#endif
		
		}while (r!=LIBUSB_ERROR_NO_DEVICE);

			r = libusb_release_interface(dev_handle, 0); //release the claimed interface
			if(r!=0) {
				#ifdef VERBOSE
					cout<<"Cannot Release Interface"<<endl;
				#endif
				return 1;
			}
	
	#ifdef VERBOSE
		cout<<"Released Interface"<<endl;
	#endif
	
	libusb_close(dev_handle); //close the device we opened
	libusb_exit(ctx); //needs to be called to end the

	delete[] data; //delete the allocated memory for data
	return 0;
}
void wrap_work(int x){
	if (work()){
		UNIX_ERR("error occur\n");
	}
}
int main(){
	
	#ifdef DEBUG
		freopen("log.txt","w",stdout);
	#endif
	
	#ifdef DAEMON
		if ( signal(SIGUSR1,wrap_work) == SIG_ERR ){
			UNIX_ERR("cannot install signal handler\n");
		}
		while (1){
			pause();
		}
		exit(0);
	#endif
	
	/** if not compiled to run as daemon*/
	#ifndef DAEMON
		work();
	#endif
	
	#ifdef DEBUG
		fclose(stdout);
	#endif
	
}
