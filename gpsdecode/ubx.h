#ifndef UBX_H_
#define UBX_H_
//headfile
#include <stdio.h>
#include <string.h>


#define LonOffset 4+4
#define LatOffset 4+8
#define hMSLOffset 4+12
#define hAccOffset 4+16
#define gSpeedOffset 4+20
#define fixTypeOffset 4+29
#define yearOffset 4+30
#define monthOffset 4+32
#define dayOffset 4+33
#define hourOffset 4+34
#define minOffset 4+35
#define secOffset 4+36
#define numSVOffset 4+38

//position fix message
typedef struct Log_PositionFix
{
	short int payload;
	
	unsigned int entryIndex;
	double longitude;
	double latitude;
	double hMSL;//
	double hAcc;//
	double gSpeed;
	char fixType;//2d,3d
	short int  year;
	char month;
	char day;
	char hour;
	char min;
	char sec;
	char numSV;//satellites number;
	unsigned char CK_A;
	unsigned char CK_B;
}Log_PositionFix_;


	
//fun
void * 	get_file_data(char *filename);
int 	Check_Sum_Deal(int CK_A,int CK_B,int length, char *);
void 	Data_Deal(unsigned char *);
void 	show_msg(void);
int 	position_fix_search(unsigned char * ,int);

Log_PositionFix_ *getLpf(unsigned char *data);



#endif
