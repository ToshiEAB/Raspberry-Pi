/*
 * Maestro_Template.c
 * 
 * Copyright 2017  <Toshikazu Kuroda>
 * 
 * This is a template program for serial communication with Pololu Mini Maestro.
 * 
 */

/* Libraries */
#include <stdio.h>  
#include <unistd.h>  
#include <fcntl.h>  
#include <string.h>  
#include <termios.h>  
#include <math.h>
#include <sys/time.h>

/* Serial port */  
#define SERIAL_PORT "/dev/ttyACM0"  
#define BAUDRATE B57600     
int fd = -1;  

/* Parameters */
int MaxReinforcer = 3;
int FeederDuration = 5000;

/* Depedent variables */
int Response_Left = 0;
int Response_Right = 0;
int Reinforcer = 0;

/* Temporary variables */
char ResponseFlag_Left = 1; // 0 = Response registrable, 1 = Not registrable
char ResponseFlag_Right = 1;

/* Maestro */
unsigned char Channel_Feeder = 0;
unsigned char Channel_Vibrator = 1;
unsigned char Channel_FeederLight =2;
unsigned char Channel_RightRed = 3;
unsigned char Channel_RightGreen = 4;
unsigned char Channel_RightBlue = 5;
unsigned char Channel_RightSensor = 6;
unsigned char Channel_LeftSensor = 7;
unsigned char Channel_LeftRed = 8;
unsigned char Channel_LeftGreen = 9;
unsigned char Channel_LeftBlue = 10;
unsigned char FEEDER_ON[4] = {0};
unsigned char FEEDER_OFF[4] = {0};
unsigned char VIBRATOR_ON[4] = {0};
unsigned char VIBRATOR_OFF[4] = {0};
unsigned char FEEDERLIGHT_ON[4] = {0};
unsigned char FEEDERLIGHT_OFF[4] = {0};
unsigned char RIGHT_RED_ON[4] = {0};
unsigned char RIGHT_RED_OFF[4] = {0};
unsigned char RIGHT_GREEN_ON[4] = {0};
unsigned char RIGHT_GREEN_OFF[4] = {0};
unsigned char RIGHT_BLUE_ON[4] = {0};
unsigned char RIGHT_BLUE_OFF[4] = {0};
unsigned char LEFT_RED_ON[4] = {0};
unsigned char LEFT_RED_OFF[4] = {0};
unsigned char LEFT_GREEN_ON[4] = {0};
unsigned char LEFT_GREEN_OFF[4] = {0};
unsigned char LEFT_BLUE_ON[4] = {0};
unsigned char LEFT_BLUE_OFF[4] = {0};
unsigned char outTemp_Right[2] = {0};
unsigned char outTemp_Left[2] = {0};

/* Functions */   
int maestroGetPosition(unsigned char *command);
int maestroSetTarget(unsigned char *command);
double get_dtime();
void Reinforcement();

/* Main Process */
void Session() 
{   
	printf("Session Start\n");  
	
	int STOP = 0;
    while (STOP == 0) 
    {		
		/* Left response */
		if (maestroGetPosition(outTemp_Left) > 500) 
		{
			if (ResponseFlag_Left == 0) 
			{
				ResponseFlag_Left = 1;
				Response_Left++;
				printf("L: %d\n", Response_Left);
				Reinforcer++;
				Reinforcement();
								
				if (Reinforcer == MaxReinforcer)
				{
					STOP = 1;
				}
			}
		} else
		{
			ResponseFlag_Left = 0;
		}
		
		/* Right response */
		if (maestroGetPosition(outTemp_Right) > 500) 
		{
			if (ResponseFlag_Right == 0) 
			{
				ResponseFlag_Right = 1;
				Response_Right++;
				printf("R: %d\n", Response_Right);
				Reinforcer++;
				Reinforcement();
				
				if (Reinforcer == MaxReinforcer)
				{
					STOP = 1;
				}
			}
		} else
		{
			ResponseFlag_Right = 0;
		}
	}
}  

/* Reinforcement */
void Reinforcement()
{	
	double FeederOnsetTime = get_dtime();
	
	// Feeder & Feeder light on
	maestroSetTarget(FEEDERLIGHT_ON);
	maestroSetTarget(FEEDER_ON);
	
	// Vibration
	maestroSetTarget(VIBRATOR_ON);
	usleep(90000); // 0.09 sec
	maestroSetTarget(VIBRATOR_OFF);
	usleep(30000);
	maestroSetTarget(VIBRATOR_ON);
	usleep(120000);
	maestroSetTarget(VIBRATOR_OFF);
	usleep(30000);
	maestroSetTarget(VIBRATOR_ON);
	usleep(120000);
	maestroSetTarget(VIBRATOR_OFF);
	usleep(30000);
	maestroSetTarget(VIBRATOR_ON);
	usleep(60000);
	
	maestroSetTarget(FEEDER_OFF);
	usleep(10000);
	maestroSetTarget(VIBRATOR_OFF);
	
	int i;
	for (i = 0; i <= 5; i++) 
	{
		maestroSetTarget(FEEDERLIGHT_OFF);
		usleep(200000);
		maestroSetTarget(FEEDERLIGHT_ON);
		usleep(200000);
	}
	 
	maestroSetTarget(FEEDERLIGHT_OFF);
	 
	while ((get_dtime() - FeederOnsetTime) >= FeederDuration) 
	{
		usleep(100000); // 0.1 sec interval
	}
}

/* Timer */
double get_dtime()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return ((double)(tv.tv_sec)*1000 + (double)(tv.tv_usec)*0.001);
}

/* Get Position of Input */
int maestroGetPosition(unsigned char *command)
{       
    if (write(fd, command, sizeof(command)) == -1)
    {
        perror("Error GetPosition");
        return -1;
    }
    
    unsigned char buf[256];
    while (read(fd, buf, sizeof(buf)) < 2) 
    {
		usleep(1);
	} 
	
    return buf[0] + buf[1] * 256;
}

/* Set Position of Output */
int maestroSetTarget(unsigned char *command)
{
    if (write(fd, command, sizeof(command)) == -1)
    {
        perror("Error SetTarget");
        return -1;
    }
    return 0;
}

/* Set Maestro Parameters */
void SetMaestroParameters()
{
	FEEDER_ON[0] = 0x84; 
	FEEDER_ON[1] = Channel_Feeder;
	FEEDER_ON[2] = 0x0;
	FEEDER_ON[3] = 0x36;
	
	FEEDER_OFF[0] = 0x84;
	FEEDER_OFF[1] = Channel_Feeder;
	FEEDER_OFF[2] = 0x7F;
	FEEDER_OFF[3] = 0x7F;
	
	VIBRATOR_ON[0] = 0x84; 
	VIBRATOR_ON[1] = Channel_Vibrator;
	VIBRATOR_ON[2] = 0x0;
	VIBRATOR_ON[3] = 0x3E;
	
	VIBRATOR_OFF[0] = 0x84;
	VIBRATOR_OFF[1] = Channel_Vibrator;
	VIBRATOR_OFF[2] = 0x1;
	VIBRATOR_OFF[3] = 0x0;
	
	FEEDERLIGHT_ON[0] = 0x84; 
	FEEDERLIGHT_ON[1] = Channel_FeederLight;
	FEEDERLIGHT_ON[2] = 0x70;
	FEEDERLIGHT_ON[3] = 0x2E;
	
	FEEDERLIGHT_OFF[0] = 0x84;
	FEEDERLIGHT_OFF[1] = Channel_FeederLight;
	FEEDERLIGHT_OFF[2] = 0x1;
	FEEDERLIGHT_OFF[3] = 0x0;

	RIGHT_RED_ON[0] = 0x84; 
	RIGHT_RED_ON[1] = Channel_RightRed;
	RIGHT_RED_ON[2] = 0x70;
	RIGHT_RED_ON[3] = 0x2E;
	
	RIGHT_RED_OFF[0] = 0x84;
	RIGHT_RED_OFF[1] = Channel_RightRed;
	RIGHT_RED_OFF[2] = 0x1;
	RIGHT_RED_OFF[3] = 0x0;
	
	RIGHT_GREEN_ON[0] = 0x84; 
	RIGHT_GREEN_ON[1] = Channel_RightGreen;
	RIGHT_GREEN_ON[2] = 0x70;
	RIGHT_GREEN_ON[3] = 0x2E;
	
	RIGHT_GREEN_OFF[0] = 0x84;
	RIGHT_GREEN_OFF[1] = Channel_RightGreen;
	RIGHT_GREEN_OFF[2] = 0x1;
	RIGHT_GREEN_OFF[3] = 0x0;
	
	RIGHT_BLUE_ON[0] = 0x84; 
	RIGHT_BLUE_ON[1] = Channel_RightBlue;
	RIGHT_BLUE_ON[2] = 0x70;
	RIGHT_BLUE_ON[3] = 0x2E;
	
	RIGHT_BLUE_OFF[0] = 0x84;
	RIGHT_BLUE_OFF[1] = Channel_RightBlue;
	RIGHT_BLUE_OFF[2] = 0x1;
	RIGHT_BLUE_OFF[3] = 0x0;
		
	LEFT_RED_ON[0] = 0x84; 
	LEFT_RED_ON[1] = Channel_LeftRed;
	LEFT_RED_ON[2] = 0x70;
	LEFT_RED_ON[3] = 0x2E;
	
	LEFT_RED_OFF[0] = 0x84;
	LEFT_RED_OFF[1] = Channel_LeftRed;
	LEFT_RED_OFF[2] = 0x1;
	LEFT_RED_OFF[3] = 0x0;
	
	LEFT_GREEN_ON[0] = 0x84; 
	LEFT_GREEN_ON[1] = Channel_LeftGreen;
	LEFT_GREEN_ON[2] = 0x70;
	LEFT_GREEN_ON[3] = 0x2E;
	
	LEFT_GREEN_OFF[0] = 0x84;
	LEFT_GREEN_OFF[1] = Channel_LeftGreen;
	LEFT_GREEN_OFF[2] = 0x1;
	LEFT_GREEN_OFF[3] = 0x0;
	
	LEFT_BLUE_ON[0] = 0x84; 
	LEFT_BLUE_ON[1] = Channel_LeftBlue;
	LEFT_BLUE_ON[2] = 0x70;
	LEFT_BLUE_ON[3] = 0x2E;
	
	LEFT_BLUE_OFF[0] = 0x84;
	LEFT_BLUE_OFF[1] = Channel_LeftBlue;
	LEFT_BLUE_OFF[2] = 0x1;
	LEFT_BLUE_OFF[3] = 0x0;
	
	outTemp_Right[0] = 0x90; 
	outTemp_Right[1] = Channel_RightSensor;
	
	outTemp_Left[0] = 0x90; 
	outTemp_Left[1] = Channel_LeftSensor;
}

/* Start */
int main(int argc, char **argv)
{  
    /* Open a serial interface
     *   fd (file descriptor) is the name of serial interface
     *   SERIAL_PORT:  pathname
     *   O_RDWR: Option for read and write
     *   O_NOCTTY: This is a device file. So don't assign a controlling terminal (which controls the process, for example, with Ctrl-C) to this serial port.
     */
    fd = open(SERIAL_PORT, O_RDWR | O_NOCTTY);  
    if (fd < 0) 
    {  
        perror(SERIAL_PORT);  
        return -1;  
    }  
    
    /* Interface for controlling a serial port 
     *   tcgetattr...: Save the current setting of fd (serial port) to oldtio
     *   memset...: Clear (set 0) the setting of a new serial port
     */
	struct termios oldtio, newtio;
    tcgetattr(fd, &oldtio);
    memset(&newtio, 0, sizeof(newtio));   
    
    /* Setting for input (c_iflag)
     *   IGNPAR: Ignore data if there is a parity error or a framing error
     *   ICRNL:  Convert carriage return (CR; return to the beginning of a sentence) to new line (NL; "\n")
     */  
    newtio.c_iflag = IGNPAR | ICRNL;  
    
    /* Setting for controlling hardware (c_cflag) 
     *   BAUDRATE: Set baudrate
     *   CS8 : 8n1 (8 bit, Non-parity, stop bit is 1) 
     *   CLOCAL: Local connection (Ignore modem control) 
     *   CREAD: Enable receiving characters
     */
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;  
      
  	/* Setting for local modes (c_lflag)
  	 *   0 = Non-cannonical mode
  	 */
  	newtio.c_lflag = 0;
      
	/* Setting for special characters (c_cc)
	 *   VTIME = 0: Disable the between-character timer 
	 */ 
    newtio.c_cc[VTIME] = 0; 
  
    tcsetattr(fd, TCSANOW, &newtio); // Make the new setting effective right now 
    
    /* Start main process */
    SetMaestroParameters();
    Session();  
    printf("Session End\n"); 
    /* End of main process */


	tcsetattr(fd, TCSANOW, &oldtio); // Change back to the original setting of fd
	close(fd); // Close the fd (serial port)

    return 0;  
}  

