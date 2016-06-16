/*
 * File:   LookupTable.h
 * Author: ivoc, ipfn
 *
 * Created on August 26, 2010
 * last modified on August 26, 2010
 * version: 0.1
 */

#ifndef _PLASMASTATUSGAM_H
#define	_PLASMASTATUSGAM_H

//#include <dirent.h>
#include "DDBInputInterface.h"
#include "DDBOutputInterface.h"
#include "GAM.h"
#include "HtmlStream.h"

OBJECT_DLL(PlasmaStatusGAM)


class PlasmaStatusGAM : public GAM, public HttpInterface {
private:

	DDBInputInterface *SignalsInputInterface;
	DDBOutputInterface *SignalsOutputInterface;

	struct InputInterfaceStruct {
		float RogowskiPlasmaCurrent;
		float InterferometryDensity;
		float HeavyIonBeamR;
		float HeavyIonBeamZ;
		float SineProbeZ;
		float CosineProbeR;
		float MagneticProbesR;
		float MagneticProbesZ;
		float MagneticProbesPlasmaCurrent;
		float ElectricProbesR;
		float ElectricProbesZ;
		float TomographyR;
		float TomographyZ;
		int32 usectime;
	};
	struct OutputInterfaceStruct {
		float PlasmaCurrent;
		float PositionR;
		float PositionZ;
		float Density;
	};
	
	int32 currentTime;

	float high_current_threshold_value;
	bool tomography_radial_bool;
	bool tomography_vertical_bool;
	float tomography_radial_high_current_weight;
	float tomography_radial_low_current_weight;
	float tomography_vertical_high_current_weight;
	float tomography_vertical_low_current_weight;
	bool electric_radial_bool;
	bool electric_vertical_bool;
	float electric_radial_high_current_weight;
	float electric_radial_low_current_weight;
	float electric_vertical_high_current_weight;
	float electric_vertical_low_current_weight;
	bool magnetic_radial_bool;
	bool magnetic_vertical_bool;
	float magnetic_radial_high_current_weight;
	float magnetic_radial_low_current_weight;
	float magnetic_vertical_high_current_weight;
	float magnetic_vertical_low_current_weight;
	bool sine_vertical_bool;
	float sine_vertical_high_current_weight;
	float sine_vertical_low_current_weight;
	bool cosine_radial_bool;
	float cosine_radial_high_current_weight;
	float cosine_radial_low_current_weight;
	bool hibd_radial_bool;
	bool hibd_vertical_bool;
	float hibd_radial_high_current_weight;
	float hibd_radial_low_current_weight;
	float hibd_vertical_high_current_weight;
	float hibd_vertical_low_current_weight;

	bool view_input_variables;


public:

	// Default constructor
	PlasmaStatusGAM();

	// Destructor
	virtual ~PlasmaStatusGAM();

	// Initialise the module
	virtual bool Initialise(ConfigurationDataBase& cdbData);

	// Execute the module functionalities
	virtual bool Execute(GAM_FunctionNumbers functionNumber);

	virtual bool ProcessHttpMessage(HttpStream &hStream);

	OBJECT_DLL_STUFF(PlasmaStatusGAM)
};



#endif	/* _LOOKUPTABLE_H */

