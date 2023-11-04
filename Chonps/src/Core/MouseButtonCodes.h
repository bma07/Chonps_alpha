#ifndef HG_CHONPS_MOUSE_BUTTON_CODES_H
#define HG_CHONPS_MOUSE_BUTTON_CODES_H


enum ChonpsMouseButtonCodes
{
	ChonpsMouseButton_1         = 0,
	ChonpsMouseButton_2         = 1,
	ChonpsMouseButton_3         = 2,
	ChonpsMouseButton_4         = 3,
	ChonpsMouseButton_5         = 4,
	ChonpsMouseButton_6         = 5,
	ChonpsMouseButton_7         = 6,
	ChonpsMouseButton_8         = 7,
	ChonpsMouseButton_Last      = ChonpsMouseButton_8,
	ChonpsMouseButton_Left      = ChonpsMouseButton_1,
	ChonpsMouseButton_Right     = ChonpsMouseButton_2,
	ChonpsMouseButton_Middle    = ChonpsMouseButton_3,
};

enum ChonpsMouseModes
{
	ChonpsMouseCursor_Arrow			= 0,
	ChonpsMouseCursor_Ibeam			= 1,
	ChonpsMouseCursor_Crosshair		= 2,
	ChonpsMouseCursor_PointingHand	= 3,
	ChonpsMouseCursor_ResizeEW		= 4,
	ChonpsMouseCursor_ResizeNS		= 5,
	ChonpsMouseCursor_ResizeNWSE	= 6,
	ChonpsMouseCursor_ResizeNESW	= 7,
	ChonpsMouseCursor_ResizeAll		= 8,
	ChonpsMouseCursor_NotAllowed	= 9,
	ChonpsMouseCursor_HResize		= ChonpsMouseCursor_ResizeEW,
	ChonpsMouseCursor_VRrsize		= ChonpsMouseCursor_ResizeNS,
	ChonpsMouseCursor_Hand			= ChonpsMouseCursor_PointingHand,
	ChonpsMouseCursor_Max			= 10
};

#endif