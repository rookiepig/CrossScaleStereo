///////////////////////////////////////////////////////
// File: GetMethod
// Desc: Get all the method interface by name
//
// Author: Zhang Kang
// Date: 2013/09/06
///////////////////////////////////////////////////////
#pragma  once
#include "CC/GrdCC.h"
#include "CC/CenCC.h"
#include "CC/CGCC.h"
#include "CAFilter/GFCA.h"
#include "CAFilter/BFCA.h"
#include "CAFilter/BoxCA.h"
#include "CANLC/NLCCA.h"
#include "CAST/STCA.h"
#include "PPWM/WMPP.h"
#include "PPSG/SGPP.h"

// get cost compuation method name
CCMethod* getCCType( const string name ) 
{
	if( name == "GRD" ) {
		return new GrdCC();
	} else if( name == "CEN" ) {
		return new CenCC();
	} else if( name == "BSM" ) {
		return NULL;
	} else if ( name == "CG" ) {
		return new CGCC();
	}
}

// get cost aggregation method name
CAMethod* getCAType( const string name )
{
	if( name == "GF" ) {
		return new GFCA();
	} else if( name == "BF" ) {
		return new BFCA();
	} else if( name == "BOX" ) {
		return new BoxCA();
	} else if( name == "NL" ) {
		return new NLCCA();
	} else if ( name == "ST" ) {
		return new STCA();
	}
}
// get cost compuation method name
PPMethod* getPPType( const string name ) 
{
	if( name == "SG" ) {
		return new SGPP();
	} else if( name == "WM" ) {
		return new WMPP();
	} else if( name == "NP" ) {
		return NULL;
	}
}