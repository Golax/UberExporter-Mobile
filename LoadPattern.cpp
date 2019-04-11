#include "stdafx.h"
#include <sstream>
// This number controls how many object identifiers are requested during each call
// to IEnumPortableDeviceObjectIDs::Next()
//<SnippetContentEnum2>
#define NUM_OBJECTS_TO_REQUEST  10

void print(map<string, vector<string>> pattern) {
	for (auto row : pattern) {
		cout << row.first << endl;
		for (auto ext : row.second)
			cout << ext << " ";
		cout << endl;
	}
}

vector<string> split(string str, char delimiter) {
	vector<string> internal;
	istringstream ss(str); // Turn the string into a stream.
	string tok;

	while (getline(ss, tok, delimiter)) {
		internal.push_back(tok);
	}

	return internal;
}


//load pattern from file
map<string, vector<string>> init_pattern() {
	ifstream pattern_file("pattern.uber");
	string key;
	string values;
	//int pos = 0;
	map<string, vector<string>> UberPattern;
	while (getline(pattern_file, key) && getline(pattern_file, values)) {					//prendo la chiave(cartella)
		UberPattern[key] = (split(values, ' '));
			
	}
	return UberPattern;
}

bool cvtLPW2stdstring(std::string& s, const LPWSTR pw,
	UINT codepage = CP_ACP)
{
	bool res = false;
	char* p = 0;
	int bsz;
	bsz = WideCharToMultiByte(codepage,
		0,
		pw, -1,
		0, 0,
		0, 0);
	if (bsz > 0) {
		p = new char[bsz];
		int rc = WideCharToMultiByte(codepage,
			0,
			pw, -1,
			p, bsz,
			0, 0);
		if (rc != 0) {
			p[bsz - 1] = 0;
			s = p;
			res = true;
		}
	}
	delete[] p;
	return res;
}

//load dirs from file
vector<string> init_dirs() {
	string t_dir;
	vector<string> dirs;
	ifstream list("dirs.uber");
	while (getline(list, t_dir))
		if (t_dir.at(0) != '#')	//elimino i commenti
			dirs.push_back(t_dir);
	return dirs;
}

LPCWSTR GetExtension(IPortableDeviceValues*  pProperties) {
	PWSTR   pszValue = NULL;
	LPCWSTR result = NULL;
	pProperties->GetStringValue(WPD_OBJECT_NAME, &pszValue);
	CAtlStringW strValue = pszValue;
	if (strValue.GetLength() > 4)
		if (strValue.GetAt(strValue.GetLength() - 4) == '.') {
			//printf("\n%ws ", pszValue);
			result = strValue.Mid(strValue.GetLength() - 4, 4).GetString();
			//printf("%ws", strValue.Right(strValue.GetLength() - 4).GetString());
		}
	CoTaskMemFree(pszValue);
	pszValue = NULL;
	return result;
}

//search UberPattern map for a value and return key

//OKKAY SIAMO ARRIVATI QUI, EXT ARRIVA CON SUCCESSO MA NON LA TROVA NELLA LISTA
//VERIFICA SE LA LISTA È PIENA


string uberFind(map<string, vector<string>> pattern, string ext) {
	for (auto row : pattern)
		if (find(row.second.begin(), row.second.end(), ext) != row.second.end())
			return row.first;
	return "";
}

//PWSTR* getNextObjectIDs(PWSTR pszObjectID, IPortableDeviceContent* pContent) {
//	CComPtr<IEnumPortableDeviceObjectIDs> pnextObjectIDs;
//
//	// Get an IEnumPortableDeviceObjectIDs interface by calling EnumObjects with the
//	// specified parent object identifier.
//	HRESULT hr = pContent->EnumObjects(0,               // Flags are unused
//		pszObjectID,     // Starting from the passed in object
//		NULL,            // Filter is unused
//		&pnextObjectIDs);
//	if (hr == S_OK) {
//		PWSTR  nextObjectIDs[50] = { 0 };
//		DWORD fetched = 0;
//		hr = pnextObjectIDs->Next(50, // Number of objects to request on each NEXT call
//			nextObjectIDs,			// pointer of PWSTR which will be populated on each NEXT call
//			&fetched);              // Number of objects written to the PWSTR array
//		return nextObjectIDs;
//	}
//	return NULL;
//}

// Reads properties for the user specified object.
void getPropFromObjID(PWSTR ObjectID, IPortableDeviceContent* pContent, IPortableDeviceValues** parent)
{
	HRESULT                               hr = S_OK;
	CComPtr<IPortableDeviceProperties>    pProperties;
	CComPtr<IPortableDeviceValues>        pObjectProperties;
	CoCreateInstance(CLSID_PortableDeviceValuesCollection,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&pObjectProperties));
	CComPtr<IPortableDeviceKeyCollection> pPropertiesToRead;

	// 2) Get an IPortableDeviceProperties interface from the IPortableDeviceContent interface
	// to access the property-specific methods.
	if (SUCCEEDED(hr))
	{
		hr = pContent->Properties(&pProperties);
		if (FAILED(hr))
		{
			printf("! Failed to get IPortableDeviceProperties from IPortableDevice, hr = 0x%lx\n", hr);
		}
	}

	// 3) CoCreate an IPortableDeviceKeyCollection interface to hold the the property keys
	// we wish to read.
	//<SnippetContentProp1>
	hr = CoCreateInstance(CLSID_PortableDeviceKeyCollection,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&pPropertiesToRead));
	if (SUCCEEDED(hr))
	{
		
		// 4) Populate the IPortableDeviceKeyCollection with the keys we wish to read.
		// NOTE: We are not handling any special error cases here so we can proceed with
		// adding as many of the target properties as we can.
		if (pPropertiesToRead != NULL)
		{
			HRESULT hrTemp = S_OK;
			hrTemp = pPropertiesToRead->Add(WPD_OBJECT_NAME);
			if (FAILED(hrTemp))
			{
				printf("! Failed to add WPD_OBJECT_NAME to IPortableDeviceKeyCollection, hr= 0x%lx\n", hrTemp);
			}

			hrTemp = pPropertiesToRead->Add(WPD_OBJECT_FORMAT);
			if (FAILED(hrTemp))
			{
				printf("! Failed to add WPD_OBJECT_FORMAT to IPortableDeviceKeyCollection, hr= 0x%lx\n", hrTemp);
			}

			hrTemp = pPropertiesToRead->Add(WPD_OBJECT_CONTENT_TYPE);
			if (FAILED(hrTemp))
			{
				printf("! Failed to add WPD_OBJECT_CONTENT_TYPE to IPortableDeviceKeyCollection, hr= 0x%lx\n", hrTemp);
			}
		}
	}

	// 5) Call GetValues() passing the collection of specified PROPERTYKEYs.
	//<SnippetContentProp2>
	if (SUCCEEDED(hr))
	{
		hr = pProperties->GetValues(ObjectID,         // The object whose properties we are reading
			pPropertiesToRead,   // The properties we want to read
			&pObjectProperties); // Driver supplied property values for the specified object
		if (FAILED(hr))
		{
			printf("\n\n\n! Failed ");
		}
		pObjectProperties->QueryInterface(IID_PPV_ARGS(parent));
	}
}

void CollectionValuesFromCollectionIDs(IPortableDevicePropVariantCollection* ppObjectIDs, IPortableDeviceContent* pContent, IPortableDeviceValuesCollection** ppValues) {
	CComPtr<IPortableDeviceValues>        pObjectProperties;
	CComPtr<IPortableDeviceValuesCollection> pValues;
	CoCreateInstance(CLSID_PortableDeviceValuesCollection,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&pValues));
		DWORD numIDS;
		ppObjectIDs->GetCount(&numIDS);
		printf("\n>> %d elementi trovati nel device!", numIDS);
		
		for (DWORD i = 0; i < numIDS; i++)
		{
			PROPVARIANT pvObjectID = { 0 };
			PropVariantInit(&pvObjectID);
			ppObjectIDs->GetAt(i, &pvObjectID);
			pObjectProperties = NULL;
			if ((pvObjectID.pwszVal != NULL) && (pvObjectID.vt == VT_LPWSTR)) {
				getPropFromObjID(pvObjectID.pwszVal, pContent, &pObjectProperties);
				pValues->Add(pObjectProperties);
			}
			//printf("%ws", (PWSTR)pvObjectID.pwszVal);
			PropVariantClear(&pvObjectID);
		}
	pValues->QueryInterface(IID_PPV_ARGS(ppValues));
}


//// Copies data from a source stream to a destination stream using the
//// specified cbTransferSize as the temporary buffer size.
HRESULT StreamCopy(IStream*    pDestStream, IStream*    pSourceStream, DWORD       cbTransferSize, DWORD*      pcbWritten);

void UberCopy(PWSTR ObjectID, PWSTR dest, IPortableDeviceContent* pContent)
{
	HRESULT                            hr = S_OK;
	CComPtr<IPortableDeviceResources>  pResources;
	CComPtr<IPortableDeviceProperties> pProperties;
	CComPtr<IStream>                   pObjectDataStream;
	CComPtr<IStream>                   pFinalFileStream;
	DWORD                              cbOptimalTransferSize = 0;
	CAtlStringW                        strOriginalFileName;


	//</SnippetTransferFrom2>
	// 2) Get an IPortableDeviceResources interface from the IPortableDeviceContent interface to
	// access the resource-specific methods.
	//<SnippetTransferFrom3>
	if (SUCCEEDED(hr))
	{
		hr = pContent->Transfer(&pResources);
		if (FAILED(hr))
		{
		}
	}
	//</SnippetTransferFrom3>
	// 3) Get the IStream (with READ access) and the optimal transfer buffer size
	// to begin the transfer.
	//<SnippetTransferFrom4>
	if (SUCCEEDED(hr))
	{
		hr = pResources->GetStream(ObjectID,             // Identifier of the object we want to transfer
			WPD_RESOURCE_DEFAULT,    // We are transferring the default resource (which is the entire object's data)
			STGM_READ,               // Opening a stream in READ mode, because we are reading data from the device.
			&cbOptimalTransferSize,  // Driver supplied optimal transfer size
			&pObjectDataStream);
		if (FAILED(hr))
		{
		}
	}
	//</SnippetTransferFrom4>

	//</SnippetTransferFrom5>
	// 5) Create a destination for the data to be written to.  In this example we are
	// creating a temporary file which is named the same as the object identifier string.
	//<SnippetTransferFrom6>
	if (SUCCEEDED(hr))
	{
		hr = SHCreateStreamOnFile(dest, STGM_CREATE | STGM_WRITE, &pFinalFileStream);
		if (FAILED(hr))
		{
		}
	}
	
	//</SnippetTransferFrom6>
	// 6) Read on the object's data stream and write to the final file's data stream using the
	// driver supplied optimal transfer buffer size.
	//<SnippetTransferFrom7>
	if (SUCCEEDED(hr))
	{
		DWORD cbTotalBytesWritten = 0;

		// Since we have IStream-compatible interfaces, call our helper function
		// that copies the contents of a source stream into a destination stream.
		hr = StreamCopy(pFinalFileStream,       // Destination (The Final File to transfer to)
			pObjectDataStream,      // Source (The Object's data to transfer from)
			cbOptimalTransferSize,  // The driver specified optimal transfer buffer size
			&cbTotalBytesWritten);  // The total number of bytes transferred from device to the finished file
		if (FAILED(hr))
		{
		}
		else
		{
		}
	}
}

void UberErase(PROPVARIANT ObjectID, IPortableDeviceContent* pContent)
{
	HRESULT                                       hr = S_OK;
	CComPtr<IPortableDevicePropVariantCollection> pObjectsToDelete;
	CComPtr<IPortableDevicePropVariantCollection> pObjectsFailedToDelete;

	// 2) CoCreate an IPortableDevicePropVariantCollection interface to hold the the object identifiers
	// to delete.
	//
	// NOTE: This is a collection interface so more than 1 object can be deleted at a time.
	//       This sample only deletes a single object.
	if (SUCCEEDED(hr))
	{
		hr = CoCreateInstance(CLSID_PortableDevicePropVariantCollection,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&pObjectsToDelete));
		if (SUCCEEDED(hr))
		{
			if (pObjectsToDelete != NULL)
			{
				// Add the object identifier to the objects-to-delete list
				// (We are only deleting 1 in this example)
				hr = pObjectsToDelete->Add(&ObjectID);
				if (SUCCEEDED(hr))
				{
					// Attempt to delete the object from the device
					hr = pContent->Delete(PORTABLE_DEVICE_DELETE_NO_RECURSION,  // Deleting with no recursion
						pObjectsToDelete,                     // Object(s) to delete
						NULL);                                // Object(s) that failed to delete (we are only deleting 1, so we can pass NULL here)
					if (SUCCEEDED(hr))
					{
						// An S_OK return lets the caller know that the deletion was successful
						if (hr == S_OK)
						{
						}

						// An S_FALSE return lets the caller know that the deletion failed.
						// The caller should check the returned IPortableDevicePropVariantCollection
						// for a list of object identifiers that failed to be deleted.
						else
						{
							//printf("The object '%ws' failed to be deleted from the device.\n", ObjectID.pwszVal);
						}
					}
					else
					{
						printf("! Failed to delete an object from the device, hr = 0x%lx\n", hr);
					}
				}
				else
				{
					printf("! Failed to delete an object from the device because we could no add the object identifier string to the IPortableDevicePropVariantCollection, hr = 0x%lx\n", hr);
				}

				// Free any allocated values in the PROPVARIANT before exiting
				PropVariantClear(&ObjectID);
			}
			else
			{
				printf("! Failed to delete an object from the device because we were returned a NULL IPortableDevicePropVariantCollection interface pointer, hr = 0x%lx\n", hr);
			}
		}
		else
		{
			printf("! Failed to CoCreateInstance CLSID_PortableDevicePropVariantCollection, hr = 0x%lx\n", hr);
		}
	}
}