//******************************************************************************
//
//		LQR.h - LQR CONTROL AND KALMAN FILTER
//		CORONA - CIANCIULLI 05/2019
//
//******************************************************************************

#if !defined (LQR_H)
#define LQR_H

#define	N_state 10
#define	N_input 2
#define	N_output 2


#include "Level0.h"
#include "Level1.h"
#include "Level2.h"
//OBJECT_DLL(IPID)

	struct Kalman{
		float Kalman_R;
		float Kalman_Z;
		float *X_est;}  ;
		
	struct LQRouputs{
		float Ivert;
		float Ihor;}  ;	
		
/** MIMO CONTROLLER */
class LQR {

//OBJECT_DLL_STUFF(IPID)
private:
	
	float *x_dot_pos;
	float *x_pos;
	float *K_LQR_pos;
	float *A_est_pos;
	float *B_est_pos;
	float *C_est_pos;
	float *D_est_pos;
	float *N_BAR_pos;
	float *C_inv_pos;
	
	float *x_dot_neg;
	float *x_neg;
	float *K_LQR_neg;
	float *A_est_neg;
	float *B_est_neg;
	float *C_est_neg;
	float *D_est_neg;
	float *N_BAR_neg;
	float *C_inv_neg;
	
	float *X_LQR;
	float *X_LQR_pos;
	float *X_LQR_neg;

     
public:	


		
	LQR ();
	LQRouputs MIMO_CONTROL_POSITIVE(float R_ref, float Z_ref, float R_real, float Z_real, float I_vertical, float I_horizontal);
	LQRouputs MIMO_CONTROL_NEGATIVE(float R_ref, float Z_ref, float R_real, float Z_real, float I_vertical, float I_horizontal);
	Kalman KALMAN_FILTER_POS(float R_real, float Z_real, float I_vertical, float I_horizontal,int sign);
	Kalman KALMAN_FILTER_NEG(float R_real, float Z_real, float I_vertical, float I_horizontal,int sign);
	int erase(float R_real, float Z_real);
	
	~LQR();

private:

	bool SortWaveform();
	bool RemoveRepeatedValues();

};


#endif
