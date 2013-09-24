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
	} else if( name == "NLC" ) {
		return new NLCCA();
	} else if ( name == "ST" ) {
		return new STCA();
	}
}
// get cost compuation method name
PPMethod* getPPType( const string name ) 
{
	if( name == "MED" ) {
		return NULL;
	} else if( name == "PGF" ) {
		return NULL;
	} else if( name == "NP" ) {
		return NULL;
	}
}