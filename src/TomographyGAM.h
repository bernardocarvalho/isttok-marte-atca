/*
 * File:   LookupTable.h
 * Author: ivoc, ipfn
 *
 * Created on August 26, 2010
 * last modified on August 26, 2010
 * version: 0.1
 */

#ifndef _TOMOGRAPHYGAM_H
#define	_TOMOGRAPHYGAM_H

//#include <dirent.h>
#include "DDBInputInterface.h"
#include "DDBOutputInterface.h"
#include "GAM.h"
#include "File.h"
#include "Matrix.h"
#include "HtmlStream.h"

OBJECT_DLL(TomographyGAM)


class TomographyGAM : public GAM, public HttpInterface {
private:
	
	DDBInputInterface *SignalsInputInterface;
	DDBOutputInterface *SignalsOutputInterface;

	struct InputInterfaceStruct {
        float ADC_tomography_top_0;
		float ADC_tomography_top_1;
		float ADC_tomography_top_2;
		float ADC_tomography_top_3;
		float ADC_tomography_top_4;
		float ADC_tomography_top_5;
		float ADC_tomography_top_6;
		float ADC_tomography_top_7;
		float ADC_tomography_outer_0;
		float ADC_tomography_outer_1;
		float ADC_tomography_outer_2;
		float ADC_tomography_outer_3;
		float ADC_tomography_outer_4;
		float ADC_tomography_outer_5;
		float ADC_tomography_outer_6;
		float ADC_tomography_outer_7;
		float ADC_tomography_bottom_0;
		float ADC_tomography_bottom_1;
		float ADC_tomography_bottom_2;
		float ADC_tomography_bottom_3;
		float ADC_tomography_bottom_4;
		float ADC_tomography_bottom_5;
		float ADC_tomography_bottom_6;
		float ADC_tomography_bottom_7;
		int usectime;
	};
	struct OutputInterfaceStruct {
		float TomographyR;
		float TomographyZ;
		float TomographyIntensity;
	};

//	float *signals;

	bool tomography_radial_bool;
    bool tomography_vertical_bool;
	int usectime_to_wait_for_starting_operation;
	int nch;
	int *onlineChannels;

	int nchd;
	int nbf;
	int n_samples;
	int griddim[2];
	float *xx;
	float *yy;
	float *svsolW;
	float *usedSignals;
	float *remove_offset;
	float *accumulator;
	float *tempArrayN;
	float *aFit;

	MatrixT<int> gmask;
	MatrixT<float> svsolV;
	MatrixT<float> svsolU;
	MatrixT<float> grnl00c;
	MatrixT<float> grnl01c;
	MatrixT<float> grnl02c;
	MatrixT<float> grnl10c;
	MatrixT<float> grnl11c;
	MatrixT<float> grnl12c;
	MatrixT<float> grnl20c;
	MatrixT<float> grnl21c;
	MatrixT<float> grnl22c;
	MatrixT<float> grnl10s;
	MatrixT<float> grnl11s;
	MatrixT<float> grnl12s;
	MatrixT<float> grnl20s;
	MatrixT<float> grnl21s;
	MatrixT<float> grnl22s;
	MatrixT<float> reconstruction;

	int temp_dims[2];
	int temp_max_dim;

	float x;
	float y;

	float mass;
	
	int i;
	int j;
	float s;

	bool view_input_variables;

public:

	// Default constructor
	TomographyGAM();

	// Destructor
	virtual ~TomographyGAM();

	// Initialise the module
	virtual bool Initialise(ConfigurationDataBase& cdbData);

	// Execute the module functionalities
	virtual bool Execute(GAM_FunctionNumbers functionNumber);
	
	virtual bool ProcessHttpMessage(HttpStream &hStream);
	
	OBJECT_DLL_STUFF(TomographyGAM)
};



#endif	/* _LOOKUPTABLE_H */

