#ifndef __GEOTEST_H
#define __GEOTEST_H

#include "stm32f4xx.h"

#define R_ref 1000.0
#define PI 3.1415926
//#define ADCMID 0x800000
#define DACMID 0x7fff
#define T_DELAY 500
#define AMPGAIN 5

void step0();
void step1();
void step2();
void step3();
void geotest();

struct  Geophone         //parameters of target geonphone under test
{
	uint32_t sn;
	uint8_t barcode[20];
	uint32_t polarity;
	uint32_t tilt;
	float temp;
	float nois;
	uint32_t resi;
	float freq;
	float damp;
	float sens;
	float dist;
	uint32_t impe;
	uint32_t minZ;
	uint32_t maxZ;
};

struct GeoParam			//geophone parameters in the list
{
	uint8_t type[20];
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
};

struct Settings		//test settings & system settings
{
	uint32_t totalparam;
	uint32_t paramnum;
	
	uint32_t sequence;
	uint32_t ldrate;
	float shunt;
	
	uint32_t testnum;
	uint8_t filename[20];
	
	uint32_t sensormode;
	float temperature;
	uint32_t testmode;
	uint32_t barcode;
	
	uint32_t buzzer;
	uint32_t backlight;
	uint32_t sleeptime;
	uint32_t shuttime;
	
	uint32_t reserved1;
	uint32_t reserved2;
	uint32_t reserved3;
	uint32_t reserved4;
};


#endif

