//
//  main.c
//  ixaya-swf-demos
//
//  Created by Marcos Ortega on 24/02/14.
//  Copyright (c) 2014 NIBSA. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>		//malloc, free
#include "testMemMap.h"
//Ixaya-swf depends on a method to uncompress ZLIB data.
//For this demo you can download EasyZLIB ("easyzlib.h" and "easyzlib.c")
//and copy them into "ixaya-swf/src/c/" folder. http://www.firstobject.com/easy-zlib-c++-xml-compression.htm
#include "easyzlib.h" //Read the note above.

// Custom memory allocation for this demo,
// for detecting memory-leaks using a STNB_MemMap.
STNB_MemMap memmap;
#define IXA_MALLOC(POINTER_DEST, POINTER_TYPE, SIZE_BYTES, STR_HINT) \
	{ \
		POINTER_DEST = (POINTER_TYPE*)malloc(SIZE_BYTES); \
		nbMemmapRegister(&memmap, POINTER_DEST, SIZE_BYTES, STR_HINT); \
	}
#define IXA_FREE(POINTER)	\
	{ \
		free(POINTER); \
		nbMemmapUnregister(&memmap, POINTER); \
	}
#include "ixaya-swf.h"
#include "ixaya-swf.c"
//In this demo, we include then "source" file to implement customized memory allocation.
//As you can see, the "ixaya-swf.c" file is not part of the project tree.

IxaBOOL uncompressSwfZlib(IxaUI8* ptrDest, const IxaUI32 destLen, const IxaUI8* ptrSrc, const IxaUI32 srcLen);

int main(int argc, const char * argv[]) {
	const char* defaultSwfPath = "/Users/mortegam/Desktop/NIBSA_proyectos/DodoTown/Recursos/JuegoEducaVial.swf"; //"/Your/path/to/swf/myfile.swf"; //You cant hardcode a test SWF file here.
	//const char* defaultSwfPath = "X:\\NIBSA_proyectos\\DodoTown\\Recursos\\JuegoEducaVial.swf";
	const char* explicitSwfPath = NULL;
	const char* swfPath = NULL;
	int i;
	nbMemmapInit(&memmap);
	//Read prameter "-swf"
	for(i=0; i<argc; i++){
		if(argv[i][0]=='-')
		if(argv[i][1]=='s')
		if(argv[i][2]=='w')
		if(argv[i][3]=='f')
		if(argv[i][4]=='\0')
			if((i+1)<argc){
				explicitSwfPath = argv[i+1];
				i++;
			}
	}
	if(explicitSwfPath==NULL){
		printf("IXAYA demo: you can specify a SWF file with params '-swf <pathToSwf>'\n");
		swfPath = defaultSwfPath;
	} else {
		swfPath = explicitSwfPath;
	}
	if(swfPath==NULL){
		printf("IXAYA demo: nothing to do (scpecify a swf file).\n");
	} else {
		//Read SWF
		STIXA_SwfFile swfFile;
		ixaSwfFileInit(&swfFile);
		printf("SWF FILE, loading: '%s'.\n", swfPath);
		printf("... please wait ...\n");
		if(!ixaSwfLoadFile(&swfFile, swfPath, uncompressSwfZlib)){
			printf("ERROR, loading SWF FILE: '%s'\n", swfPath);
		} else {
			printf("SWF FILE, successfully loaded: '%s'\n", swfPath);
		}
		ixaSwfFileFinalize(&swfFile);
	}
	//Analize memory state
	if(memmap.currCountAllocationsActive==0){
		printf("Ixaya: no memory leaking detected :)\n");
	} else {
		printf("--------------\n");
		printf("---- WARNING, IXAYA MEMORY-LEAK! :(\n");
		printf("--------------\n");
	}
	nbMemmapPrintActive(&memmap);
	nbMemmapFinalize(&memmap);
	//
	{
		char c;
		printf("Press ENTER to exit.");
		scanf("%c", &c);
	}
    return 0;
}

IxaBOOL uncompressSwfZlib(IxaUI8* ptrDest, const IxaUI32 destLen, const IxaUI8* ptrSrc, const IxaUI32 srcLen){
	//Ixaya-swf depends on a method to uncompress ZLIB data.
	//For this demo you can download EasyZLIB ("easyzlib.h" and "easyzlib.c")
	//and copy them into "ixaya-swf/src/c/" folder. http://www.firstobject.com/easy-zlib-c++-xml-compression.htm
	long destLenLng = destLen;
	if(0 == ezuncompress(ptrDest, &destLenLng, ptrSrc, srcLen)){
		return IXA_TRUE;
	}
	return IXA_FALSE;
}
