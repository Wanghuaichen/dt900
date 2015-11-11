#ifndef __GEOTEST_H
#define __GEOTEST_H

#include "stm32f4xx.h"

#define R_ref 470.0
#define PI 3.1415926
//#define ADCMID 0x800000
#define DACMID 0x7fff
#define T_DELAY 500
#define AMPGAIN 5

void analog(int option);
int geotest();
int ztest(int freq);
long encode();

struct  Geophone         //parameters of target geonphone under test
{
	float temp;
	float nois;
	float leakage;
	uint32_t resi;
	float freq;
	float damp;
	float sens;
	float dist;
	uint32_t impe;
	uint32_t polarity;
	uint32_t minZ;
	uint32_t maxZ;
	int32_t fault;
	
	uint32_t rmax;
	uint32_t rmin;
	uint32_t rnom;
	float fmax;
	float fmin;
	float fnom;
	float bmax;
	float bmin;
	float bnom;
	float smax;
	float smin;
	float snom;
	uint32_t zmax;
	uint32_t zmin;
	uint32_t znom;
	float rline;
	float rtotal;
	float ztotal;
};

struct GeoParam			//geophone parameters in the list
{
	char type[20];
	float R;
	float Rp;
	float Rn;
	float B;
	float Bp;
	float Bn;
	float S;
	float Sp;
	float Sn;
	float F;
	float Fp;
	float Fn;
	float Z;
	float Zp;
	float Zn;
	float D;
	float M;
	float X;
	float T;
	float DF;
	float speed;
};

struct Settings		//test settings & system settings
{
	uint32_t magic;
	
	uint8_t filename[20];
	
	uint8_t code[20];
	uint32_t serialno;
	
	uint32_t totalparam;
	uint32_t paramnum;
	
	uint32_t sensormode;
	float temperature;
	
	uint32_t shunt;
	uint32_t series;
	uint32_t parallel;
	uint32_t lineR; 
	uint32_t leadin;
	uint32_t interval;
	
	uint32_t ldrate;
	uint32_t polarity;
	uint32_t constant; //1 velocity 0 excursion
	uint32_t iteration;
	uint32_t timeinterval;
	
	uint32_t buzzer;
	uint32_t backlight;
	uint32_t sleeptime;
	uint32_t shuttime;
	uint32_t units;//0 metric 1 imperial
	
	long passwd;
};


#endif

