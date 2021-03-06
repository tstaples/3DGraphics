#include "StdAfx.h"
#include "CmdDrawPixel.h"
#include "Utility/ScriptParser.h"
#include "Graphics/Rasterizer.h"

BOOL CCmdDrawPixel::execute( CString &params )
{
	// Decode parameters
	CStringList paramStrList;
	CScriptParser::StringSplit( paramStrList, params, CString( ' ' ) );

	// Need at least 2 params for x, y
	const int numParams = 2;
	if ( paramStrList.GetCount() < numParams )
	{
		return FALSE;
	}

	int coords[numParams];
	POSITION pos = paramStrList.GetHeadPosition();
	for ( int i = 0; i < numParams; i++ )
	{
		CString paramStr = paramStrList.GetNext( pos );

		// Cast to float, add 0.5 and cast into an int
		coords[i] = (int)( wcstod( paramStr, NULL ) + 0.5f );
	}

	CRasterizer::Instance()->DrawPoint( coords[0], coords[1] );

	return TRUE;
}
