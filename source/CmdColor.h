// ----------------------------------------------------------------------------
//	File: CmdColor.h
//	Created by: Tyler Staples
//	Date created: 6/10/2014
//	Description: 
//		Sets the draw color.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_CMDCOLOR_H
#define INCLUDED_CMDCOLOR_H
#pragma once

#include "CmdCommand.h"

class CCmdColor : public CCmdCommand
{
public:
	CCmdColor(void) {}
	~CCmdColor(void) {}

public:
	BOOL execute(CString &params);
};

#endif