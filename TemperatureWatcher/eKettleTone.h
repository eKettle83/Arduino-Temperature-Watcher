#ifndef eKettleTone_h
#define eKettleTone_h

//	Notes
#define A4	440.00
#define B4	493.88	
#define C5	523.25	
#define D5	587.33	
#define E5	659.25	
#define F5	698.46	
#define G5	783.99

class eKettleTone : public Print {
public:

	void eKettleTone(int iPin);

	void AlarmUp();
	void AlarmDown();

private:

	void SimpleToneProgression(int iSteps, bool bUp, unsigned int iDuration_ms);
  	int _iPin;
  	
};

#endif

	
