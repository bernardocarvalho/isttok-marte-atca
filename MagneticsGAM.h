#ifndef _MAGNETICSGAM_H
#define	_MAGNETICSGAM_H


//#include <dirent.h>
#include "DDBInputInterface.h"
#include "DDBOutputInterface.h"
#include "GAM.h"
#include "HtmlStream.h"

OBJECT_DLL(MagneticsGAM)


class MagneticsGAM : public GAM, public HttpInterface {
private:

	DDBInputInterface *SignalsInputInterface;
	DDBOutputInterface *SignalsOutputInterface;

	struct InputInterfaceStruct {
		float ADC_magnetic_0;
		float ADC_magnetic_1;
		float ADC_magnetic_2;
		float ADC_magnetic_3;
		float ADC_magnetic_4;
		float ADC_magnetic_5;
		float ADC_magnetic_6;
		float ADC_magnetic_7;
		float ADC_magnetic_8;
		float ADC_magnetic_9;
		float ADC_magnetic_10;
		float ADC_magnetic_11;
		int usectime;
	};
	struct OutputInterfaceStruct {
		float MagneticProbesR;
		float MagneticProbesZ;
		float MagneticProbesPlasmaCurrent;
	};

	int usectime_to_wait_for_starting_operation;
	bool magnetic_radial_bool;
	bool magnetic_vertical_bool;
	int NumberOfProbes;
	float *magnetic_Angles;
	float *magnetic_Calibration;
	int NumberOfMeasurements;
	int *ProbeNumbers;

	float *radial_coeficients;
	float *vertical_coeficients;
	float *remove_offset;
	float *accumulator;
	float *ADC_values;
	float *corrected_probes;
	
	float *points_x;
	float *points_y;
	float *m_x;
	float *m_y;
	float y_a;
	float y_b;
	float x_a;
	float x_b;
	float r_a;
	float r_b;
	float m_a;
	float m_b;
	float plasma_current_convertion_factor;

	int n_samples;
	float probe_radius, major_radius, clip_limit;
	float radial_position, vertical_position;
//	float rOffset, zOffset;
	float magnetic_field_sum;

	bool view_input_variables;

public:

	// Default constructor
	MagneticsGAM();

	// Destructor
	virtual ~MagneticsGAM();

	// Initialise the module
	virtual bool Initialise(ConfigurationDataBase& cdbData);

	// Execute the module functionalities
	virtual bool Execute(GAM_FunctionNumbers functionNumber);
	
	virtual bool ProcessHttpMessage(HttpStream &hStream);
	
	

	OBJECT_DLL_STUFF(MagneticsGAM)
};



#endif	

