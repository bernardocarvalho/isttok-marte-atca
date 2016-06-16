#ifndef _HEAVYIONBEAMGAM_H
#define _HEAVYIONBEAMGAM_H

//#include <dirent.h>
#include "DDBInputInterface.h"
#include "DDBOutputInterface.h"
#include "GAM.h"
#include "HtmlStream.h"

OBJECT_DLL(HeavyIonBeamGAM)


class HeavyIonBeamGAM : public GAM, public HttpInterface {
private:

	DDBInputInterface *SignalsInputInterface;
	DDBOutputInterface *SignalsOutputInterface;

	struct InputInterfaceStruct {
		int HIBD_Sec_01;
		int HIBD_Sec_02;
		int HIBD_Sec_03;
		int HIBD_Sec_04;
		int HIBD_Sec_05;
		int HIBD_Sec_06;
		int HIBD_Sec_07;
		int HIBD_Sec_08;
		int HIBD_Sec_09;
		int HIBD_Sec_10;
		int HIBD_Sec_11;
		int HIBD_Sec_12;
		int usectime;
	};
	struct OutputInterfaceStruct {
		float HeavyIonBeamR;
		float HeavyIonBeamZ;		
		float HeavyIonBeamISecTotal;
		float HeavyIonBeamRISec;
		float HeavyIonBeamZISec;
		float HeavyIonBeamIprim0;		
		float HeavyIonBeamNeSigmaSimpleTotal;
		float HeavyIonBeamRNeSigmaSimple;
		float HeavyIonBeamZNeSigmaSimple;
	};
	
	//inputs from the MARTe configuration file
	//booleans
	int hibd_radial_bool;
	int hibd_vertical_bool;
	int hibd_pos_from_isec_bool;
	int hibd_pos_from_nesigmasimple_bool;
	//number of summed samples in the current returned from the FPGA
	int hibd_nav;
	//HIBD detector description
	int no_of_sec_hibd_chs; //number of secondary HIBD channels
	float *hibd_sec_chs_Zs;
	float *hibd_sec_chs_dls;
	//HIBD primary current description
	float hibd_iprim_i; //initial injected HIB primary current (nA)
	float hibd_iprim_f; //final injected HIB primary current after hibd_dt ms (nA)
	float hibd_iprim_dt; //time interval between hibd_iprim_i & hibd_iprim_f (ms)
	//For calibration
	float hibd_radial_constant;
	float hibd_radial_offset;
	float hibd_vertical_constant;
	float hibd_vertical_offset;
	//For threshold
	float hibd_isec_total_threshold;
	float hibd_nesigmasimple_total_threshold;
	
	//internal use	
	float *currents; //array to store the values obtained from the FPGA converted to currents
	float conv_to_nA;
	float iprim_a; //HeavyIonBeamIprim0 = iprim_a*time + iprim_b
	float iprim_b; //HeavyIonBeamIprim0 = iprim_a*time + iprim_b
	float *iprim_j; //array to store the values of Ij+
	float *ne_sigma_simple; //array to store the NeSigmaSimple values
	float out_of_bounds_limit; //limiter (mm)
	
	bool view_input_variables;

public:

	// Default constructor
	HeavyIonBeamGAM();

	// Destructor
	virtual ~HeavyIonBeamGAM();

	// Initialise the module
	virtual bool Initialise(ConfigurationDataBase& cdbData);

	// Execute the module functionalities
	virtual bool Execute(GAM_FunctionNumbers functionNumber);

	virtual bool ProcessHttpMessage(HttpStream &hStream);

	OBJECT_DLL_STUFF(HeavyIonBeamGAM)
};


#endif

