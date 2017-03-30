/*
 * FirstTest_ConcurrentVIVI.c
 * 
 * Copyright 2017  Toshikazu Kuroda
 * 
 * <Description>
 * This program is for testing the latency from a response to the reinforcer it produces, using an Arduino-Pi interface.
 * Schedule of reinforcement: A concurrent VI VI schedule without a changeover delay.
 * 
 * <References>
 * Serial communication for Linux computers: www.tldp.org/HOWTO/text/Serial-Programming-HOWTO
 * Xorshift random number generator: Marsaglia (2003). Xorshift RNGs. Journal of Statistical Software, 8, 1-6.
 * Flesher-Hoffman distribution: Escobar & Perez-Herrera (2015) Low-cost USB interface for operant research using Arduino and Visual Basic. JEAB, 103, 427-435.
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
int ScheduleValue_2 = 1;
int MaxReinforcer = 1000;
int SessionDuration = 3600;

/* Dependent variables */
unsigned int dvResponse = 0;
unsigned int dvResponse_2 = 0;
int dvReinforcer = 0;
int dvReinforcer_2 = 0;
char DateTime_start[25] = {0};
char DateTime_end[25] = {0};
double Real_Time = 0;

/* Temporary variables */
double Session_Start = 0;
double Period_Start = 0;
double Period_Start_2 = 0;
int VItimer_RFT = 0;
int VItimer_RFT_2 = 0;

int tempReinforcer = 0;
int tempReinforcer2 = 0;

/* Flags */
char RFT_Arranged = 0; // 0 = False, 1 = True
char RFT_Arranged_2 = 0; // 0 = False, 1 = True
char ResponseFlag = 1; // 0 = Response registrable, 1 = Not registrable
char ResponseFlag_2 = 1; // 0 = Response registrable, 1 = Not registrable

/* Fleshler-Hoffman distribution */
#define n 10
int Value_Variable  = 0;
int v  = 0;
int order  = 0;
int Iteration_Variable  = 0;
int rd[n + 1] = {0}; // "+ 1" to allow the insertion of "\n"
int vi[n + 1] = {0};
int Sumxser  = 0;

#define n_2 10
int Value_Variable_2  = 0;
int v_2  = 0;
int order_2  = 0;
int Iteration_Variable_2  = 0;
int rd_2[n_2 + 1] = {0}; // "+ 1" to allow the insertion of "\n"
int vi_2[n_2 + 1] = {0};
int Sumxser_2  = 0;

/* Event Markers */
#define MaxEvents 20000
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
void Fleshler_Hoffman(int Parameter);
void Fleshler_Hoffman_2(int Parameter);
unsigned long xor128();
int TimeOfThePeriod();
int TimeOfThePeriod_2();
int ReadArduino(char *buf);
int WriteArduino(char *buf);
int RealTime();
double get_dtime();
void GetDateTime(char *DateTime);
void EventMarker(char EventType[2]);
void EndSession();
int main(int argc, char **argv);

/* Main process */
void Session()
{			
	usleep(100000); // Wait until serial port stablizes
			
	printf("START\n");
	
	Fleshler_Hoffman(ScheduleValue);
	VItimer_RFT = rd[Value_Variable];
	Period_Start = get_dtime();
	
	Fleshler_Hoffman_2(ScheduleValue_2);
	VItimer_RFT_2 = rd_2[Value_Variable_2];
	Period_Start_2 = get_dtime();
	
	GetDateTime(DateTime_start);
	Session_Start = get_dtime();
	
	int STOP = 0;
    while (STOP == 0) 
    {	
		RealTime();
		
		/* Check the number of reinforcers */ 
		if ((dvReinforcer + dvReinforcer_2) >= MaxReinforcer) 
		{
			
			STOP = 1;
			EndSession();
			break;
		}  
				
		/* Check VIs */
		if (RFT_Arranged == 0) // 0 = False, 1 = True
		{
			VItimer_RFT = rd[Value_Variable] - TimeOfThePeriod();
			if (VItimer_RFT <= 0)
			{
				RFT_Arranged = 1; // True
			}
		}
		
		if (RFT_Arranged_2 == 0) // 0 = False, 1 = True
		{
			VItimer_RFT_2 = rd_2[Value_Variable_2] - TimeOfThePeriod_2();
			if (VItimer_RFT_2 <= 0)
			{
				RFT_Arranged_2 = 1; // True
			}
		}	
						
		/* Receive data from Arduino */
		ReadArduino(buffer);
			
		/* Response 1 */	 
		if ((strcmp(buffer, "00") == 0) || (strcmp(buffer, "01") == 0))
		{
			if (ResponseFlag == 0) 
			{
				ResponseFlag = 1;
				dvResponse++;
				EventMarker("01");
				
				if (RFT_Arranged == 1)
						{
							dvReinforcer++;
							EventMarker("03");
							WriteArduino("r");	
							printf("L: %d\n", dvReinforcer);
							tempReinforcer++;
							tempReinforcer2 = 0;
							usleep(1000000); 	
							Period_Start = get_dtime();
							Value_Variable++;
							Fleshler_Hoffman(ScheduleValue);
							VItimer_RFT = rd[Value_Variable];
							RFT_Arranged = 0;
						}					
			}
		} else if ((strcmp(buffer, "10") == 0) || (strcmp(buffer, "11") == 0))
		{
			ResponseFlag = 0;	
		}
		
		/* Response 2 */	
		if ((strcmp(buffer, "00") == 0) || (strcmp(buffer, "10") == 0))
		{
			if (ResponseFlag_2 == 0) 
			{
				ResponseFlag_2 = 1;
				dvResponse_2++;
				EventMarker("02");
				
				if (RFT_Arranged_2 == 1)
						{
							dvReinforcer_2++;
							EventMarker("04");
							WriteArduino("r");	
							printf("R: %d\n", dvReinforcer_2); 
							tempReinforcer2++;
							tempReinforcer = 0;
							usleep(1000000);
							Period_Start_2 = get_dtime();
							Value_Variable_2++;
							Fleshler_Hoffman_2(ScheduleValue_2);
							VItimer_RFT_2 = rd_2[Value_Variable_2];
							RFT_Arranged_2 = 0;
						}					
			}
		} else if ((strcmp(buffer, "01") == 0) || (strcmp(buffer, "11") == 0))
		{
			ResponseFlag_2 = 0;	
		}	
	} // While loop
}

/* Second VI */
void Fleshler_Hoffman(int Parameter)
{
	if (Value_Variable == n) 
	{
		Value_Variable = 0;
	}
	
	if (Value_Variable == 0) 
	{
		v = Parameter * 1000;
		
		int b;
		for (b = 0; b <= n; b++) 
		{
			rd[b] = 0;
			vi[b] = 0;
		}
		
		int i = 1;
		for (i = 1; i <= n; i++) 
		{
			if (i == n) 
			{
				vi[i] = v * (1 + log(n));
			} else {
				vi[i] = (int)(v * (1 + (log(n)) + (n - i) * (log(n - i)) - (n - i + 1) * log(n - i + 1))); // To use log(n - i), I had to add "-lm" in "Build Command" on Geany
			}
					
			do 
			{
				order = xor128()%n;
			} while (rd[order] != 0);
			
			rd[order] = vi[i];
		}
		
		int a;
		for (a = 0; a <= n; a++) 
		{
			Sumxser = Sumxser + rd[a];
		}
		
		if (Sumxser != (v * n)) 
		{
			rd[0] = rd[0] + ((v * n) - Sumxser);
		}
		Sumxser = 0;
	}
}

/* First VI */
void Fleshler_Hoffman_2(int Parameter)
{
	if (Value_Variable_2 == n_2) 
	{
		Value_Variable_2 = 0;
	}
	
	if (Value_Variable_2 == 0) 
	{
		v_2 = Parameter * 1000;
		
		int b;
		for (b = 0; b <= n_2; b++) 
		{
			rd_2[b] = 0;
			vi_2[b] = 0;
		}
		
		int i = 1;
		for (i = 1; i <= n_2; i++) 
		{
			if (i == n_2) 
			{
				vi_2[i] = v_2 * (1 + log(n_2));
			} else {
				vi_2[i] = (int)(v_2 * (1 + (log(n_2)) + (n_2 - i) * (log(n_2 - i)) - (n_2 - i + 1) * log(n_2 - i + 1)));
			}
					
			do 
			{
				order_2 = xor128()%n_2;
			} while (rd_2[order_2] != 0);
			
			rd_2[order_2] = vi_2[i];
		}
		
		int a;
		for (a = 0; a <= n_2; a++) 
		{
			Sumxser_2 = Sumxser_2 + rd_2[a];
		}
		
		if (Sumxser_2 != (v_2 * n_2)) 
		{
			rd_2[0] = rd_2[0] + ((v_2 * n_2) - Sumxser_2);
		}
		Sumxser_2 = 0;
	}
}

/* Xorshift random number generator */
unsigned long xor128()
{
	// Create an array
	unsigned long seed128[4] = {0};
	unsigned long s = clock();
	int i;
	for (i = 1; i <= 4; i++)
	{
		seed128[i - 1] = s = 1812433253U * (s^(s >> 30)) + i;
	}

	unsigned long t = (seed128[0]^(seed128[0] << 11));
	seed128[0] = seed128[1];
	seed128[1] = seed128[2];
	seed128[2] = seed128[3];
	
	return (seed128[3] = (seed128[3]^(seed128[3] >> 19))^(t^(t >> 8)));
}

/* For the first VI timer */
int TimeOfThePeriod()
{
	return get_dtime() - Period_Start - (tempReinforcer2 * 1000);
}

/* For the second VI timer */
int TimeOfThePeriod_2()
{
	return get_dtime() - Period_Start_2 - (tempReinforcer * 1000);
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
	char strdvResponse1[NumOfLetters];
	char strdvResponse2[NumOfLetters];
	char strdvReinforcer1[NumOfLetters];
	char strdvReinforcer2[NumOfLetters];
	char strRealTime[NumOfLetters];
	char strScheduleValue[NumOfLetters];
	char strMaxReinforcer[NumOfLetters];
	char strSessionDuration[NumOfLetters];
	
	// Convert int to char
	sprintf(strdvResponse1,      "Response1:     %d\n", dvResponse);
	sprintf(strdvResponse2,      "Response2:     %d\n", dvResponse_2);
	sprintf(strdvReinforcer1,    "Reinforcer1:   %d\n", dvReinforcer);
	sprintf(strdvReinforcer2,    "Reinforcer2:   %d\n", dvReinforcer_2);
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
	fputs(strdvResponse1,fp);
	fputs(strdvResponse2,fp);
	fputs(strdvReinforcer1,fp);
	fputs(strdvReinforcer2,fp);
	fputs(strRealTime,fp);
	fputs("\n",fp);
	
	fputs("INDEPENDENT VARIABLES\n",fp);
	fputs(strScheduleValue,fp);
	fputs(strMaxReinforcer,fp);
	fputs(strSessionDuration,fp);
	fputs("\n", fp);
	
	fputs("EVENT MARKERS\n",fp);
	fputs("01: Response1 onset\n",fp);
	fputs("02: Response2 onset\n",fp);
	fputs("03: Reinforcer1 onset \n",fp);
	fputs("04: Reinforcer2 onset \n",fp);
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
	
	// Release the memory set by the malloc command
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



