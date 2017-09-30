#include "eKettleTone.h"
#include "Arduino.h"

eKettleTone::eKettleTone(int iPin)
{
	_iPin = iPin;
}

void eKettleTone::AlarmUp(){
	SimpleToneProgression(5, true, 500);
}

void eKettleTone::AlarmDown(){
	SimpleToneProgression(5, false, 500);
}

void eKettleTone::SimpleToneProgression(int iSteps, bool bUp, unsigned int iDuration_ms){
	
	//	Split Duration into Steps
	iDuration_ms = int (iDuration_ms / iSteps);	

	int iCycle_ms;	//	Duration of half a Tone cycle

	for(int i = 0; i < iSteps; i++){
		//	Iterate steps


  Serial.begin(9600);
  Serial.println("Alarm!");

		//	Simple 1 ms Tone difference between steps
		if (bUp){
			iCycle_ms = i + 1;
		}else{
			iCycle_ms = iSteps - i;
		}

		unsigned long time;
		time = millis();	//	Note the actual time
		while (millis()< time + iDuration_ms){
			//	Loop until the duration of the current step is expired

			//	Piezo OFF
			digitalWrite(_iPin, LOW);
			delayMicroseconds(iCycle_ms * 1000);
			//	Piezo ON
			digitalWrite(_iPin, HIGH);
			delayMicroseconds(iCycle_ms *1000);
		
		}
	}
}


