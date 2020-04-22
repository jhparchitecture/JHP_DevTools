#pragma once
// *****************************************************************************
// JHP_Utilities.h
// JHP Utilities for JHP_Menu Addon
// API Development Kit 21; Mac/Win
//
// Namespaces:		Contact person:
//		-None-
//
// [SG compatible] - Yes
// *****************************************************************************

//VERSIONING INFORMATION
//USE THIS TO TRIGGER RE-IMPORTS/UPDATES



//General
#include	"APIEnvir.h"
#include	"ACAPinc.h"		// also includes APIdefs.h
#include	"APICommon.h"

//For Devtools
#include "windows.h"
#include "Lmcons.h"
#include "iostream"
#include "commdlg.h"

//For Links
#include "shellapi.h"
#include "shlobj.h"
#include "shtypes.h"

//#include <sstream> //this allows us to use string conversions
#include "StringConversion.hpp"
#include "StringComparison.hpp"
#include "String.hpp"

//These are for saving/loading data
#include "Array.hpp"
#include "MemoryOChannel.hpp"
#include "MemoryIChannel.hpp"
#include "SetPlatformProtocol.hpp"
#include "DG.h"

#ifndef	_JHP_UTILITIES_H_
#define	_JHP_UTILITIES_H_
#define NOMINMAX

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// VERSION AND VERSION CHECKING
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define _JHPMENU_ARCHVERSION	22
#define _JHPMENU_VERSION		"19Q3.00"

bool JHP_VersionUpToDate(void); //returns true if there exists a property definition containing our version above. false otherwise

GSErrCode JHP_SyncProjectPluginVersion(void); //sets the project property definition to our version, or creates if doesnt exist.

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// JHP Messages
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//void JHP_MsgBox(const char* str, ...);
//void JHP_Msg(std::string x);
//void ShowMessage(char strMenuPick[]);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// JHP Length and Angle Helpers
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define METFEET 3.28084

double JHP_GetLength(API_Coord a, API_Coord b);
double JHP_GetAngle(API_Coord a, API_Coord b);
double JHP_GetSupplementaryAngle(double angle);
bool JHP_Approx(double a, double b, double epsilon = 0.0001);

GS::UniString JHP_DecInchToStr(double fraction);
GS::UniString JHP_LengthToString(double length);
GS::UniString JHP_AngleToString(double angle);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// JHP Property Functions
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

GS::Array<API_Guid> JHP_GetClassificationDefaults(API_ElemTypeID id, API_ElemVariationID varid);

API_Guid JHP_GetorCreatePropertyGroup(const char* name);

GSErrCode JHP_CreatePropertyDefinition(API_Guid groupGuid, const char* name, API_VariantType type/*, GS::Array<API_Guid> classifications*/);

enum class XMLPropertySet
{
	WallCheckerProperties
};

enum class XMLCriterion
{
	test
};

GSErrCode JHP_ImportProperties(XMLPropertySet properties);

const char* JHP_GetXMLProperties(XMLPropertySet prop);

// ~~~~~~~~~~~~~~~
// Text >
// ~~~~~~~~~~~~~~~

GS::Array<GS::UniString> JHP_Split(GS::UniString string, GS::UniChar delimiter);

GS::UniString JHP_Join(GS::Array<GS::UniString> strings, GS::UniChar delimiter);

enum class JHP_Case
{
	Smart = 2,
	Upper = 1,
	Lower = 0
};

///<summary> Sets the selected text to the desired casing. 
///Asks user for new selection if invalid/missing. </summary>
GSErrCode JHP_TextToCase (JHP_Case textCase);

// ~~~~~~~~~~~~~~~
// Dimension >
// ~~~~~~~~~~~~~~~
GSErrCode DimsTo256(void);

// ~~~~~~~~~~~~~~~
// Links >
// ~~~~~~~~~~~~~~~

enum class JHP_Link
{
	//PDF Links
	PDF_DesignManual = 0,
	PDF_FairHousing = 1, 
	PDF_CityofDallas = 2, 
	PDF_OCEPlotwave = 3, 
	PDF_Dimensions101 = 4,

	//Web Links
	WEB_IBC = 5, 
	WEB_UL = 6,
	WEB_GITHELP = 7,

	//Other "Open" Links
	SLACK_Archicad = 8,
	MSDN_APIDocs = 9,

	//File Locations (over 1000!)
	LOC_Logos = 1001,
	LOC_Standards = 1002,
};

///<summary> 
///Returns the correctly formatted path to the provided link enum. 
///</summary>
const char* JHP_Links(JHP_Link link);

///<summary> 
///Opens the provided link without safety. 
///exflag = "open" for files/links and "explore" for locations.
///</summary>
GSErrCode JHP_Open(const char* exflag, const char* link);

///<summary> 
///Opens the provided link. 
///</summary>
GSErrCode JHP_Open(JHP_Link link);

// ~~~~~~~~~~~~~~~
// Dev Tools >
// ~~~~~~~~~~~~~~~
GS::UniString JHP_UserName(void);
//void JHP_Publish();
std::string ExtractFileName(const std::string& fullPath);
GSErrCode JHP_GetProp(const API_Guid& guid);
GSErrCode JHP_Entget(void);

/* -- Selection Utilities ------------------------------- */

///<summary> 
///Set pointer to all selected neigs;.
///If (newSelectionIfInvalid == true), ask for new selection. 
///</summary>
GSErrCode JHP_GetSelection(GS::Array<API_Neig> *selNeigs, bool newSelectionIfInvalid);

///<summary> 
///Set pointer to single selected neig. 
///If (newSelectionIfInvalid == true), ask for new selection. 
///</summary>
GSErrCode JHP_GetSelection(API_Neig* theNeig, bool newSelectionIfInvalid = false); 

///<summary> 
///Returns the type of the selected neig. 
///If (newSelectionIfInvalid == true), asks for new selection when invalid. 
///</summary>
API_ElemTypeID JHP_GetSelectionType(bool newSelectionIfInvalid = false); 

///<summary> 
///Selects all elements of type "id" on layer "layer" on the active floor. 
///</summary>
GSErrCode JHP_SelectElements(API_ElemTypeID id, const char* layer); 

///<summary> 
///Selects all elements of type and layer matching the currently selected element. 
///</summary>
GSErrCode JHP_SelectSimilar(void);

GSErrCode SelectExteriorWalls();



/* -- Attribute Utilities ------------------------------- */

///<summary> Gets layer attributes by name, returns short for use in testing against element.header.layer </summary>
API_AttributeIndex JHP_GetLayerAttributeIndex(const char* layerName); 


#endif