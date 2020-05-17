// *****************************************************************************
// Source code for 'JHP Menu' Add-On
// API Development Kit 22
//
// Started on 05/29/2019 at 2:30pm by CFranklin
//
// *****************************************************************************


//**********************
//INFORMATION
//**********************

//String-scan codes:
//ACAPI_WriteReport ("the answer is %d", false, i); //where i is an integer
//ACAPI_WriteReport ("the answer is %lf", false, d); //where d is a double

//%c	: single character
//%lf	: double
//%d	: decimal integer
//%f	: float


/* -- Includes  ------------------------------- */

#include "JHP_Utilities.h"

// -----------------------------------------------------------------------------
// EVENTS
// -----------------------------------------------------------------------------

//GSErrCode JHP_AttachObserver(API_Elem_Head** elemHead)
//{
//	GSErrCode err = NoError;
//
//	return err;
//}

GSErrCode ReleaseAllObservedElements(void)
{
	GSErrCode err = NoError;
	//API_Elem_Head** elemHeads;
	

	GS::Array<API_Elem_Head> elemHeads;

	err = ACAPI_Element_GetObservedElements(&elemHeads);

	if (err != NoError || elemHeads.GetSize() == 0) { return err; }

	for (UInt32 i = 0; i < elemHeads.GetSize(); ++i)
	{
		ACAPI_Element_DetachObserver((elemHeads)[i].guid);
	}

	return err;
}



// -----------------------------------------------------------------------------
// Standard message box that takes a single argument
// -----------------------------------------------------------------------------


GS::UniString JHP_UserName()
{
	GS::UniString uniName;
	static char name[UNLEN + 1];
	DWORD size = UNLEN + 1;
	GetUserName((TCHAR*)name, &size);
	return uniName = name;
}

// Usage:  printf("Filename = %s\n", ExtractFileName("c:\\some\\dir\\hello.exe").c_str());
std::string ExtractFileName(const std::string & fullPath)
{
	const size_t lastSlashIndex = fullPath.find_last_of("/\\");
	return fullPath.substr(lastSlashIndex + 1);
}

// -----------------------------------------------------------------------------
// UniStrings
// -----------------------------------------------------------------------------

GS::UniString JHP_Join(GS::Array<GS::UniString> strings, GS::UniChar delimiter)
{
	GS::UniString joinedString = "";
	for (UInt32 i = 0; i < strings.GetSize(); ++i)
	{
		GS::UniString curString = strings[i];
		if (!curString.BeginsWith(delimiter)) { curString.Insert(0, delimiter); }
		joinedString += curString;
	}

	return joinedString;
}

GS::Array<GS::UniString> JHP_Split(GS::UniString string, GS::UniChar delimiter)
{
	GS::Array<GS::UniString> stringArray;

	GS::UIndex first = string.FindFirst(delimiter);
	GS::UIndex last = string.FindLast(delimiter);

	GS::UniString workingString = "";

	for (UInt32 i = first; i < string.GetLength(); ++i)
	{
		workingString += string[i];

		if (workingString.GetLast() == delimiter && workingString.GetLength() != 1)
		{
			stringArray.Push(workingString.GetSubstring(0, workingString.GetLength() - 1));
			workingString = "";
			workingString += delimiter;
		}

		if (i == last) { stringArray.Push(string.GetSubstring(i, MaxUIndex)); workingString = ""; break; }
	}

	return stringArray;
}


// -----------------------------------------------------------------------------
// Properties
// -----------------------------------------------------------------------------





//double scale = 0;
//ACAPI_Database(APIDb_GetDrawingScaleID, &scale); <- sets scale variable to drawing scale. switch out ID for other info.

// -----------------------------------------------------------------------------
// Length and Angle Helpers
// -----------------------------------------------------------------------------

#define METFEET 3.28084

double JHP_GetLength(API_Coord a, API_Coord b)
{
	double length = _CMATH_::sqrt(_CMATH_::pow(b.x - a.x, 2) + _CMATH_::pow(b.y - a.y, 2)) * METFEET;
	length = (_CMATH_::ceil(length * 10000000.0) / 10000000.0);
	return length; //Length returned in decimal feet
}

double JHP_GetAngle(API_Coord a, API_Coord b)
{
	double angle = _CMATH_::abs((_CMATH_::atan2(b.x - a.x, b.y - a.y) * RADDEG) - 90);
	angle *= 10000;
	int angleAsInt = (int)_CMATH_::round(angle);
	angle = ((double)angleAsInt / 10000.0);

	if (angle > 180) { angle = _CMATH_::abs(angle - 360); } //matches angle from archicad while drawing, 4 decimal places
	return angle;
}

double JHP_GetSupplementaryAngle(double angle)
{
	return _CMATH_::abs(180.0000 - angle);
}

bool JHP_Approx(double a, double b, double epsilon)
{
	//tolerance for comparison
	return _CMATH_::abs(a - b) < epsilon;
}

//Input is a decimal representing decimal inches, returns fraction if 1/2", 1/4", or 1/8", decimal otherwise.
GS::UniString JHP_DecInchToStr(double fraction)
{
	GS::UniString inchStr = GS::ValueToUniString(fraction);
	inchStr.DeleteFirst();

	if (JHP_Approx(fraction, 0)) { inchStr = ""; }
	if (JHP_Approx(fraction, 0.125)) { inchStr = " 1/8"; }
	if (JHP_Approx(fraction, 0.250)) { inchStr = " 1/4"; }
	if (JHP_Approx(fraction, 0.375)) { inchStr = " 3/8"; }
	if (JHP_Approx(fraction, 0.500)) { inchStr = " 1/2"; }
	if (JHP_Approx(fraction, 0.625)) { inchStr = " 5/8"; }
	if (JHP_Approx(fraction, 0.750)) { inchStr = " 3/4"; }
	if (JHP_Approx(fraction, 0.875)) { inchStr = " 7/8"; }

	return inchStr + "\"";
}

GS::UniString JHP_LengthToString(double length) //returns a formatted string of feet-inches where inches is accurate to 1/8"
{
	GS::UniString lengthString;

	int feet = (int)length;
	double fraction = length - feet;
	double decimalInches = fraction * 12;
	int inches = (int)decimalInches;
	double inchfract = decimalInches - (double)inches;

	lengthString += GS::ValueToUniString(feet);
	lengthString += "' - ";
	lengthString += GS::ValueToUniString(inches);
	lengthString += JHP_DecInchToStr(inchfract);

	return lengthString;

}

GS::UniString JHP_AngleToString(double angle) //returns a formatted string of feet-inches where inches is accurate to 1/8"
{
	GS::UniString angleString = GS::ValueToUniString(angle);
	angleString += "0000"; //make sure the angle has at least 4 decimal places... fill with zero if absent

	UInt32 decimalIndex = angleString.FindFirst('.');

	angleString = angleString.GetSubstring(0, decimalIndex + 5);

	return angleString;
}

// -----------------------------------------------------------------------------
// Returns attribute ID of the layer passed in via string
// -----------------------------------------------------------------------------

/// Returns the attribute index of the corresponding layer name as it 
/// appears in Archicad for the user. If the layer name is invalid, returns -1.

API_AttributeIndex JHP_GetLayerAttributeIndex(const char* layerName) //Gets the attribute index of a layer
{
	API_Attribute	attrib;
	GSErrCode			err;

	BNZeroMemory(&attrib, sizeof(API_Attribute));
	attrib.header.typeID = API_LayerID;
	strcpy(attrib.header.name, layerName);

	err = ACAPI_Attribute_Search(&attrib.header);
	if (err != NoError)
	{
		return -1;
	}

	return attrib.header.index;
}


// -----------------------------------------------------------------------------
// Adds a single Neig to the current selection. 
// -----------------------------------------------------------------------------

///Added this helper function because ACAPI_Element_Select() takes in a pointer of pointers,
///like an array of Neigs. due to this, its less trivial to add a single object. A function for
///adding multiple isnt really necessary... just use ACAPI_Element_Select(selNeigs, numNeigs, true) 
///where numNeigs is the number to add, and true is add to selection.

GSErrCode JHP_AddToSelection(API_Neig* theNeig)
{
	GSErrCode				err;
	GS::Array<API_Neig> 	selNeigs;
	selNeigs.Push(*theNeig);
	//API_Neig** selNeigs = (API_Neig **)BMAllocateHandle(sizeof(API_Neig), ALLOCATE_CLEAR, 0);
	//(*selNeigs)[0] = *theNeig;
	err = ACAPI_Element_Select(selNeigs, true);
	//BMKillHandle((GSHandle*)& selNeigs);
	selNeigs.Clear();
	return err;
}

// --------------------------------------------------------------------------------------------
// Sets the passed pointer to collection of the selection, or asks for new selection if invalid
// --------------------------------------------------------------------------------------------

///This version of JHP_GetSelection accepts a pointer to a pointer to a pointer
///meaning that when we use the *selneigs in the ClickElements, it dereferences it to
///a pointer to a pointer, satisfying that call. it also works with ACAPI_Selection_Get.

///This also means that when calling the function for multiple, you correctly allocate a 
///pointer to a pointer: 
///
///API_Neig** selNeigs = (API_Neig **)BMAllocateHandle(sizeof(API_Neig), ALLOCATE_CLEAR, 0);
///
///and then call the function like this:
///
///JHP_GetSelection(&selNeigs);
///
///finally, free the memory when you're done:
///
///BMKillHandle((GSHandle*)&selNeigs);

///The bool is optional, meaning leaving it out will default to false, and only return the
///selection. Handling the empty selection is up to you. 

///Otherwise, settings the bool to true, i.e. JHP_GetSelection(&selNeigs, true); 
///will ask for a valid selection if nothing is selected.

GSErrCode JHP_GetSelection(GS::Array<API_Neig> *selNeigs, bool newSelectionIfInvalid)
{
	API_SelectionInfo		selectionInfo;
	GSErrCode				err;

	err = ACAPI_Selection_Get(&selectionInfo, selNeigs, false); //Gets the currect selection and sets to pointer 
	BMKillHandle((GSHandle*)& selectionInfo.marquee.coords);

	if (err == APIERR_NOSEL)
		err = NoError;

	if (selectionInfo.typeID == API_SelEmpty && newSelectionIfInvalid) //if nothing selected, ask for new selection
	{
		*selNeigs = ClickElements_Neig("Select Some items", API_ZombieElemID);
	}

	return err;
}

// --------------------------------------------------------------------------------------------------
// Sets the passed Neig pointer to a single selection of a neig, or asks for new selection if invalid
// --------------------------------------------------------------------------------------------------

///This version of JHP_GetSelection accepts simply a pointer to a neig

///This means that when calling the function, youll simply have a variable that is not a pointer
///and pass it in as a reference, making it a pointer:
///
///BNClear(theNeig);   
///JHP_GetSelection(&theNeig);

///The bool is optional, meaning leaving it out will default to false, and only return the
///selection. Handling the empty selection is up to you. 

///Otherwise, settings the bool to true, i.e. JHP_GetSelection(&selNeigs, true); 
///will deselect the current selection and ask for a valid selection.

GSErrCode JHP_GetSelection(API_Neig* theNeig, bool newSelectionIfInvalid)
{
	GS::Array<API_Neig> 	selNeigs;
	API_SelectionInfo		selectionInfo;
	GSErrCode				err;

	//selNeigs = (API_Neig **)BMAllocateHandle(sizeof(API_Neig), ALLOCATE_CLEAR, 0); //allocates a collection of neigs to hold selection

	err = ACAPI_Selection_Get(&selectionInfo, &selNeigs, false); //gets the current selection
	BMKillHandle((GSHandle*)& selectionInfo.marquee.coords);

	if (err == APIERR_NOSEL) //lets us bypass the nothing selected error
		err = NoError;

	if (selectionInfo.typeID != API_SelEmpty)
	{
		//UInt32 nSel = selNeigs.Count;//BMGetHandleSize((GSHandle)selNeigs) / sizeof(API_Neig); //gets the number of objects selected
		if (selNeigs.GetSize() == 1)
		{
			*theNeig = selNeigs[0];
		}
		else if (newSelectionIfInvalid) //more than 1 selected
		{
			err = ACAPI_Element_DeselectAll();//empty the selection
			if (!ClickAnElem("Click an element:", API_ZombieElemID, theNeig))
			{
				return err;
			}

			JHP_AddToSelection(theNeig); //add new single selection 
		}
	}
	else if (newSelectionIfInvalid) //nothing is selected
	{
		if (!ClickAnElem("Click an element:", API_ZombieElemID, theNeig))
		{
			return err;
		}

		JHP_AddToSelection(theNeig); //add new single selection
	}

	//BMKillHandle((GSHandle*)&selNeigs);
	selNeigs.Clear();

	return err;
}



// -----------------------------------------------------------------------------
// Select all of the provided type on the active floor and by layer
// -----------------------------------------------------------------------------

/// Proof of concept for iterating elements and selecting desired based on id and layer

//OVERLOAD FOR SHORT LAYER
GSErrCode JHP_SelectElements(API_ElemTypeID id, API_AttributeIndex layer)
{
	GSErrCode				err;

	// ----- Deselect all selected elements -----
	err = ACAPI_Element_DeselectAll();
	if (err != NoError)
		ErrorBeep("ACAPI_Element_Select", err);

	// ----- Select all elements -----
	GS::Array<API_Guid> elemList;
	err = ACAPI_Element_GetElemList(id, &elemList, APIFilt_OnActFloor); //get all elements of a type id on active floor
	
	GS::Array<API_Neig> selNeigs;
	//int selCount = 0;//Added counter to keep track of exact number of elements afterwards. 
	
	for (GS::Array<API_Guid>::ConstIterator it = elemList.Enumerate(); it != nullptr; ++it) { //This is how the archicad api documentation says to iterate all elements of a type

		API_Element element;
		API_Neig neig;
		BNZeroMemory(&element, Sizeof32(element));
		BNZeroMemory(&neig, Sizeof32(neig));
		element.header.guid = *it;
		neig.guid = *it;

		if (ACAPI_Element_GetHeader(&element.header) == NoError)
		{
			if (element.header.layer != layer)
			{
				continue;
			}
			
			//IMPROVED SPEED A TON BY ADDING A COUNTER AND ALLOCATING SPACE ONLY ONCE FOR THE WHOLE SELECTION!
			//(*selNeigs)[selCount].guid = element.header.guid;
			//++selCount;
			selNeigs.Push(neig);
		}
	}
	//selNeigs = (API_Neig **)BMReallocHandle((GSHandle)selNeigs, selCount * sizeof(API_Neig), 0, 0); //this lets me resize the array!
	err = ACAPI_Element_Select(selNeigs, true);
	//BMKillHandle(reinterpret_cast<GSHandle*> (&selNeigs));

	return err;
}

//OVERLOAD FOR CHAR LAYER
GSErrCode JHP_SelectElements(API_ElemTypeID id, const char* layer)
{
	return JHP_SelectElements(id, JHP_GetLayerAttributeIndex(layer));
}


// -----------------------------------------------------------------------------
// Select all objects that match the typeID and layer of the selected object
// -----------------------------------------------------------------------------

//Select Similar - Kind of useless but a decent test of our selection utilities

GSErrCode JHP_SelectSimilar(void)
{
	GSErrCode				err = NoError;
	API_Neig				theNeig;

	//BNClear(theNeig);

	err = JHP_GetSelection(&theNeig, true);

	if (err != NoError)
	{
		ACAPI_WriteReport("Error!", true);
		return err;
	}

	if (theNeig.neigID == APINeig_None) //This is how you catch if the selection you asked for comes back with nothing
	{
		ACAPI_WriteReport("Neig is none", true);
		return err;
	}

	API_Element element;
	BNZeroMemory(&element, sizeof(API_Element));
	element.header.guid = theNeig.guid;
	if (ACAPI_Element_Get(&element) == NoError)
	{
		//One Element Selected, do whatever
		err = JHP_SelectElements(element.header.typeID, element.header.layer);
	}

	return err;
}

// -------------------------------------------------------------------------------------------------------------
//Returns the selected objects type, if nothing is selected, asks for a selection and returns that objects type
// -------------------------------------------------------------------------------------------------------------

///To use, create a variable of type API_ElemTypeID and assign it this function like below:

///API_ElemTypeID selectedID;
///selectedID = JHP_GetSelectedType();
///if (selectedID != API_ZombieElemID)
///		{ 
///			selected ID is valid 
///		}
	

API_ElemTypeID JHP_GetSelectionType(bool newSelectionIfInvalid)
{
	API_ElemTypeID			id = API_ZombieElemID; //When using, test that this doesnt return zombie as a catch of errors
	API_Neig				theNeig;

	//BNClear(theNeig); //Clear memory for selected Neig

	if (JHP_GetSelection(&theNeig, newSelectionIfInvalid) != NoError) //Get selected Neig
		return id;

	if (theNeig.neigID == APINeig_None)
		ACAPI_WriteReport("This would be dialog box asking user to choose a type.", true);

	API_Element element;
	BNZeroMemory(&element, sizeof(API_Element));
	element.header.guid = theNeig.guid;
	if (ACAPI_Element_Get(&element) == NoError) //Get element from Neig
	{
		//One Element Selected, get ID
		id = element.header.typeID;
		ACAPI_WriteReport(ElemID_To_Name(id), true);
	}
	
	return id;
}


