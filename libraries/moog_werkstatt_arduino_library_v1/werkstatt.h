/*
 werkstatt.h -   
 Library for the Moog Werkstatt-01
 Moog Music Inc. 2014
 Written by Chris David Howe
*/

#include "Arduino.h"

#ifndef werkstatt_h
#define werkstatt_h


/* INTERFACE */

class interface
{
public:
	interface(int interface_pin);
	int momentary(int min, int max);
	int toggle(int max);
	int potentiometer(int min, int max);
private:
	int buttonCounter;
	int lastbuttonState;
	int _pin;
};

/* NOISE */

class noise
{
public:
    noise(int noise_pin);
    void generate(int noise_color);
private:
    int _pin;
	int _color;
	unsigned long int reg;
	unsigned long int newr;
	unsigned char lobit;
	unsigned char b31, b29, b25, b24;
};

/* ARPEGGIATOR */

class arpeggiator
{
public:
	arpeggiator(int arp_pin);
	void play(float bpm, int note, int note_values);
private:
	int _pin;
	float _bpm;
};

/* LFO RATE QUANTIZER*/

class lfo
{
public:
	lfo(int lfo_pin, int cv_pin);
	void rate(int cv_note, int rate);
	int cv_pin;
private:
	int _pin;
	int _cv;
	int cv_note;
	int rate_value;
};



//Arpeggiator define

#define tonic 0
#define minor2nd 5
#define major2nd 10
#define minor3rd 15
#define major3rd 20
#define fourth 25
#define tritone 30
#define fifth 35
#define minor6th 40
#define major6th 45
#define minor7th 50
#define major7th 55
#define octave 60
#define octave2 120

#define w 0
#define h 1
#define q 2
#define qt 3
#define e 4
#define et 5
#define sx 6
#define sxt 7
#define th 8
#define sxf 9
	
//LFO Quantizer define

#define C1 0
#define Db 1
#define D 2
#define Eb 3
#define E 4
#define F 5
#define Gb 6
#define G 7
#define Ab 8
#define A 9
#define Bb 10
#define B 11
#define C2 12



#endif