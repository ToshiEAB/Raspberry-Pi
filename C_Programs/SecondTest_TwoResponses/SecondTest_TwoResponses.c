/*
 * SecondTest_TwoResponses.c
 * 
 * Copyright 2017  Toshikazu Kuroda
 * 
 * <Description>
 * This program is for testing the maximum number of responses per sec can be recorded accurately.
 * 
 * <References>
 * Serial communication for Linux computers: www.tldp.org/HOWTO/text/Serial-Programming-HOWTO
 * 
 */

/* Libraries */  
#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <fcntl.h>  
#include <string.h>  
#include <termios.h>  
#include <time.h>  
#include <math.h>
#include <sys/time.h>

/* Parameters */
int ScheduleValue = 1;
int MaxReinforcer = 1000;
int SessionDuration = 3600;

/* Dependent variables */
unsigned int dvResponse = 0;
int dvReinforcer = 0;
char DateTime_start[25] = {0};
char DateTime_end[25] = {0};
double Real_Time = 0;

/* Temporary variables */
double Session_Start = 0;
int FRcount = 0;

/* Flags */
char RFT_Arranged = 0; // 0 = False, 1 = True
char ResponseFlag = 1; // 0 = Response registrable, 1 = Not registrable

/* Event Markers */
#define MaxEvents 25000
int NumOfEvents  = 0;
char *ptArrayOfEvents[MaxEvents] = {0}; // Use pointer here to record the initial address of each TempString

/* Serial Port */  
#define SERIAL_PORT "/dev/ttyACM0"  // COM 1-3 on Windows is equivalent to ttyACM0-2 on Raspberry Pi
#define BAUDRATE B9600  // Default baudrate of Arduino
#define BUFFSIZE 255  
static int fd = -1;  
char buffer[BUFFSIZE] = {0};

/* Functions */
void Session();
int ReadArduino(char *buf);
int WriteArduino(char *buf);
int RealTime();
double get_dtime();
void GetDateTime(char *DateTime);
void EventMarker(char EventType[2]);
void EndSession();
int main(int argc, char **argv);

/* 2nd Response */
char ResponseFlag_2 = 1; 
unsigned int dvResponse_2 = 0;

/* Main process */
void Session()
{			
	usleep(100000); // Wait until serial port stablizes
			
	printf("START\n");
	
	GetDateTime(DateTime_start);
	Session_Start = get_dtime();
	
	int STOP = 0;
    while (STOP == 0) 
    {	
		
		RealTime();
		
		/* Check the number of responses */
		if ((dvResponse >= 5000) && (dvResponse_2 >= 5000)) 
		{
			STOP = 1;
			EndSession();
			break;
		}  
						
		/* Response */	 		
		ReadArduino(buffer);
		
		
		if ((strcmp(buffer, "00") == 0) || (strcmp(buffer, "01") == 0))
		{
			if (ResponseFlag == 0) 
			{
				ResponseFlag = 1;
				dvResponse++;
				EventMarker("01");
				printf("L: %d\n", dvResponse);					
			}
		} else if ((strcmp(buffer, "10") == 0) || (strcmp(buffer, "11") == 0))
		{
			if (ResponseFlag == 1)
			{
				ResponseFlag = 0;
				EventMarker("02");
			}	
		}
		
		
		/* 2nd Response */		
		if ((strcmp(buffer, "00") == 0) || (strcmp(buffer, "10") == 0)) 
		{
			if (ResponseFlag_2 == 0) 
			{
				ResponseFlag_2 = 1;
				dvResponse_2++;
				EventMarker("05");
				printf("R: %d\n", dvResponse_2);					
			}
		} else if ((strcmp(buffer, "01") == 0) || (strcmp(buffer, "11") == 0))
		{
			if (ResponseFlag_2 == 1)
			{
				ResponseFlag_2 = 0;
				EventMarker("06");
			}	
		}
				
	} // While loop
}

/* Read 2 bytes (2 characters) from Arduino */
int ReadArduino(char *buf)
{
	int byte_count = 0;
	while (byte_count < 2)
	{
		byte_count = read(fd, buf, 2);
		if (byte_count == -1)
		{
			perror("Error SignalFromArduino");
			return -1;
		}
	}
	return 0;
}

/* Send 1 byte (1 character) to Arduino */
int WriteArduino(char *buf)
{	
	int byte_count = 0;
	while (byte_count < 1)
	{
		byte_count = write(fd, buf, sizeof(buf));
		if (byte_count == -1)
		{
			perror("Error SignalToArduino");
			return -1;
		}
	}
	return 0;
}

/* Real time since the onset of session */
int RealTime()
{
	Real_Time = get_dtime() - Session_Start;
	return (int) floor(Real_Time);
}

/* Obtain the computer time */
double get_dtime()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return ((double)(tv.tv_sec)*1000 + (double)(tv.tv_usec)*0.001);
}

/* Obtain date and time */
void GetDateTime(char *DateTime)
{
	time_t timer;
	struct tm *local;
	timer = time(NULL);
	local = localtime(&timer);
	sprintf(DateTime, "%d/%d/%d %d:%d:%d\n", local->tm_year + 1900, local->tm_mon + 1, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec);
}

/* Real time data with event markers */
void EventMarker(char EventType[2])
{
	char TempString[20] = {0};
	sprintf(TempString, "%s: %d", EventType, RealTime());
	NumOfEvents++;	
	ptArrayOfEvents[NumOfEvents] =  (char *)malloc(sizeof(char) * (strlen(TempString) + 1)); 
	strcpy(ptArrayOfEvents[NumOfEvents], TempString);
}

/* Save data in a text file */
void EndSession()
{
	EventMarker("99");
		
	GetDateTime(DateTime_end);

	FILE *fp;
	char *filename = "/home/pi/Documents/Result.txt"; // Name of the data file

	// Create or append a file at 'a' mode; otherwise exit
	if ((fp = fopen(filename, "a")) == NULL) 
	{
		fprintf(stderr, "%s could not be opened.\n", filename);
		exit(EXIT_FAILURE);
	} 
	
	unsigned char NumOfLetters = 50;
	char strdvResponse[NumOfLetters];
	char strdvResponse_2[NumOfLetters];
	char strdvReinforcer[NumOfLetters];
	char strRealTime[NumOfLetters];
	char strScheduleValue[NumOfLetters];
	char strMaxReinforcer[NumOfLetters];
	char strSessionDuration[NumOfLetters];

	
	// Convert int to char
	sprintf(strdvResponse,      "LeftResponse:     %d\n", dvResponse);
	sprintf(strdvResponse_2,    "RightResponse:    %d\n", dvResponse_2);
	sprintf(strdvReinforcer,    "LeftReinforcer:   %d\n", dvReinforcer);
	sprintf(strRealTime,        "RealTime:         %f\n", Real_Time);
	sprintf(strScheduleValue,   "ScheduleValue:     %d\n", ScheduleValue);
	sprintf(strMaxReinforcer,   "MaxReinforcer:     %d\n", MaxReinforcer);
	sprintf(strSessionDuration, "SessionDuration:   %d\n", SessionDuration);

	// Date and Time
	fputs(DateTime_start, fp);
	fputs(DateTime_end, fp);
	fputs("\n", fp);

	// Write a line in the opened file
	fputs("DEPENDENT VARIABLES\n",fp);
	fputs(strdvResponse,fp);
	fputs(strdvResponse_2,fp);
	fputs(strdvReinforcer,fp);
	fputs(strRealTime,fp);
	fputs("\n",fp);
	
	fputs("INDEPENDENT VARIABLES\n",fp);
	fputs(strScheduleValue,fp);
	fputs(strMaxReinforcer,fp);
	fputs(strSessionDuration,fp);
	fputs("\n", fp);
	
	fputs("EVENT MARKERS\n",fp);
	fputs("01: Response onset\n",fp);
	fputs("02: Response offset\n",fp);
	fputs("05: Response_2 onset \n",fp);
	fputs("06: Response_2 offset\n",fp);
	fputs("99: End of session\n",fp);
	fputs("\n",fp);
	
	int i;
	for (i = 1; i <= NumOfEvents; i++)
	{
		fputs(ptArrayOfEvents[i],fp);
		fputs("\n", fp);
	}
	fputs("\n", fp);
	fputs("\n", fp);
	fputs("FINISH\n", fp);
	fputs("\n", fp);
								
	fclose(fp);
	
	// Check to see if the following code really releases the memory
	int j;
	for (j = 0; j <= NumOfEvents; j++)
	{
		free(ptArrayOfEvents[j]);
	}
}

/* Beginning and end of the program */
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

	/* Start a session */
	Session();	
	
	/* End of the session */
	tcsetattr(fd, TCSANOW, &oldtio); // Change back to the original setting of fd
	close(fd); // Close the fd (serial port)
	
	printf("FINISHED!\n");
	
	return 0;
}



