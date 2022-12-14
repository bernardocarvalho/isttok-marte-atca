#ifndef _INTERFEROMETRYGAM_H
#define	_INTERFEROMETRYGAM_H

//#include <dirent.h>
#include "DDBInputInterface.h"
#include "DDBOutputInterface.h"
#include "GAM.h"
#include "HtmlStream.h"
#include <math.h>

OBJECT_DLL(InterferometryGAM)


class InterferometryGAM : public GAM, public HttpInterface {
private:
	

	DDBInputInterface *SignalsInputInterface;
	DDBOutputInterface *SignalsOutputInterface;

	struct InputInterfaceStruct {
		float ADC_interferometry_sine;
		float ADC_interferometry_cosine;
		float HAlpha;
		int32 DischargeStatus;
	};
	struct OutputInterfaceStruct {
		float InterferometryDensity;
		float InterferometryR;
	};
	
	float offset_sine;
	float offset_cosine;
	float old_offset_sine;
	float old_offset_cosine;
	float corrected_sine;
	float corrected_cosine;
	float old_phase;	
	float minimum_density_for_centred_plasma;
	float constant_phase_to_density;
	float constant_PI;

	float phase;

	bool interferometry_radial_control_bool;


	float sine;
	float cosine;
	float InterferencePhase;
	float InterferenceAmplitude;
	float HAmplitude;
	float PhaseShift;



	int i;

	bool view_input_variables;
	
	
	

public:

	// Default constructor
	InterferometryGAM();

	// Destructor
	virtual ~InterferometryGAM();

	// Initialise the module
	virtual bool Initialise(ConfigurationDataBase& cdbData);

	// Execute the module functionalities
	virtual bool Execute(GAM_FunctionNumbers functionNumber);
	
	virtual bool ProcessHttpMessage(HttpStream &hStream);

	OBJECT_DLL_STUFF(InterferometryGAM)
};



#endif	
