#include <stdio.h>
#include <stdlib.h>

#include "GlobalVar.h"
#include "Xrfunit.h"




//------XUNIT_UPDATE------
void XsampleLid_update()//2
{
	//do device IO
	//Closed,Colsing,Opened,Opening,Unknown
	switch(instrument.m_sampleLid.m_status){
		case Closed:
			break;
		case Colsing:
			break;
		case Opened:
			break;
		case Opening:
			break;
		case Unknown:
			break;
	}
}

void XrayTube_update()  //3
{

}

void XvacuumPump_update() //4
{

}

void XflowGas_update()	 //5
{

}

void Xshamber_update() //6
{

}

void Xspinner_update() //7
{

}

void XbeamStop_update()//8
{

}


//------XUNIT_PERROR------
void XsampleLid_perror() //2
{
	if(instrument.m_sampleLid.ErrorCode)
	{
		printf("XsampleLid_perror: %u \n",instrument.m_sampleLid.ErrorCode);
		//add_to_upload_queue(instrument.m_sampleLid.ErrorCode); //add_to_upload_queue(BYTE)
	}
	return;
}

void XrayTube_perror() //3
{

}

void XvacuumPump_perror()//4
{

}

void XflowGas_perror()  //5
{

}

void Xshamber_perror()//6
{

}

void Xspinner_perror()//7
{

}

void XbeamStop_perror() //8
{

}


int init_instrument()
{
    if(!init_sampleLid()) {
    	printf("error init_sampleLid \n");
    	return 0;
     }

    if(!init_xrayTube()) {
    	printf("error init_xrayTube \n");
    	return 0;
     }

    if(!init_vacuumPump()) {
    	printf("error init_vacuumPump \n");
    	return 0;
     }

    if(!init_flowGas()) {
    	printf("error init_flowGas \n");
    	return 0;
     }

    if(!init_shamber()) {
    	printf("error init_shamber \n");
    	return 0;
     }

    if(!init_spinner()){
    	printf("error init_spinner \n");
    	return 0;

     }
    if(!init_beamStop()){
    	printf("error init_beamStop \n");
    	return 0;
    }
    return 1; //sucess
}

void instrument_change_mode(BYTE* pdata)
{
    if(pdata == NULL) return;
    unsigned char instrument_mode = *pdata;
    switch(instrument_mode){
        case 'E':
        case 'e':
            instrument_status =SwitchToIdle;
            break;
        case 'M': //mesaure
            instrument_status =SwitchToMeasure;
            break;
        case 'C': //change sample
            instrument_status =ChangeSample;
            break;
        case 'P': //pause sampling
            instrument_status =Pause;
            break;
        case 'G': //go on sampling
            instrument_status =ContinueSampling;
            break;
        case 'S': //stop, down to safeHV,safeMA
            instrument_status =Stop;
            break;
    }
}

void instrument_update(void)
{
	XsampleLid_update();  //2
	XsampleLid_perror();

	XrayTube_update();   //3
	XrayTube_perror();

	XvacuumPump_update(); //4
	XvacuumPump_perror();

	XflowGas_update();	   //5
	XflowGas_perror();

	Xshamber_update();    //6
	Xshamber_perror();

	Xspinner_update();    //7
	Xspinner_perror();

	XbeamStop_update();   //8
	XbeamStop_perror();

	//if(send_queue != NULL) pthread_cond_signal
}


//------XUNIT_INITIALIZE------
int init_sampleLid()
{
    FILE* fp = fopen("cfg/samplelid.ini","rw");
    if(fp){
        if(fscanf(fp,"%d",&(instrument.m_sampleLid.closed))!=1) return 0;
        fclose(fp);
    }else {
    	perror("init_sampleLid");
    	return 0;
    }

    instrument.m_sampleLid.ErrorCode = 0x00;

    return 1;
}

int init_xrayTube()
{
    FILE* fp = fopen("cfg/xraytube.ini","rw");
    if(fp){
        if(fscanf(fp,"%f%f",&(instrument.m_xrayTube.m_SafeHV),
                            &(instrument.m_xrayTube.m_SafeMA))!=2) return 0;

        if(fscanf(fp,"%f%f",&(instrument.m_xrayTube.m_HvStep),
                            &(instrument.m_xrayTube.m_HvTarget))!=2) return 0;

        if(fscanf(fp,"%f%f",&(instrument.m_xrayTube.m_MaStep),
                            &(instrument.m_xrayTube.m_MaTarget))!=2) return 0;
        fclose(fp);
    }else {
    	perror("init_xrayTube");
    	return 0;
    }

    instrument.m_xrayTube.ErrorCode = 0x00;

    return 1;
}

int init_vacuumPump()
{
    FILE* fp = fopen("cfg/vacuumpump.ini","rw");
    if(fp){
        //if(fscanf(fp,"%f",&(instrument.m_vacuumPump.m_CurrVacuum))!=1) return 0;
        if(fscanf(fp,"%f",&(instrument.m_vacuumPump.m_WorkingVacuum))!=1) return 0;
        fclose(fp);
    }else {
    	perror("init_vacuumPump");
    	return 0;
    }
    //printf("%f \n",instrument.m_vacuumPump.m_WorkingVacuum);
    instrument.m_vacuumPump.ErrorCode = 0x00;
    return 1;
}

int init_flowGas()
{
    FILE* fp = fopen("cfg/flowgas.ini","rw");
    if(fp){

        if(fscanf(fp,"%f%f",&(instrument.m_flowGas.m_PressureTooLow),
                            &(instrument.m_flowGas.m_WorkPressure))!=2) return 0;
        fclose(fp);

    }else {
    	perror("init_flowGas");
    	return 0;
    }

    instrument.m_flowGas.ErrorCode = 0x00;

    return 1;
}

int init_shamber()
{
    FILE* fp = fopen("cfg/shamconf.ini","rw");
    if(fp){
        if(fscanf(fp,"%f", &(instrument.m_shamber.m_WorkingTemp))!=1) return 0;
        fclose(fp);
    }else {
    	perror("init_shamber");
    	return 0;
    }

    instrument.m_shamber.ErrorCode = 0x00;

    return 1;
}

int init_spinner()
{
    FILE* fp = fopen("cfg/spinconf.ini","rw");
    if(fp){
        if(fscanf(fp,"%d",&(instrument.m_spinner.spin))!=1) return 0;
        fclose(fp);
    }else {
    	perror("init_shamber");
    	return 0;
    }

    instrument.m_spinner.ErrorCode = 0x00;

    return 1;
}

int init_beamStop()
{
    FILE* fp = fopen("cfg/shutconf.ini","rw");
    if(fp){
        if(fscanf(fp,"%d",&(instrument.m_beamStop.closed))!=1) return 0;
        fclose(fp);
    }else {
    	perror("init_shamber");
    	return 0;
    }

    instrument.m_beamStop.ErrorCode = 0x00;

    return 1;
}


