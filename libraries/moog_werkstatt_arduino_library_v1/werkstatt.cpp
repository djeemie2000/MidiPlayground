/*
 werkstatt.cpp -   
 Library for the Moog Werkstatt-01
 Moog Music Inc. 2014
 Written by Chris David Howe
 */

#include "Arduino.h"
#include "werkstatt.h"




/* INTERFACE */

interface::interface(int interface_pin)
{
	pinMode(interface_pin, INPUT);
	_pin = interface_pin;
	
	int buttonCounter = 0;   // counter for the number of button presses
	int lastbuttonState = 0;     // previous state of the button
	
}

int interface::momentary(int min, int max)
{
	int momentaryState = digitalRead(_pin);
	int output = map(momentaryState, 0, 1, min, max);//map  values
	return output;
}

int interface::toggle(int max)
{
	int buttonState = digitalRead(_pin);
	
	// compare the buttonState to its previous state
	if (buttonState != lastbuttonState) {
		// if the state has changed, increment the counter
		if (buttonState == HIGH) {			
			buttonCounter++;
			}
			//reset buttonCounter to set maximum
			if (buttonCounter == max)
			{
				buttonCounter = 0 ;
			} 
		
	}
	// save the current state as the last state, 
	//for next time through the loop
	lastbuttonState = buttonState;
	return buttonCounter;

}

	

int interface::potentiometer(int min, int max){

	int value = analogRead(_pin);//read the potentiometer value
	int output = map(value, 0, 1023, min, max);//map  values
	return output;
	
	
};
	



/* NOISE */

noise::noise(int noise_pin)
{
	pinMode(noise_pin, OUTPUT);
	_pin = noise_pin;
	reg = 0x55aa55aaL; //The seed for the bitstream. 
	
}
void noise::generate(int noise_color)
{
    b31 = (reg & (1L << 31)) >> 31;
	b29 = (reg & (1L << 29)) >> 29;
	b25 = (reg & (1L << 25)) >> 25;
	b24 = (reg & (1L << 24)) >> 24;
	lobit = b31 ^ b29 ^ b25 ^ b24;
	newr = (reg << 1) | lobit;
	reg = newr;
	digitalWrite (_pin, reg & 1);
	delayMicroseconds (noise_color);
};





/* ARPEGGIATOR */

arpeggiator::arpeggiator(int arp_pin)
{
	pinMode(arp_pin, OUTPUT);
	_pin = arp_pin;
	
}
void arpeggiator::play(float bpm, int note, int note_values)
{
	_bpm = bpm;
	float _q = 1/_bpm*60000; //BPM to milliseconds formula, gives us our quarter note value 
	
	//calculate note durations based on BPM
	float _w = 4*_q; //whole notes
	float _h = _q*2; //half notes
	float _qt = (_q/3)*2; //quarter note triplets
	float _e = _q/2; // eighth notes
	float _et = (_e/3)*2; // eigth note triplets
	float _sx = _q/4; //sixteenth notes
	float _sxt = (_sx/3)*2; //sixteenth note triplets
	float _th = _q/8; //thrity-second notes
	float _sxf = _q/16; //sixty-fourth notes
	
	float _values[] = {_w, _h, _q, _qt, _e, _et, _sx, _sxt, _th, _sxf};

	analogWrite(_pin, note);
	delay(_values[note_values]);

};





/* LFO RATE QUANTIZER*/

lfo::lfo(int lfo_pin, int cv_pin)
{
	pinMode(lfo_pin, OUTPUT);
	_pin = lfo_pin;
	
	pinMode(cv_pin, INPUT);
	
}

void lfo::rate(int cv_note ,int rate_value)
{
	
	int _cv = analogRead(cv_pin);
	int cv_array[13] = {0, 50, 120, 180, 250, 310, 380, 440, 500, 570, 640, 700, 750};
	int _lfo_rate[] = {0, 66, 105, 133, 170 , 201, 235, 255};

	if (_cv >= cv_array[cv_note] && _cv < cv_array[cv_note+1]) 
	{ 
		//do something when note is triggered//
		analogWrite(_pin, _lfo_rate[rate_value] );
	}
	
	
	
	
};





