#include "ubx.h"
/*file stat headfile   stat*/
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <stdlib.h>//fun malloc

#include <sys/time.h>
struct timeval start,end;

struct Log_PositionFix LPF;
struct stat buf;//use buf.st_size;

int num = 0;
//int main(int argc,char **argv)
//{
//    gettimeofday (&start,NULL);
//    if(argc != 2)
//    {
//        printf("useful: ./app <filename>\n");
//        return -1;
//    }
//    int i=0; //check 3600+ buf message
//    unsigned char * data;
	
//    data = get_file_data("UBX256.DAT");
//    position_fix_search(data, buf.st_size);
//    printf("buf_num = %d \n",num);
	
//    gettimeofday (&end,NULL);
//    printf("time = %ldms  %ldus\n",end.tv_usec/1000-start.tv_usec/1000 , end.tv_usec-start.tv_usec );
	

//    return 0;
//}



void* get_file_data(char *filename)
{
	if(stat(filename,&buf) != 0)
	{
		printf("stat gets error\n");
		return NULL;
	}
	FILE * fp = fopen(filename,"r");
	if(fp == NULL)
	{
		printf("file open fail\n");
		return NULL;
	}
	
	unsigned char * str = (char * )malloc(buf.st_size);//get file data
	fread(str,1,buf.st_size,fp);//read file data
	
	fclose(fp);
	return str;
}


int Check_Sum_Deal(int CK_A,int CK_B,int length,char * check_data)//checksum
{	
	int i;
	unsigned char check_a = 0 ,check_b =  0;//check sum
	for(i=0;i<length;i++)
	{
		check_a = check_a + check_data[i];
		check_b = check_b + check_a ;
	}
	if(CK_A != check_a || CK_B != check_b)
	{
		printf("checksum error\n");
		printf("LPF.A=%d LPF.B=%d   CK_A=%d CK_B=%d\n",CK_A,CK_B,check_a,check_b);
		return -1;
	}
	return 0;
}

//data deal;
void Data_Deal(unsigned char *str) //data start in class 
{
	//output position fix message
	LPF.entryIndex = str[4] | str[5] << 8 | str[6] << 16 | str[7] << 24 ;
	LPF.longitude = str[LonOffset] | str[LonOffset+1] << 8	| str[LonOffset+2] << 16	| str[LonOffset+3] << 24;
	LPF.latitude = 	str[LatOffset] | str[LatOffset+1] << 8	| str[LatOffset+2] << 16	| str[LatOffset+3] << 24;
	LPF.hMSL = str[hMSLOffset] | str[hMSLOffset+1] << 8 | str[hMSLOffset+2] << 16	| str[hMSLOffset+3] << 24;
	LPF.hAcc = str[hAccOffset] | str[hAccOffset+1] << 8 | str[hAccOffset+2] << 16	| str[hAccOffset+3] << 24;
	LPF.gSpeed = str[gSpeedOffset] | str[gSpeedOffset+1] << 8 | str[gSpeedOffset+2] << 16 | str[gSpeedOffset+3] << 24;
	LPF.fixType = str[fixTypeOffset];
	//get time 
	LPF.year = str[yearOffset] | str[yearOffset+1]<<8;
	LPF.month = str[monthOffset];
	LPF.day = str[dayOffset] ;
	LPF.hour = str[hourOffset] ;
	LPF.min = str[minOffset] ;
	LPF.sec = str[secOffset] ;
	
	LPF.numSV = str[numSVOffset];		
	
	//unit conversion
	LPF.longitude =	LPF.longitude / 10000000;
	LPF.latitude = LPF.latitude / 10000000;
	LPF.hMSL = LPF.hMSL / 1000;
	LPF.hAcc = LPF.hAcc / 1000;
	LPF.gSpeed = LPF.gSpeed / 1000;
	
	//time set
	LPF.hour += 8;
	if(LPF.hour >= 24)
	{
		LPF.hour -= 24;
		LPF.day +=1;
	}
}

//printf LPF message
void show_msg(void)
{
	printf("Index=%3d lon=%fE lat=%fN  hMSL=%fm hAcc=%fm gSpeed=%fm/s fixType=%dD time:%d.%d.%d %d:%d:%d  numSV=%d   \n",LPF.entryIndex,LPF.longitude,LPF.latitude,LPF.hMSL,LPF.hAcc,LPF.gSpeed,LPF.fixType,LPF.year,LPF.month,LPF.day,LPF.hour,LPF.min,LPF.sec,LPF.numSV);
}


int position_fix_search(unsigned char * data,int size)
{
	int i,I;

	unsigned char buf_data[44] = {0};//ubx buf data start in class
	for(i=0; i < size; i++)
	{	
		if(data[i] == 0xb5 && data[i + 1] == 0x62)
		{
			if(data[i + 2] == 0x21 && data[i + 3] == 0x0b)//ubx-log
			{	
				LPF.payload = data[i + 4] |data[i + 5] << 8;//get payload
				LPF.CK_A = data[i+6+40] ;//buf checksum CK_A
				LPF.CK_B = data[i+6+41] ;
				
				for(I=0; I < 4 + LPF.payload; I++)	//get ubx buf data start in class
					buf_data[I] = data[i + I + 2];
				
				if(Check_Sum_Deal(LPF.CK_A,LPF.CK_B,4 + LPF.payload,buf_data) != 0)//checksum error
					return -1;//end process
				
				Data_Deal(buf_data);
				show_msg();	
				num += 1;
			}	
		}
	}
	return 0;
}



Log_PositionFix_ *getLpf(unsigned char *data)
{
    unsigned char buf_data[44] = {0};
    LPF.payload = data[4] |data[5] << 8;//get payload
    LPF.CK_A = data[46] ;//buf checksum CK_A
    LPF.CK_B = data[47] ;
    int I;
    for(I=0; I < 4 + LPF.payload; I++)	//get ubx buf data start in class
        buf_data[I] = data[I + 2];

    if(Check_Sum_Deal(LPF.CK_A,LPF.CK_B,4 + LPF.payload,buf_data) != 0)//checksum error
        return NULL;//end process

    Data_Deal(buf_data);
    return &LPF;
}
