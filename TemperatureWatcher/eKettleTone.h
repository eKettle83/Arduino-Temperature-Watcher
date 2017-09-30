#ifndef eKettleTone_h
#define eKettleTone_h

//	Notes
#define NA4 440.00f
#define NB4 493.88f	
#define NC5	523.25f	
#define ND5	587.33f	
#define NE5	659.25f	
#define NF5	698.46f	
#define NG5	783.99f

class eKettleTone{
public:

	eKettleTone(int iPin);

	void AlarmUp();
	void AlarmDown();

private:

	void SimpleToneProgression(int iSteps, bool bUp, unsigned int iDuration_ms);
  	int _iPin;
  	
};

#endif

	
