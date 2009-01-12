/*
 *  LLCalc.cpp
 *  SecondLife
 *
 *  Created by Aimee Walton on 28/09/2008.
 *  Copyright 2008 Aimee Walton.
 *
 */

#include "linden_common.h"

#include "llcalc.h"

#include <boost/spirit/core.hpp>
#include <boost/spirit/error_handling.hpp>

#include "llcalcparser.h"
#include "llmath.h"


// Variable names for use in the build floater
const char* LLCalc::X_POS = "X";
const char* LLCalc::Y_POS = "Y";
const char* LLCalc::Z_POS = "Z";
const char* LLCalc::X_SCALE = "XS";
const char* LLCalc::Y_SCALE = "YS";
const char* LLCalc::Z_SCALE = "ZS";
const char* LLCalc::X_ROT = "XR";
const char* LLCalc::Y_ROT = "YR";
const char* LLCalc::Z_ROT = "ZR";
const char* LLCalc::HOLLOW = "HLW";
const char* LLCalc::CUT_BEGIN = "CB";
const char* LLCalc::CUT_END = "CE";
const char* LLCalc::PATH_BEGIN = "PB";
const char* LLCalc::PATH_END = "PE";
const char* LLCalc::TWIST_BEGIN = "TB";
const char* LLCalc::TWIST_END = "TE";
const char* LLCalc::X_SHEAR = "XSH";
const char* LLCalc::Y_SHEAR = "YSH";
const char* LLCalc::X_TAPER = "XTP";
const char* LLCalc::Y_TAPER = "YTP";
const char* LLCalc::RADIUS_OFFSET = "ROF";
const char* LLCalc::REVOLUTIONS = "REV";
const char* LLCalc::SKEW = "SKW";
const char* LLCalc::X_HOLE = "XHL";
const char* LLCalc::Y_HOLE = "YHL";
const char* LLCalc::TEX_U_SCALE = "TSU";
const char* LLCalc::TEX_V_SCALE = "TSV";
const char* LLCalc::TEX_U_OFFSET = "TOU";
const char* LLCalc::TEX_V_OFFSET = "TOV";
const char* LLCalc::TEX_ROTATION = "TROT";
const char* LLCalc::TEX_TRANSPARENCY = "TRNS";
const char* LLCalc::TEX_GLOW = "GLOW";


LLCalc* LLCalc::sInstance = NULL;

LLCalc::LLCalc() : mLastErrorPos(0)
{
//	mUserVariables = new calc_map_t;
	mVariables = new calc_map_t;
	mConstants = new calc_map_t;
		
	// Init table of constants
	(*mConstants)["PI"] = F_PI;
	(*mConstants)["TWO_PI"] = F_TWO_PI;
	(*mConstants)["PI_BY_TWO"] = F_PI_BY_TWO;
	(*mConstants)["SQRT2"] = F_SQRT2;
	(*mConstants)["DEG_TO_RAD"] = DEG_TO_RAD;
	(*mConstants)["RAD_TO_DEG"] = RAD_TO_DEG;
	(*mConstants)["GRAVITY"] = GRAVITY;
}

LLCalc::~LLCalc()
{
	delete mConstants;
	delete mVariables;
//	delete mUserVariables;	
}

//static
void LLCalc::cleanUp()
{
	delete sInstance;
	sInstance = NULL;
}

//static
LLCalc* LLCalc::getInstance()
{
    if (!sInstance)	sInstance = new LLCalc();
	return sInstance;
}

void LLCalc::setVar(const std::string& name, const F32& value)
{
	(*mVariables)[name] = value;
}

void LLCalc::clearVar(const std::string& name)
{
	mVariables->erase(name);
}

void LLCalc::clearAllVariables()
{
	mVariables->clear();
}

/*
void LLCalc::updateVariables(LLSD& vars)
{
	LLSD::map_iterator cIt = vars.beginMap();
	for(; cIt != vars.endMap(); cIt++)
	{
		setVar(cIt->first, (F32)(LLSD::Real)cIt->second);
	}
}
*/

bool LLCalc::evalString(const std::string& expression, F32& result)
{
	using namespace boost::spirit;
	
	std::string expr_upper = expression;
	LLStringUtil::toUpper(expr_upper);
	
	LLCalcParser calc(result, mConstants, mVariables);

	mLastErrorPos = 0;
	std::string::iterator start = expr_upper.begin();
 	parse_info<std::string::iterator> info;
	
	try
	{
		info = parse(start, expr_upper.end(), calc, space_p);
		lldebugs << "Math expression: " << expression << " = " << result << llendl;
	}
	catch(parser_error<std::string, std::string::iterator> &e)
	{
		mLastErrorPos = e.where - expr_upper.begin();
		
		llinfos << "Calc parser exception: " << e.descriptor << " at " << mLastErrorPos << " in expression: " << expression << llendl;
		return false;
	}
	
	if (!info.full)
	{
		mLastErrorPos = info.stop - expr_upper.begin();
		llinfos << "Unhandled syntax error at " << mLastErrorPos << " in expression: " << expression << llendl;
		return false;
	}
	
	return true;
}
