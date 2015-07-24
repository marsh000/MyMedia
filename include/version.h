#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "11";
	static const char MONTH[] = "07";
	static const char YEAR[] = "2015";
	static const char UBUNTU_VERSION_STYLE[] =  "15.07";
	
	//Software Status
	static const char STATUS[] =  "Beta";
	static const char STATUS_SHORT[] =  "b";
	
	//Standard Version Type
	static const long MAJOR  = 0;
	static const long MINOR  = 4;
	static const long BUILD  = 114;
	static const long REVISION  = 265;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 437;
	#define RC_FILEVERSION 0,4,114,265
	#define RC_FILEVERSION_STRING "0, 4, 114, 265\0"
	static const char FULLVERSION_STRING [] = "0.4.114.265";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 13;
	

}
#endif //VERSION_H
