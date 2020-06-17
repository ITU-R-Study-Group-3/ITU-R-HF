// Operating system preprocessor directives *********************************************************
#ifdef _WIN32
	#define DLLEXPORT __declspec(dllexport)
#endif
#ifdef __linux__
	#define DLLEXPORT
#endif
#ifdef __APPLE__
	#define DLLEXPORT
#endif

// External Preprocessors Dependancies
// The path structure constains the P372 noise structure
#include "Noise.h"
// End External Preprocessor Dependancies

// Start P372.DLL typedef ******************************************************
#ifdef _WIN32
#include <Windows.h>
// P372Version() & P372CompileTime()
typedef const char* (__cdecl* cP372Info)();
// AllocateNoiseMemory() & FreeNoiseMemory()
typedef int(__cdecl* iNoiseMemory)(struct NoiseParams* noiseP);
// Noise()
typedef int(__cdecl* iNoise)(struct NoiseParams* noiseP, int hour, double lng, double lat, double frequency);
// ReadFamDud()
typedef int(__cdecl* iReadFamDud)(struct NoiseParams* noiseP, const char* DataFilePath, int month);
// InitializeNoise()
typedef void(__cdecl* vInitializeNoise)(struct NoiseParams* noiseP);
#endif
// End P372.DLL typedef ********************************************************

#ifdef _WIN32
	HINSTANCE hLib;
	cP372Info dllP372Version;
	cP372Info dllP372CompileTime;
	iNoise dllNoise;
	iNoiseMemory dllAllocateNoiseMemory;
	iNoiseMemory dllFreeNoiseMemory;
	iReadFamDud dllReadFamDud;
	vInitializeNoise dllInitializeNoise;
#elif __linux__ || __APPLE__
	#include <dlfcn.h>
	void* hLib;
	char* (*dllP372Version)();
	char* (*dllP372CompileTime)();
	int(*dllNoise)(struct NoiseParams*, int, double, double, double);
	int(*dllAllocateNoiseMemory)(struct NoiseParams*);
	int(*dllFreeNoiseMemory)(struct NoiseParams*);
	int(*dllReadFamDud)(struct NoiseParams*, const char*, int);
	void(*dllInitializeNoise)(struct NoiseParams*);
#endif
// End operating system preprocessor *******************************************

// End P372.DLL typedef ******************************************************

// ITURNoise Return codes
//  Error codes
#define RTN_ERRYEAR						100	// ERROR: Invalid Input Year
#define RTN_ERRMONTH					101 // ERROR: Invalid Input Month
#define RTN_ERRHOUR						102 // ERROR: Invalid Input Hour
#define RTN_ERRFREQ						103 // ERROR: Invalid Input Frequency
#define RTN_ERRDATAFILEPATH				104 // ERROR: Invalid Input Data File Path
#define RTN_ERRCOMMANDLINEARGS			105 // ERROR: Insufficient Number of Command Line Arguments

// Successfull return codes 
#define RTN_ITURNOISEOK		90
// End ITURNoise return codes