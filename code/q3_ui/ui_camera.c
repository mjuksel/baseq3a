// Copyright (C) 2009 MDave (ZEQ2 Lite)

/* SETUP MENU CAMERA */

#include "ui_local.h"

#define ART_BACKGROUND		"menu/art/addbotframe"
#define ART_BACK0			"menu/art/back_0"
#define ART_BACK1			"menu/art/back_1"

#define ID_3RD_PERSON	10
#define ID_RANGE			11
#define ID_HEIGHT			12
#define ID_ANGLE			13
#define ID_SLIDE			14
#define ID_BACK				15

typedef struct{
	char* name;
	float defaultvalue;
	float value;
} cameraconfigcvar_t;

typedef struct{
	menuframework_s		menu;

	menutext_s				banner;
	menubitmap_s			background;

	menutext_s				range;
	menutext_s				height;
	menutext_s				angle;

	menuradiobutton_s	thirdperson;

	menuslider_s			rangeSlider;
	menuslider_s			heightSlider;
	menuslider_s			slideSlider;
	menuslider_s			angleSlider;

	qboolean					changesMade;

	menubitmap_s			back;
} camera_t;

static camera_t s_camera;

static cameraconfigcvar_t g_cameraconfigcvars[]={
	{"cg_thirdPerson", 0, 0},
	{"cg_thirdPersonAngle",  0, 0},
	{"cg_thirdPersonHeight", 0, 0},
	{"cg_thirdPersonRange",  0, 0},
	{"cg_thirdPersonSlide",  0, 0},
	{NULL, 0, 0}
};

/* Camera_InitCvars */
static void Camera_InitCvars(void){
	int				i;
	cameraconfigcvar_t* cvarptr;
	cvarptr=g_cameraconfigcvars;
	for(i=0; ;i++,cvarptr++){
		if(!cvarptr->name)
			break;
		// get current value
		cvarptr->value=trap_Cvar_VariableValue(cvarptr->name);
		// get default value
		trap_Cvar_Reset(cvarptr->name);
		cvarptr->defaultvalue=trap_Cvar_VariableValue(cvarptr->name);
		// restore current value
		trap_Cvar_SetValue(cvarptr->name,cvarptr->value);
	}
}

/* Camera_GetCvarDefault */
static float Camera_GetCvarDefault(char* name){
	cameraconfigcvar_t* cvarptr;
	int				i;
	cvarptr=g_cameraconfigcvars;
	for(i=0; ;i++,cvarptr++){
		if(!cvarptr->name)
			return (0);
		if(!strcmp(cvarptr->name,name))
			break;
	}
	return (cvarptr->defaultvalue);
}

/* Camera_GetCvarValue */
static float Camera_GetCvarValue(char* name){
	cameraconfigcvar_t* cvarptr;
	int				i;
	cvarptr=g_cameraconfigcvars;
	for(i=0; ;i++,cvarptr++){
		if(!cvarptr->name)
			return (0);
		if(!strcmp(cvarptr->name,name))
			break;
	}
	return (cvarptr->value);
}

/* Camera_GetConfig */
static void Camera_GetConfig(void){
	s_camera.thirdperson.curvalue=UI_ClampCvar(0,1,trap_Cvar_VariableValue("cg_thirdPerson"));
	s_camera.rangeSlider.curvalue=UI_ClampCvar(31,69,trap_Cvar_VariableValue("cg_thirdPersonRange"));
	s_camera.heightSlider.curvalue=UI_ClampCvar(-20,20,trap_Cvar_VariableValue("cg_thirdPersonHeight"));
	s_camera.slideSlider.curvalue=UI_ClampCvar(-30,30,trap_Cvar_VariableValue("cg_thirdPersonSlide"));
	s_camera.angleSlider.curvalue=UI_ClampCvar(0,359,trap_Cvar_VariableValue("cg_thirdPersonAngle"));
}

/* Camera_SetConfig */
static void Camera_SetConfig(void){
	trap_Cvar_SetValue("cg_thirdPerson",s_camera.thirdperson.curvalue);
	trap_Cvar_SetValue("cg_thirdPersonRange",s_camera.rangeSlider.curvalue);
	trap_Cvar_SetValue("cg_thirdPersonHeight",s_camera.heightSlider.curvalue);
	trap_Cvar_SetValue("cg_thirdPersonSlide",s_camera.slideSlider.curvalue);
	trap_Cvar_SetValue("cg_thirdPersonAngle",s_camera.angleSlider.curvalue);
}

/* Camera_MenuEvent */
static void Camera_MenuEvent(void* ptr,int event){
	switch(((menucommon_s*)ptr)->id){
		{
	case ID_BACK:
		if(event==QM_ACTIVATED){
			if(s_camera.changesMade)
				Camera_SetConfig();
			UI_PopMenu();
		}
		break;
	case ID_3RD_PERSON:
	case ID_RANGE:
	case ID_HEIGHT:
	case ID_SLIDE:
	case ID_ANGLE:
		if(event==QM_ACTIVATED){
			s_camera.changesMade=qtrue;
			Camera_SetConfig();
		}
		break;
		}
	}
}

/* UI_CameraMenu_Draw */
static void UI_CameraMenu_Draw(void){
	UI_DrawBannerString(320,16,"CAMERA OPTIONS",UI_CENTER,color_white);
	UI_DrawNamedPic(320-233,240-166,466,332,ART_BACKGROUND);

	Menu_Draw(&s_camera.menu);
}

/* Camera_MenuInit */
static void Camera_MenuInit(void){
	int y;
	// zero set all our globals
	memset(&s_camera,0,sizeof(camera_t));

	s_camera.menu.draw=UI_CameraMenu_Draw;

	s_camera.menu.wrapAround=qtrue;
	s_camera.menu.fullscreen=qfalse;

	Camera_Cache();

	y=180;
	s_camera.thirdperson.generic.type=MTYPE_RADIOBUTTON;
	s_camera.thirdperson.generic.name="Enable 3rd Person: ";
	s_camera.thirdperson.generic.flags=QMF_SMALLFONT;
	s_camera.thirdperson.generic.callback=Camera_MenuEvent;
	s_camera.thirdperson.generic.id=ID_3RD_PERSON;
	s_camera.thirdperson.generic.x=SCREEN_WIDTH/2;
	s_camera.thirdperson.generic.y=y;

	y+=20;
	s_camera.rangeSlider.generic.type=MTYPE_SLIDER;
	s_camera.rangeSlider.generic.name="Range: ";
	s_camera.rangeSlider.generic.flags=QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_camera.rangeSlider.generic.callback=Camera_MenuEvent;
	s_camera.rangeSlider.generic.id=ID_RANGE;
	s_camera.rangeSlider.generic.x=SCREEN_WIDTH/2;
	s_camera.rangeSlider.generic.y=y;
	s_camera.rangeSlider.minvalue=31;
	s_camera.rangeSlider.maxvalue=69;

	y+=20;
	s_camera.heightSlider.generic.type=MTYPE_SLIDER;
	s_camera.heightSlider.generic.name="Height: ";
	s_camera.heightSlider.generic.flags=QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_camera.heightSlider.generic.callback=Camera_MenuEvent;
	s_camera.heightSlider.generic.id=ID_HEIGHT;
	s_camera.heightSlider.generic.x=SCREEN_WIDTH/2;
	s_camera.heightSlider.generic.y=y;
	s_camera.heightSlider.minvalue=-20;
	s_camera.heightSlider.maxvalue=20;

	y+=20;
	s_camera.slideSlider.generic.type=MTYPE_SLIDER;
	s_camera.slideSlider.generic.name="Slide: ";
	s_camera.slideSlider.generic.flags=QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_camera.slideSlider.generic.callback=Camera_MenuEvent;
	s_camera.slideSlider.generic.id=ID_SLIDE;
	s_camera.slideSlider.generic.x=SCREEN_WIDTH/2;
	s_camera.slideSlider.generic.y=y;
	s_camera.slideSlider.minvalue=-30;
	s_camera.slideSlider.maxvalue=30;

	y+=20;
	s_camera.angleSlider.generic.type=MTYPE_SLIDER;
	s_camera.angleSlider.generic.name="Angle: ";
	s_camera.angleSlider.generic.flags=QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_camera.angleSlider.generic.callback=Camera_MenuEvent;
	s_camera.angleSlider.generic.id=ID_ANGLE;
	s_camera.angleSlider.generic.x=SCREEN_WIDTH/2;
	s_camera.angleSlider.generic.y=y;
	s_camera.angleSlider.minvalue=0;
	s_camera.angleSlider.maxvalue=359;

	s_camera.back.generic.type=MTYPE_BITMAP;
	s_camera.back.generic.name=ART_BACK0;
	s_camera.back.generic.flags=QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_camera.back.generic.id=ID_BACK;
	s_camera.back.generic.callback=Camera_MenuEvent;
	s_camera.back.generic.x=320-128;
	s_camera.back.generic.y=256+128-64;
	s_camera.back.width=128;
	s_camera.back.height=64;
	s_camera.back.focuspic=ART_BACK1;

	Menu_AddItem(&s_camera.menu,&s_camera.thirdperson);
	Menu_AddItem(&s_camera.menu,&s_camera.rangeSlider);
	Menu_AddItem(&s_camera.menu,&s_camera.heightSlider);
	Menu_AddItem(&s_camera.menu,&s_camera.slideSlider);
	Menu_AddItem(&s_camera.menu,&s_camera.angleSlider);

	Menu_AddItem(&s_camera.menu,&s_camera.back);

	// initialize the configurable cvars
	Camera_InitCvars();

	// initialize the current config
	Camera_GetConfig();
}

/* Camera_Cache */
void Camera_Cache(void){
	trap_R_RegisterShaderNoMip(ART_BACKGROUND);
	trap_R_RegisterShaderNoMip(ART_BACK0);
	trap_R_RegisterShaderNoMip(ART_BACK1);
}

/* UI_CameraMenu */
void UI_CameraMenu(void){
	Camera_MenuInit();
	UI_PushMenu(&s_camera.menu);
}
