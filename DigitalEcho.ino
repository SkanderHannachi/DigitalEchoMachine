#include <math.h>
#include <Audio.h>
#include <Wire.h>
#include <SD.h>
#include <SerialFlash.h>

static const uint32_t LEN = 0x06400;
const float THRTH = .0002; 
const int TIME_RAMP = 150; 
const int myInput = AUDIO_INPUT_LINEIN;
const int NUM_READINGS = 100; 


int readings[NUM_READINGS];    
int readIndex = 0;    
int16_t delaybuf[LEN];
float total = 0;
float deltime = 0;
float old_time = 200; 
float err = 0; 


float compute_time(int val);

AudioControlSGTL5000	 audioShield;
AudioSynthWaveformDc	     time_delta_sig;           
AudioEffectModDelay	 delay2;
AudioInputI2S  	  audioInput;         // audio shield: mic or line-in
AudioOutputI2S 	  audioOutput;        // audio shield: headphones & 
AudioConnection 		patchCord1(audioInput, 0, delay2, 0);
AudioConnection 		patchCord2(delay2, 0, audioOutput, 0);
AudioConnection 		patchCord100(time_delta_sig,0, delay2, 1);



void setup() {
	AudioMemory(10);

	audioShield.enable();
	audioShield.inputSelect(myInput);
	audioShield.volume(100);
// 	Serial.begin(9600); 
	delay2.setbuf(LEN, delaybuf);
	time_delta_sig.amplitude(1);
	AudioProcessorUsageMaxReset();
	AudioMemoryUsageMaxReset();

}

void loop() {
	int knob_time = analogRead(3)/ 3 + 5;
// 	Serial.println(deltime);
	deltime = compute_time(knob_time)/340; 
	err = abs(old_time - deltime) ; 
	if (err > THRTH)  time_delta_sig.amplitude(.02+deltime, TIME_RAMP);
	old_time = deltime; 
	delay(1);
}


float compute_time(int val){ 

	total = total - readings[readIndex];
	readings[readIndex] = val;
	total = total + readings[readIndex];
	readIndex = readIndex + 1;
	if (readIndex >= NUM_READINGS) readIndex = 0;
	return total / NUM_READINGS;

}
