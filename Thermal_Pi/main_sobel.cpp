#include <opencv2/opencv.hpp>
#include <string>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <iostream>
#include <sys/time.h>

#include "spi.h" 

#include "leptonSDKEmb32PUB/LEPTON_SDK.h"
#include "leptonSDKEmb32PUB/LEPTON_SYS.h"
#include "leptonSDKEmb32PUB/LEPTON_Types.h"
#include "leptonSDKEmb32PUB/LEPTON_AGC.h"

#define PACKET_SIZE 164
#define PACKET_SIZE_UINT16 (PACKET_SIZE/2)
#define PACKETS_PER_FRAME 60
#define FRAME_SIZE_UINT16 (PACKET_SIZE_UINT16*PACKETS_PER_FRAME)
#define FPS 27;

LEP_CAMERA_PORT_DESC_T _port;
LEP_SYS_FPA_TEMPERATURE_KELVIN_T fpaTemperatureKelvin; 
LEP_SYS_AUX_TEMPERATURE_KELVIN_T auxTemperatureKelvin;
LEP_SYS_FPA_TEMPERATURE_CELCIUS_T fpaTemperatureCelcius;
LEP_SYS_AUX_TEMPERATURE_CELCIUS_T auxTemperatureCelcius;
LEP_SYS_UPTIME_NUMBER_T sysCameraUpTime;

using namespace cv;
using namespace std;

uint8_t result[PACKET_SIZE*PACKETS_PER_FRAME];
uint16_t *frameBuffer;

Mat thermal_image = Mat(Size(80, 60), CV_8UC1);
Mat pi_image, pi_image_gray;
Mat thermal_image_r;

static void Sobel(Mat , const char*);


int main( int argc, char **argv )
{
	VideoCapture cap(0);
	
	if(!cap.isOpened())
	{
		cout << "PiCamera don't opened" << endl;
	}
	
	cout << "PRESS 'ESC' TO EXIT" << endl;
	
	Spi_Open_port();
	
	while(true)
	{	
		//read data packets from lepton over SPI
		int resets = 0;
		for(int j = 0; j < PACKETS_PER_FRAME; j++)
		{
			//if it's a drop packet, reset j to 0, set to -1 so he'll be at 0 again loop
			read(spi_cs0_fd, result + sizeof(uint8_t) * PACKET_SIZE * j, sizeof(uint8_t) * PACKET_SIZE);
			int packetNumber = result[j * PACKET_SIZE + 1];
			if(packetNumber != j)
			{
				j = -1;
				resets += 1;
				usleep(1000);

				//Note: we've selected 750 resets as an arbitrary limit, since there should never be 750 "null" packets between two valid transmissions at the current poll rate
				//By polling faster, developers may easily exceed this count, and the down period between frames may then be flagged as a loss of sync
				if(resets == 750)
				{
					Spi_Close_port();
					usleep(750000);
					Spi_Open_port();
				}
			}
		}
		
		if(resets >= 30)
		{
			//cout << "done reading, resets: " << resets << endl;
		}

		frameBuffer = (uint16_t *)result;
		int row, column;
		uint16_t value;
		uint16_t minValue = 65535;
		uint16_t maxValue = 0;
		float temp;

		
		for(int i=0;i<FRAME_SIZE_UINT16;i++)
		{
			//skip the first 2 uint16_t's of every packet, they're 4 header bytes
			if(i % PACKET_SIZE_UINT16 < 2)
			{
				continue;
			}
			
			//flip the MSB and LSB at the last second
			int temp = result[i*2];
			result[i*2] = result[i*2+1];
			result[i*2+1] = temp;
			
			value = frameBuffer[i];
			if(value > maxValue)
			{
				maxValue = value;
			}
			if(value < minValue)
			{
				minValue = value;
			}
		}

		float diff = maxValue - minValue;
		float scale = 255/diff;

		for(int i=0;i<FRAME_SIZE_UINT16;i++)
		{
			if(i % PACKET_SIZE_UINT16 < 2)
			{
				continue;
			}
			value = (frameBuffer[i] - minValue) * scale;
			
			column = (i % PACKET_SIZE_UINT16 ) - 2;
			row = i / PACKET_SIZE_UINT16;
			
			thermal_image.ptr()[row * thermal_image.cols + column] = value;	
		}

		cap >> pi_image;
		
		//cout << pi_image.size() << endl; //[640 x 480]

		resize(thermal_image, thermal_image_r, pi_image.size());
		
		cvtColor( pi_image, pi_image_gray, COLOR_BGR2GRAY );
		
		Sobel(thermal_image_r, "Edge Map Thermal Image");
		Sobel(pi_image_gray, "Edge Map Pi Image");
		
		int key = waitKey(15);

		if(key == 27)
		{
			break;
		}
	}
	
	Spi_Close_port();

	return 0;
}

static void Sobel(Mat image_gray, const char* window_name)
{
	Mat grad;
	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;
  
	GaussianBlur( image_gray, image_gray, Size(3,3), 0, 0, BORDER_DEFAULT );
	
	Mat grad_x, grad_y;
	Mat abs_grad_x, abs_grad_y;
  
	//Scharr( image_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
	Sobel( image_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
  
	convertScaleAbs( grad_x, abs_grad_x );
  
	//Scharr( image_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
	Sobel( image_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
  
	convertScaleAbs( grad_y, abs_grad_y );
  
	addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad );
  
	imshow( window_name, grad );
  
}


