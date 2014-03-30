/*
 * Xrfunit.h
 *
 *  Created on: Jun 2, 2011
 *      Author: root
 */

#ifndef XRFUNIT_H_
#define XRFUNIT_H_

#include "GlobalVar.h"

/*
struct GalilUart //1
{

}*/

struct XSampleLid //2
{
	BYTE ErrorCode;
    int closed;
    enum STATUS {Closed,Colsing,Opened,Opening,Unknown} m_status;
};

struct XRayTube //3
{
	BYTE ErrorCode;
    float m_SafeHV,m_SafeMA;
    float m_HvStep,m_HvTarget;
    float m_MaStep,m_MaTarget;
};

struct XVacuumPump //4
{
	BYTE ErrorCode;
    float m_CurrVacuum;
    float m_WorkingVacuum;
};

struct FlowGas //5
{
	BYTE ErrorCode;
    float m_CurrGasPressure,m_PrevPressure;
    float m_PressureTooLow,m_WorkPressure;
};

struct XShamber //6
{
	BYTE ErrorCode;
    float m_CurrTemp,m_WorkingTemp;
};

struct XSpinner //7
{
	BYTE ErrorCode;
    int spin;
};

struct XBeamStop //8
{
	BYTE ErrorCode;
    int closed;
};

struct XInstrument //0
{
	//BYTE ErrorCode;
    struct XSampleLid  m_sampleLid;
    struct XRayTube    m_xrayTube;
    struct XVacuumPump m_vacuumPump;
    struct FlowGas      m_flowGas;
    struct XShamber     m_shamber;
    struct XSpinner     m_spinner;
    struct XBeamStop    m_beamStop;
};

int init_instrument(); //0
int init_sampleLid();  //2
int init_xrayTube();   //3
int init_vacuumPump(); //4
int init_flowGas();	  //5
int init_shamber();    //6
int init_spinner();    //7
int init_beamStop();   //8

void XsampleLid_update();  //2
void XrayTube_update();    //3
void XvacuumPump_update(); //4
void XflowGas_update();	   //5
void Xshamber_update();    //6
void Xspinner_update();    //7
void XbeamStop_update();   //8

void XsampleLid_perror();  //2
void XrayTube_perror();    //3
void XvacuumPump_perror(); //4
void XflowGas_perror();	  //5
void Xshamber_perror();    //6
void Xspinner_perror();    //7
void XbeamStop_perror();   //8

void instrument_update(void);

//-- process un-packet
int process_sampling_param(BYTE* pdata);
int process_analysis_param(BYTE* pdata);
int set_instrument_env(BYTE* pdata);
int set_instrument_env_adv(BYTE* pdata);
//int upload_env_data(void);
void instrument_change_mode(BYTE* pdata);


#endif /* XRFUNIT_H_ */
