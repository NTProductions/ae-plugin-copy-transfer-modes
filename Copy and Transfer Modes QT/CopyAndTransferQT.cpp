/*******************************************************************/
/*                                                                 */
/*                      ADOBE CONFIDENTIAL                         */
/*                   _ _ _ _ _ _ _ _ _ _ _ _ _                     */
/*                                                                 */
/* Copyright 2007 Adobe Systems Incorporated                       */
/* All Rights Reserved.                                            */
/*                                                                 */
/* NOTICE:  All information contained herein is, and remains the   */
/* property of Adobe Systems Incorporated and its suppliers, if    */
/* any.  The intellectual and technical concepts contained         */
/* herein are proprietary to Adobe Systems Incorporated and its    */
/* suppliers and may be covered by U.S. and Foreign Patents,       */
/* patents in process, and are protected by trade secret or        */
/* copyright law.  Dissemination of this information or            */
/* reproduction of this material is strictly forbidden unless      */
/* prior written permission is obtained from Adobe Systems         */
/* Incorporated.                                                   */
/*                                                                 */
/*******************************************************************/

/*	Vignette.cpp	

	This is a compiling husk of a project. Fill it in with interesting
	pixel processing code.
	
	Revision History

	Version		Change													Engineer	Date
	=======		======													========	======
	1.0			(seemed like a good idea at the time)					bbb			6/1/2002

	1.0			Okay, I'm leaving the version at 1.0,					bbb			2/15/2006
				for obvious reasons; you're going to 
				copy these files directly! This is the
				first XCode version, though.

	1.0			Let's simplify this barebones sample					zal			11/11/2010

	1.0			Added new entry point									zal			9/18/2017

*/

#include "CopyAndTransferQT.h"

static PF_Err 
About (	
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output )
{
	AEGP_SuiteHandler suites(in_data->pica_basicP);
	
	suites.ANSICallbacksSuite1()->sprintf(	out_data->return_msg,
											"%s v%d.%d\r%s",
											STR(StrID_Name), 
											MAJOR_VERSION, 
											MINOR_VERSION, 
											STR(StrID_Description));
	return PF_Err_NONE;
}

static PF_Err 
GlobalSetup (	
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output )
{
	out_data->my_version = PF_VERSION(	MAJOR_VERSION, 
										MINOR_VERSION,
										BUG_VERSION, 
										STAGE_VERSION, 
										BUILD_VERSION);

	out_data->out_flags =  PF_OutFlag_DEEP_COLOR_AWARE;	// just 16bpc, not 32bpc
	
	return PF_Err_NONE;
}

static PF_Err 
ParamsSetup (	
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output )
{
	PF_Err		err		= PF_Err_NONE;
	PF_ParamDef	def;	

	AEFX_CLR_STRUCT(def);

	PF_ADD_POPUPX("Transfer Mode", 4, 1, "ADD|SCREEN|DIFF|MULT", 0, MODE_DISK_ID);
	
	out_data->num_params = COPYANDTRANSFERQT_NUM_PARAMS;

	return err;
}

static PF_Err 
Render (
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output )
{
	PF_Err				err		= PF_Err_NONE;
	AEGP_SuiteHandler	suites(in_data->pica_basicP);

	/*	Put interesting code here. */
	GainInfo			giP;
	AEFX_CLR_STRUCT(giP);
	A_long				linesL	= 0;

	linesL 		= output->extent_hint.bottom - output->extent_hint.top;
	giP.modeInt = params[COPYANDTRANSFERQT_MODE]->u.pd.value;

	/*PF_CompositeMode	compositeMode;
	compositeMode.opacity = 255;
	compositeMode.opacitySu = 1;
	compositeMode.rgb_only = FALSE;

	switch(giP.modeInt) {
		case 1:
			compositeMode.xfer = PF_Xfer_ADD;
		break;
		case 2:
			compositeMode.xfer = PF_Xfer_SCREEN;
		break;
		case 3:
			compositeMode.xfer = PF_Xfer_DIFFERENCE;
		break;
		case 4:
			compositeMode.xfer = PF_Xfer_MULTIPLY;
		break;
	}*/


	PF_CompositeMode ourCompositeMode;
	switch (giP.modeInt) {
	case 1:
		ourCompositeMode.xfer = PF_Xfer_ADD;
	break;
	case 2:
		ourCompositeMode.xfer = PF_Xfer_SCREEN;
	break;
	case 3:
		ourCompositeMode.xfer = PF_Xfer_DIFFERENCE;
		break;
	case 4:
		ourCompositeMode.xfer = PF_Xfer_MULTIPLY;
		break;
	}
	ourCompositeMode.opacity = 255;
	ourCompositeMode.opacitySu = A_u_short(1.0);

	PF_Rect			rectOne;
	rectOne.left = 0;
	rectOne.top = 0;
	rectOne.right = params[COPYANDTRANSFERQT_INPUT]->u.ld.width;
	rectOne.bottom = params[COPYANDTRANSFERQT_INPUT]->u.ld.height;
	
	if (PF_WORLD_IS_DEEP(output)){
		// 16/32 BPC

		// copy layer effect is applied to, on the output canvas
		ERR(PF_COPY(&params[COPYANDTRANSFERQT_INPUT]->u.ld, output, NULL, NULL));

		ERR(suites.WorldTransformSuite1()->transfer_rect(in_data->effect_ref, in_data->quality, NULL, NULL, &rectOne, &params[COPYANDTRANSFERQT_INPUT]->u.ld, &ourCompositeMode, NULL, 0, 0, output));

	} else {
		// 8 BPC

		// copy layer effect is applied to, on the output canvas
		/*ERR(PF_COPY(&params[COPYANDTRANSFERQT_INPUT]->u.ld, output, NULL, NULL));

		ERR(suites.WorldTransformSuite1()->transfer_rect(in_data->effect_ref,
			in_data->quality,
			NULL,
			NULL,
			&rectOne,
			&params[COPYANDTRANSFERQT_INPUT]->u.ld,
			&compositeMode,
			NULL,
			0,
			0,
			output));*/
	}

	return err;
}


extern "C" DllExport
PF_Err PluginDataEntryFunction(
	PF_PluginDataPtr inPtr,
	PF_PluginDataCB inPluginDataCallBackPtr,
	SPBasicSuite* inSPBasicSuitePtr,
	const char* inHostName,
	const char* inHostVersion)
{
	PF_Err result = PF_Err_INVALID_CALLBACK;

	result = PF_REGISTER_EFFECT(
		inPtr,
		inPluginDataCallBackPtr,
		"CopyAndTransferQT", // Name
		"ADBE CopyAndTransferQT", // Match Name
		"NT Productions", // Category
		AE_RESERVED_INFO); // Reserved Info

	return result;
}


PF_Err
EffectMain(
	PF_Cmd			cmd,
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output,
	void			*extra)
{
	PF_Err		err = PF_Err_NONE;
	
	try {
		switch (cmd) {
			case PF_Cmd_ABOUT:

				err = About(in_data,
							out_data,
							params,
							output);
				break;
				
			case PF_Cmd_GLOBAL_SETUP:

				err = GlobalSetup(	in_data,
									out_data,
									params,
									output);
				break;
				
			case PF_Cmd_PARAMS_SETUP:

				err = ParamsSetup(	in_data,
									out_data,
									params,
									output);
				break;
				
			case PF_Cmd_RENDER:

				err = Render(	in_data,
								out_data,
								params,
								output);
				break;
		}
	}
	catch(PF_Err &thrown_err){
		err = thrown_err;
	}
	return err;
}

