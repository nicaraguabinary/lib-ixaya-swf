//
//  ixaya-swf.c
//  ixaya
//
//  Created by Marcos Ortega on 24/02/14.
//  Copyright (c) 2014 NIBSA. All rights reserved.
//
//  This entire notice must be retained in this source code.
//  This source code is under LGLP v2.1 Licence.
//
//  This software is provided "as is", with absolutely no warranty expressed
//  or implied. Any use is at your own risk.
//
//  Latest fixes enhancements and documentation at https://github.com/nicaraguabinary/ixaya-swf
//

#include <stdio.h>		//NULL
#include <string.h>		//memcpy, memset
#include <assert.h>		//assert
#include "ixaya-swf.h"

//
// You can custom memory management by defining this MACROS
// and CONSTANTS before this file get included or compiled.
//
// This are the default memory management MACROS and CONSTANTS:
#if !defined(IXA_MALLOC) || !defined(IXA_FREE)
	#include <stdlib.h>		//malloc, free
	#ifndef IXA_MALLOC
		#define IXA_MALLOC(POINTER_DEST, POINTER_TYPE, SIZE_BYTES, STR_HINT)	POINTER_DEST = (POINTER_TYPE*)malloc(SIZE_BYTES);
	#endif
	#ifndef IXA_FREE
		#define IXA_FREE(POINTER)		 free(POINTER);
	#endif
#endif
#ifndef IXA_FIGURE_VERTEXS_SIZE_GROWTH
	#define IXA_FIGURE_VERTEXS_SIZE_GROWTH		1
#endif
#ifndef IXA_SHAPE_FIGURES_SIZE_GROWTH
	#define IXA_SHAPE_FIGURES_SIZE_GROWTH		1
#endif
#ifndef IXA_SPRITE_ELEMS_SIZE_GROWTH
	#define IXA_SPRITE_ELEMS_SIZE_GROWTH		1
#endif
#ifndef IXA_SPRITE_ELEMSPROPS_SIZE_GROWTH
	#define IXA_SPRITE_ELEMSPROPS_SIZE_GROWTH	1
#endif
#ifndef IXA_SPRITE_SOUNDEVENTS_SIZE_GROWTH
	#define IXA_SPRITE_SOUNDEVENTS_SIZE_GROWTH	1
#endif
#ifndef IXA_SWF_DICC_SIZE_GROWTH
	#define IXA_SWF_DICC_SIZE_GROWTH			1
#endif

//-------------------------------
//-- BASIC DEFINITIONS
//-------------------------------

#define IXA_ASSERT(EVAL)	assert(EVAL);

#if defined(__ANDROID__)
	#include <android/log.h>
	//#pragma message("COMPILANDO PARA ANDROID")
	//Requiere #include <android/log.h> (en el encabezado precompilado)
	#define PRINTF_INFO(STR_FMT, ...)		__android_log_print(ANDROID_LOG_INFO, "Ixaya-swf", STR_FMT, ##__VA_ARGS__)
	#define PRINTF_ERROR(STR_FMT, ...)		__android_log_print(ANDROID_LOG_ERROR, "Ixaya-swf", "ERROR, "STR_FMT, ##__VA_ARGS__)
	#define PRINTF_WARNING(STR_FMT, ...)	__android_log_print(ANDROID_LOG_WARN, "Ixaya-swf", "WARNING, "STR_FMT, ##__VA_ARGS__)
#elif defined(__APPLE__) || (defined(__APPLE__) && defined(__MACH__))
	//#pragma message("COMPILANDO PARA iOS/Mac")
	#define PRINTF_INFO(STR_FMT, ...)		printf("Ixa, " STR_FMT, ##__VA_ARGS__)
	#define PRINTF_ERROR(STR_FMT, ...)		printf("Ixa ERROR, " STR_FMT, ##__VA_ARGS__)
	#define PRINTF_WARNING(STR_FMT, ...)	printf("Ixa WARNING, " STR_FMT, ##__VA_ARGS__)
#else
	//#pragma message("(SE ASUME) COMPILANDO PARA BLACKBERRY")
	#define PRINTF_INFO(STR_FMT, ...)		fprintf(stdout, "Ixa, " STR_FMT, ##__VA_ARGS__); fflush(stdout)
	#define PRINTF_ERROR(STR_FMT, ...)		fprintf(stderr, "Ixa ERROR, " STR_FMT, ##__VA_ARGS__); fflush(stderr)
	#define PRINTF_WARNING(STR_FMT, ...)	fprintf(stdout, "Ixa WARNING, " STR_FMT, ##__VA_ARGS__); fflush(stdout)
#endif

//+++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++
//++ Private Header
//+++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++

//Stream reader
typedef struct {
	const IxaBYTE*	_flujo;
	IxaSI32		_posicionEnFlujo;
	IxaSI32		_bitsRestantes;
	IxaSI32		_conteoTotalBytesConsumidos;
	IxaSI32		_conteoTmpBytesConsumidos;
} STIXA_Reader;

void			__ixaReaderInit(STIXA_Reader* rdr, const IxaBYTE* stream);
void			__ixaReaderFinalize(STIXA_Reader* rdr);
void			__ixaReaderSetStream(STIXA_Reader* rdr, const IxaBYTE* stream);
void			__ixaReaderConsumeBytes(STIXA_Reader* rdr, IxaSI32 bytes);
const IxaBYTE*	__ixaReaderCurrentByte(STIXA_Reader* rdr);
void			__ixaReaderPositionInByteAligned(STIXA_Reader* rdr);
void			__ixaReaderRestartBytesReadedCounter(STIXA_Reader* rdr);
void			__ixaReaderIgnoreBytes(STIXA_Reader* rdr, IxaSI32 bytes);
void			__ixaReaderPeekBytes(STIXA_Reader* rdr, void* dest, IxaSI32 bytes);
void			__ixaReaderGetBytes(STIXA_Reader* rdr, void* dest, IxaSI32 bytes);
IxaUI32			__ixaReaderGetBits(STIXA_Reader* rdr, IxaUI8 bits);
IxaUI32			__ixaReaderGetBitsUnsigned(STIXA_Reader* rdr, IxaUI8 bits);
IxaSI32			__ixaReaderGetBitsSigned(STIXA_Reader* rdr, IxaUI8 bits);
float			__ixaReaderGetBitsFixed16(STIXA_Reader* rdr, IxaUI8 bits);
IxaUI8			__ixaReaderGetUI8(STIXA_Reader* rdr);
IxaUI16			__ixaReaderGetUI16(STIXA_Reader* rdr);
IxaUI32			__ixaReaderGetUI32(STIXA_Reader* rdr);
IxaUI32			__ixaReaderGetUVariable(STIXA_Reader* rdr, IxaUI8 bytes);
IxaUI16			__ixaReaderGetUI16Inverted(STIXA_Reader* rdr);
IxaUI32			__ixaReaderGetUI32Inverted(STIXA_Reader* rdr);
IxaUI32			__ixaReaderGetUVariableInverted(STIXA_Reader* rdr, IxaUI8 bytes);
IxaSI8			__ixaReaderGetIxaSI8(STIXA_Reader* rdr);
IxaSI16			__ixaReaderGetSI16(STIXA_Reader* rdr);
IxaSI32			__ixaReaderGetSI32(STIXA_Reader* rdr);
IxaSI16			__ixaReaderGetSI16Inverted(STIXA_Reader* rdr);
IxaSI32			__ixaReaderGetSI32Inverted(STIXA_Reader* rdr);
float			__ixaReaderGetFixed8(STIXA_Reader* rdr);
float			__ixaReaderGetFixed16(STIXA_Reader* rdr);
float			__ixaReaderGetF2Dot14(STIXA_Reader* rdr);
float			__ixaReaderGetF2Dot14Inverted(STIXA_Reader* rdr);

	
char*			__ixaLoadSTRING(STIXA_Reader* rdr);
STIXA_Rect		__ixaLoadRECT(STIXA_Reader* rdr);
STIXA_Kerning	__ixaLoadKERNING(STIXA_Reader* rdr, const IxaBOOL wideCodes);
STIXA_Matrix	__ixaLoadMATRIX(STIXA_Reader* rdr);
STIXA_Gradient	__ixaLoadGRADIENT(STIXA_Reader* rdr, const ENIXA_Shape formType, const IxaBOOL esFocal);
STIXA_StyleLine	__ixaLoadLINESTYLE(STIXA_Reader* rdr, const ENIXA_Shape formType, const IxaBOOL esLineStyle2);
STIXA_StyleFill	__ixaLoadFILLSTYLE(STIXA_Reader* rdr, const ENIXA_Shape formType);
STIXA_CXForm	__ixaLoadCXFORM(STIXA_Reader* rdr, const IxaBOOL tieneAlpha);
IxaUI32			__ixaLoadCLIPEVENTFLAGS(STIXA_Reader* rdr, const IxaUI32 versionFlash);
STIXA_SoundEnv	__ixaLoadSOUNDENVELOPE(STIXA_Reader* rdr);
STIXA_SoundInfo	__ixaLoadSOUNDINFO(STIXA_Reader* rdr);
void			__ixaLoadSHAPE(STIXA_SwfFile* swf, STIXA_Reader* rdr, STIXA_Shape* dstShape, const ENIXA_Shape formType, const IxaBOOL withStyle);

const char*		__ixaTagName(const ENIXA_TagID tagId);
IxaBOOL			__ixaSwfParseTags(STIXA_SwfFile* swf, STIXA_Reader* rdr, STIXA_Sprite* sprite, PTRIXA_zlibUncompress funcUncompress, const IxaUI16 currIdentLevel); //Identation
IxaBOOL			__ixaSwfDictLoadJPEGSharedTable(STIXA_SwfFile* swf, STIXA_Reader* rdr, const IxaUI32 bytesTag);
IxaBOOL			__ixaSwfDictLoadFile(STIXA_SwfFile* swf, STIXA_Reader* rdr, ENIXA_TagID tipoTag, IxaSI32 bytesTag);
IxaBOOL			__ixaSwfDictLoadBitmap(STIXA_SwfFile* swf, STIXA_Reader* rdr, ENIXA_TagID tipoTag, IxaSI32 bytesTag, PTRIXA_zlibUncompress funcUncompress);
IxaBOOL			__ixaSwfDictLoadShape(STIXA_SwfFile* swf, STIXA_Reader* rdr, const ENIXA_TagID tagType, const IxaSI32 bytesTag);
IxaBOOL			__ixaSwfDictLoadFont(STIXA_SwfFile* swf, STIXA_Reader* rdr, const ENIXA_TagID tagType, const IxaSI32 bytesTag);
IxaBOOL			__ixaSwfDictLoadSound(STIXA_SwfFile* swf, STIXA_Reader* rdr, const ENIXA_TagID tagType, const IxaSI32 bytesTag);
IxaBOOL			__ixaSwfFramePlaceObject(STIXA_SwfFile* swf, STIXA_Reader* rdr, STIXA_Sprite* sprite, const ENIXA_TagID tagType, const IxaSI32 bytesTag);
IxaBOOL			__ixaSwfFrameRemoveObject(STIXA_SwfFile* swf, STIXA_Reader* rdr, STIXA_Sprite* sprite, const ENIXA_TagID tagType, const IxaSI32 bytesTag);

//+++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++
//++ Code
//+++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++
	
//-------------------------------
//-- ARRAYS
//-------------------------------
void ixaArrayResize(void** pArr, IxaUI16* pArrUse, IxaUI16* pArrSize, const size_t pItemSize, const IxaUI16 pArrGrowth){
	void* newArr;
	//
	IXA_ASSERT(pArr!=NULL)
	//IXA_ASSERT(pArrUse < pArrSize) //The variable "use" should be declared before "size"
	//IXA_ASSERT((pArrUse+2)==pArrSize || (pArrUse+4)==pArrSize) //The variable "use" should be declared before "size"
	IXA_ASSERT((*pArrUse)<=(*pArrSize))
	IXA_ASSERT(pItemSize>0)
	IXA_ASSERT(pArrGrowth>0)
	IXA_ASSERT(pArrGrowth <= (0xFFFF - *pArrSize)) //Error when passing the IxaUI16 range
	//
	*pArrSize	+= pArrGrowth;
	IXA_MALLOC(newArr, void, pItemSize * (*pArrSize), "newArr")
	if(*pArr !=NULL){
		if((*pArrUse)!=0) memcpy(newArr, *pArr, pItemSize * (*pArrUse));
		IXA_FREE(*pArr);
	}
	*pArr		= newArr;
}

//-------------------------------
//-- STRINGS
//-------------------------------

IxaUI32 ixaStringLenght(const char* str){
	IxaUI32 r = 0; while(str[r]!='\0') r++;
	return r;
}

//-------------------------------
//-- BITMAPS
//-------------------------------

ENIXA_FileType ixaBitmapFileType(const IxaBYTE* bitmapData, IxaUI32* outBytesToIgnore){
	ENIXA_FileType tipoArchivo = ENIXA_FileTypeJPEG;
	IxaUI32 bytesToIgnore = 0;
	if(bitmapData[0] == 0x89 && bitmapData[1] == 0x50 && bitmapData[2] == 0x4E && bitmapData[3] == 0x47 && bitmapData[4] == 0x0D && bitmapData[5] == 0x0A && bitmapData[6] == 0x1A && bitmapData[7] == 0x0A) {
		tipoArchivo = ENIXA_FileTypePNG;
	} else if(bitmapData[0] == 0x47 && bitmapData[1] == 0x49 && bitmapData[2] == 0x46 && bitmapData[3] == 0x38 && bitmapData[4] == 0x39 && bitmapData[5] == 0x61) {
		tipoArchivo = ENIXA_FileTypeGIF;
	} else {
		tipoArchivo = ENIXA_FileTypeJPEG;
		if(bitmapData[0] == 0xFF && bitmapData[1] == 0xD9 && bitmapData[2] == 0xFF && bitmapData[3] == 0xD8 && bitmapData[4] == 0xFF && bitmapData[5] == 0xD8){
			//el SOI es incorrecto (ignorar los primeros cuatros bytes) alguno SWF previos a la verison 8
			bytesToIgnore = 4;
		} else if(bitmapData[0] == 0xFF && bitmapData[1] == 0xD8) {
			//el SOI JPEG es correcto
		} else {
			//ERROR?
		}
	}
	//
	if(outBytesToIgnore!=NULL) *outBytesToIgnore = bytesToIgnore;
	return tipoArchivo;
}

STIXA_Size ixaBitmapSize(const IxaBYTE* bitmapData, const IxaUI32 sizeBytes, const ENIXA_FileType fileType){
	STIXA_Size size; size.width = size.height = 0;
	switch(fileType){
		case ENIXA_FileTypeJPEG_SIN_TABLA:
		case ENIXA_FileTypeJPEG:
		case ENIXA_FileTypeJPEG_CON_ALPHA_ANEXO:
			size = ixaBitmapSizeJPEG(bitmapData, sizeBytes);
			break;
		case ENIXA_FileTypeGIF:
			size = ixaBitmapSizeGIF(bitmapData, sizeBytes);
			break;
		case ENIXA_FileTypePNG:
			size = ixaBitmapSizePNG(bitmapData, sizeBytes);
			break;
		default:
			break;
	}
	return size;
}

STIXA_Size ixaBitmapSizeJPEG(const IxaBYTE* bitmapData, const IxaUI32 sizeBytes){
	STIXA_Size size; size.width = size.height = 0;
	//Interpretar los marcadores JPEG
	if(bitmapData[0]!=0xFF || bitmapData[1]!=M_SOI){
		PRINTF_INFO("Los datos no son de un JPEG, el primer marcador no es un SOI\n");
	} else {
		IxaUI32 posicion = 2;
		IxaBOOL continuar = IXA_TRUE;
		IxaBYTE c1, c2; IxaUI32 tamanoBloque;
		while(continuar && posicion < sizeBytes){
			IxaBYTE esteMarcador;
			//posicionar en siguiente marcador
			while(bitmapData[posicion++]!=0xFF && posicion < sizeBytes);
			//ignorar los 0xFF continuos (son utilizados como relleno en el JPEG)
			do {
				esteMarcador = bitmapData[posicion++];
			} while(esteMarcador == 0xFF && posicion < sizeBytes);
			//procesar marcador
			if(posicion < sizeBytes){
				switch (esteMarcador) {
					case M_SOF0:		/* Baseline */
					case M_SOF1:		/* Extended sequential, Huffman */
					case M_SOF2:		/* Progressive, Huffman */
					case M_SOF3:		/* Lossless, Huffman */
					case M_SOF5:		/* Differential sequential, Huffman */
					case M_SOF6:		/* Differential progressive, Huffman */
					case M_SOF7:		/* Differential lossless, Huffman */
					case M_SOF9:		/* Extended sequential, arithmetic */
					case M_SOF10:		/* Progressive, arithmetic */
					case M_SOF11:		/* Lossless, arithmetic */
					case M_SOF13:		/* Differential sequential, arithmetic */
					case M_SOF14:		/* Differential progressive, arithmetic */
					case M_SOF15:		/* Differential lossless, arithmetic */
						c1 = bitmapData[posicion++];
						c2 = bitmapData[posicion++];
						tamanoBloque = (((unsigned int) c1) << 8) + ((unsigned int) c2);
						if(tamanoBloque<2){
							PRINTF_ERROR("en el tamano del bloque JPG\n");
						} else {
							IxaBYTE alto1, alto2, ancho1, ancho2; IxaUI32 alto, ancho;
							/*precision 	= datosJPEG[posicion++];*/ posicion++;
							alto1		= bitmapData[posicion++];
							alto2		= bitmapData[posicion++];
							ancho1		= bitmapData[posicion++];
							ancho2		= bitmapData[posicion++];
							/*BYTE componentes= datosJPEG[posicion++];*/ posicion++;
							alto		= (((IxaUI32) alto1) << 8) + ((IxaUI32) alto2);
							ancho		= (((IxaUI32) ancho1) << 8) + ((IxaUI32) ancho2);
							size.height	= alto;
							size.width	= ancho;
							//PRINTF_INFO("Marcador JPEG-SOF, precision(%d), ancho(%d), alto(%d), componentes(%d)\n", precision, ancho, alto, componentes);
						}
						continuar = IXA_FALSE;
						break;
					case M_SOS:			/* stop before hitting compressed data */
					case M_EOI:			/* in case it's a tables-only JPEG stream */
						continuar = IXA_FALSE;
						break;
					default:			/* asumir que tiene un parametro 'leght' e ignorar */
						c1 = bitmapData[posicion++];
						c2 = bitmapData[posicion++];
						tamanoBloque = (((unsigned int) c1) << 8) + ((unsigned int) c2);
						if(tamanoBloque<2){
							PRINTF_ERROR("en el tamano del bloque JPG\n");
							continuar = IXA_FALSE;
						} else {
							posicion += (tamanoBloque - 2); //ya incluye el tamano
						}
						break;
				}
			}
		}
	}
	return size;
}

STIXA_Size ixaBitmapSizePNG(const IxaBYTE* bitmapData, const IxaUI32 sizeBytes){
	STIXA_Size size; size.width = size.height = 0;
	if(bitmapData[0] == 0x89 && bitmapData[1] == 0x50 && bitmapData[2] == 0x4E && bitmapData[3] == 0x47 && bitmapData[4] == 0x0D && bitmapData[5] == 0x0A && bitmapData[6] == 0x1A && bitmapData[7] == 0x0A){
		STIXA_PngIHDR* chunckIHDR = (STIXA_PngIHDR*) &(bitmapData[12]); //Los primeros 8 bytes son encabezado, los siguientes 4 son el tamaño del CHUNCK.
		if(chunckIHDR->tipoChunk[0] == 'I' && chunckIHDR->tipoChunk[1] == 'H' && chunckIHDR->tipoChunk[2] == 'D' && chunckIHDR->tipoChunk[3] == 'R'){
			IxaBYTE* arrOrig; IxaBYTE* arrInvr;
			arrOrig 		= (IxaBYTE*)&chunckIHDR->ancho;
			arrInvr 		= (IxaBYTE*)&size.width;
			arrInvr[0]		= arrOrig[3];
			arrInvr[1]		= arrOrig[2];
			arrInvr[2]		= arrOrig[1];
			arrInvr[3]		= arrOrig[0];
			//
			arrOrig 		= (IxaBYTE*)&chunckIHDR->alto;
			arrInvr 		= (IxaBYTE*)&size.height;
			arrInvr[0]		= arrOrig[3];
			arrInvr[1]		= arrOrig[2];
			arrInvr[2]		= arrOrig[1];
			arrInvr[3]		= arrOrig[0];
		}
	}
	return size;
}

STIXA_Size ixaBitmapSizeGIF(const IxaBYTE* bitmapData, const IxaUI32 sizeBytes){
	STIXA_Size size; size.width = size.height = 0;
	if(bitmapData[0] == 0x47 && bitmapData[1] == 0x49 && bitmapData[2] == 0x46 && bitmapData[3] == 0x38 && bitmapData[4] == 0x39 && bitmapData[5] == 0x61){
		IxaBYTE ancho1	= bitmapData[6];
		IxaBYTE ancho2	= bitmapData[7];
		IxaBYTE alto1	= bitmapData[8];
		IxaBYTE alto2 	= bitmapData[9];
		size.width 	= (((IxaUI32)ancho2)<<8) + ancho1;
		size.height	= (((IxaUI32)alto2)<<8) + alto1;
	}
	return size;
}


//-------------------------------
//-- READER
//-------------------------------

void __ixaReaderInit(STIXA_Reader* rdr, const IxaBYTE* stream){
	rdr->_flujo							= stream;
	rdr->_posicionEnFlujo				= 0;
	rdr->_bitsRestantes					= 0;
	rdr->_conteoTotalBytesConsumidos	= 0;
	rdr->_conteoTmpBytesConsumidos 		= 0;
}

void __ixaReaderFinalize(STIXA_Reader* rdr){
	rdr->_flujo							= NULL;
	rdr->_posicionEnFlujo				= 0;
	rdr->_bitsRestantes					= 0;
	rdr->_conteoTotalBytesConsumidos	= 0;
	rdr->_conteoTmpBytesConsumidos 		= 0;
}

void __ixaReaderSetStream(STIXA_Reader* rdr, const IxaBYTE* stream){
	rdr->_flujo							= stream;
	rdr->_posicionEnFlujo				= 0;
	rdr->_bitsRestantes					= 0;
	rdr->_conteoTotalBytesConsumidos	= 0;
	rdr->_conteoTmpBytesConsumidos 		= 0;
}

void __ixaReaderConsumeBytes(STIXA_Reader* rdr, IxaSI32 bytes){
	IXA_ASSERT(rdr->_flujo!=NULL)
	if(bytes!=0){
		rdr->_posicionEnFlujo			+= bytes;
		rdr->_bitsRestantes				= 0;
		rdr->_conteoTmpBytesConsumidos	+= bytes;
		rdr->_conteoTotalBytesConsumidos += bytes;
	}
}

const IxaBYTE* __ixaReaderCurrentByte(STIXA_Reader* rdr){
	IXA_ASSERT(rdr->_flujo!=NULL)
	return &(rdr->_flujo[rdr->_posicionEnFlujo]);
}

void __ixaReaderPositionInByteAligned(STIXA_Reader* rdr){
	if(rdr->_bitsRestantes!=0) __ixaReaderConsumeBytes(rdr, 1);
}

void __ixaReaderRestartBytesReadedCounter(STIXA_Reader* rdr){
	//PENDIENTE
}

void __ixaReaderIgnoreBytes(STIXA_Reader* rdr, IxaSI32 bytes){
	__ixaReaderConsumeBytes(rdr, bytes);
}

void __ixaReaderPeekBytes(STIXA_Reader* rdr, void* dest, IxaSI32 bytes){
	IXA_ASSERT(rdr->_flujo!=NULL)
	IXA_ASSERT(bytes>=0)
	if(bytes>0){
		IxaSI32 i; IxaBYTE* destByte = (IxaBYTE*)dest;
		//fastest: memcpy or for?
		for(i=0; i<bytes; i++) destByte[i] = rdr->_flujo[rdr->_posicionEnFlujo + i];
	}
}

void __ixaReaderGetBytes(STIXA_Reader* rdr, void* dest, IxaSI32 bytes){
	IXA_ASSERT(rdr->_flujo!=NULL)
	IXA_ASSERT(bytes>=0)
	if(bytes>0){
		IxaSI32 i; IxaBYTE* destByte = (IxaBYTE*)dest;
		//si sobran bits del byte anterior, ignorarlos y pasar al siguiente bytes completo
		if(rdr->_bitsRestantes!=0) __ixaReaderConsumeBytes(rdr, 1);
		//fastest: memcpy or for?
		for(i=0; i<bytes; i++) destByte[i] = rdr->_flujo[rdr->_posicionEnFlujo + i];
		//consume
		__ixaReaderConsumeBytes(rdr, bytes);
	}
}

IxaUI32 __ixaReaderGetBits(STIXA_Reader* rdr, IxaUI8 bits){
	IxaUI32 valorBits = 0;
	IXA_ASSERT(rdr->_flujo!=NULL)
	if(bits>0){
		IxaSI32 bitsIgnorarIzq, bitsLeer, bitsSobrantesDer, bytesLeer; IxaUI8 valor[4];
		//determinar cantidades en bits
		bitsIgnorarIzq 		= (rdr->_bitsRestantes==0? 0 : 8 - rdr->_bitsRestantes);
		bitsLeer			= bits;
		bitsSobrantesDer	= 0; while(((bitsIgnorarIzq + bitsLeer + bitsSobrantesDer) % 8) != 0) bitsSobrantesDer++;
		//determinar cantidades en bytes
		bytesLeer			= (bitsIgnorarIzq + bitsLeer + bitsSobrantesDer) / 8;
		//PRINTF_INFO("--------\n");
		//PRINTF_INFO("%d bytes a leer (bits = %d bits ignorar + %d bits datos + %d bits sobrantes)\n", bytesLeer, bitsIgnorarIzq, bitsLeer, bitsSobrantesDer);
		valor[0] = 0; valor[1] = 0; valor[2] = 0; valor[3] = 0;
		//leer bytes sin consumirlos
		__ixaReaderPeekBytes(rdr, &valor, bytesLeer);
		//PRINTF_INFO("Leidos (%d, %d, %d, %d)\n", valor[0], valor[1], valor[2], valor[3]);
		valor[0] = (valor[0] << bitsIgnorarIzq);
		valor[0] = (valor[0] >> bitsIgnorarIzq); 		//trunca la izquierda no deseada
		valor[bytesLeer-1] = (valor[bytesLeer-1] >> bitsSobrantesDer);
		valor[bytesLeer-1] = (valor[bytesLeer-1] << bitsSobrantesDer);	//trunca la derecha no deseada
		//PRINTF_INFO("Trunca (%d, %d, %d, %d)\n", valor[0], valor[1], valor[2], valor[3]);
		if(bytesLeer==4){
			valorBits = ((valor[0] << (24-bitsSobrantesDer)) | (valor[1] << (16-bitsSobrantesDer)) | (valor[2] << (8-bitsSobrantesDer)) | (valor[3] >> (bitsSobrantesDer)));
		} else if(bytesLeer==3){
			valorBits = ((valor[0] << (16-bitsSobrantesDer)) | (valor[1] << (8-bitsSobrantesDer)) | (valor[2] >> (bitsSobrantesDer)));
		} else if(bytesLeer==2){
			valorBits = ((valor[0] << (8-bitsSobrantesDer)) | (valor[1] >> (bitsSobrantesDer)));
		} else if(bytesLeer==1){
			valorBits = ((valor[0] >> (bitsSobrantesDer)));
		} else {
			valorBits = 0;
		}
		//PRINTF_INFO("valorBits (%d)\n", valorBits);
		//consumir bytes
		__ixaReaderConsumeBytes(rdr, bitsSobrantesDer==0 ? bytesLeer : (bytesLeer-1));
		rdr->_bitsRestantes = bitsSobrantesDer;
	}
	return valorBits;
}

IxaUI32 __ixaReaderGetBitsUnsigned(STIXA_Reader* rdr, IxaUI8 bits){
	IXA_ASSERT(rdr->_flujo!=NULL)
	return __ixaReaderGetBits(rdr, bits);
}

IxaSI32 __ixaReaderGetBitsSigned(STIXA_Reader* rdr, IxaUI8 bits){
	IxaUI32 valor = __ixaReaderGetBits(rdr, bits);
	if(bits>1){
		//crear mascara de signo
		IxaSI32 pos;
		IxaUI32 mascaraBitSigno = 1;
		for(pos=1; pos<bits; pos++){
			mascaraBitSigno *= 2;
		}
		//es negativo, activar todos los bits a la izquierda del signo
		if((mascaraBitSigno & valor) != 0){
			IxaUI32 mascaraBitsIzq = 0, unBit = 1;
			for(pos=1; pos<32; pos++){
				unBit *= 2;
				if(pos>=bits) mascaraBitsIzq += unBit;
			}
			valor = (valor | mascaraBitsIzq);
		}
	}
	return *((IxaSI32*)&valor);
}

float __ixaReaderGetBitsFixed16(STIXA_Reader* rdr, IxaUI8 bits){
	IxaSI32 intValue	= __ixaReaderGetBitsSigned(rdr, bits);
	return (float)intValue / (float)0xFFFF;
}

IxaUI8 __ixaReaderGetUI8(STIXA_Reader* rdr){
	IxaUI8 r; __ixaReaderGetBytes(rdr, &r, sizeof(r));
	return r;
}

IxaUI16 __ixaReaderGetUI16(STIXA_Reader* rdr){
	IxaUI16 r; __ixaReaderGetBytes(rdr, &r, sizeof(r));
	return r;
}

IxaUI32 __ixaReaderGetUI32(STIXA_Reader* rdr){
	IxaUI32 r; __ixaReaderGetBytes(rdr, &r, sizeof(r));
	return r;
}

IxaUI32 __ixaReaderGetUVariable(STIXA_Reader* rdr, IxaUI8 bytes){
	IxaUI32 r; IxaUI8 iByte; IxaUI8* arrVal;
	IXA_ASSERT(rdr->_flujo!=NULL)
	IXA_ASSERT(bytes==1 || bytes==2 || bytes==3 || bytes==4)
	r			= 0;
	iByte		= 4 - bytes;
	arrVal		= (IxaUI8*)&r;
	do {
		arrVal[iByte++]	= __ixaReaderGetUI8(rdr);
	} while(iByte!=4);
	return r;
}

IxaUI16 __ixaReaderGetUI16Inverted(STIXA_Reader* rdr){
	IxaUI16 r, invertido; IxaBYTE* arrOrig; IxaBYTE* arrInvr;
	__ixaReaderGetBytes(rdr, &r, sizeof(r));
	arrOrig 	= (IxaBYTE*)&r;
	arrInvr 	= (IxaBYTE*)&invertido;
	arrInvr[0]	= arrOrig[1];
	arrInvr[1]	= arrOrig[0];
	return r;
}

IxaUI32 __ixaReaderGetUI32Inverted(STIXA_Reader* rdr){
	IxaUI32 r, invertido; IxaBYTE* arrOrig; IxaBYTE* arrInvr;
	IXA_ASSERT(rdr->_flujo!=NULL)
	__ixaReaderGetBytes(rdr, &r, sizeof(r));
	arrOrig 	= (IxaBYTE*)&r;
	arrInvr 	= (IxaBYTE*)&invertido;
	arrInvr[0]	= arrOrig[3];
	arrInvr[1]	= arrOrig[2];
	arrInvr[2]	= arrOrig[1];
	arrInvr[3]	= arrOrig[0];
	return r;
}

IxaUI32 __ixaReaderGetUVariableInverted(STIXA_Reader* rdr, IxaUI8 bytes){
	IxaUI32 r; IxaUI8 iByte; IxaUI8* arrVal;
	IXA_ASSERT(rdr->_flujo!=NULL)
	IXA_ASSERT(bytes==1 || bytes==2 || bytes==3 || bytes==4)
	r		= 0;
	iByte	= bytes;
	arrVal	= (IxaUI8*)&r;
	do {
		arrVal[--iByte]	= __ixaReaderGetUI8(rdr);
	} while(iByte!=0);
	return r;
}

IxaSI8 __ixaReaderGetIxaSI8(STIXA_Reader* rdr){
	IxaSI8 r; __ixaReaderGetBytes(rdr, &r, sizeof(r));
	return r;
}

IxaSI16 __ixaReaderGetSI16(STIXA_Reader* rdr){
	IxaSI16 r; __ixaReaderGetBytes(rdr, &r, sizeof(r));
	return r;
}

IxaSI32 __ixaReaderGetSI32(STIXA_Reader* rdr){
	IxaSI32 r; __ixaReaderGetBytes(rdr, &r, sizeof(r));
	return r;
}

IxaSI16 __ixaReaderGetSI16Inverted(STIXA_Reader* rdr){
	IxaSI16 r, invertido; IxaBYTE* arrOrig; IxaBYTE* arrInvr;
	IXA_ASSERT(rdr->_flujo!=NULL)
	__ixaReaderGetBytes(rdr, &r, sizeof(r));
	arrOrig 	= (IxaBYTE*)&r;
	arrInvr 	= (IxaBYTE*)&invertido;
	arrInvr[0]	= arrOrig[1];
	arrInvr[1]	= arrOrig[0];
	return r;
}

IxaSI32 __ixaReaderGetSI32Inverted(STIXA_Reader* rdr){
	IxaSI32 r, invertido; IxaBYTE* arrOrig; IxaBYTE* arrInvr;
	__ixaReaderGetBytes(rdr, &r, sizeof(r));
	arrOrig 	= (IxaBYTE*)&r;
	arrInvr 	= (IxaBYTE*)&invertido;
	arrInvr[0]	= arrOrig[3];
	arrInvr[1]	= arrOrig[2];
	arrInvr[2]	= arrOrig[1];
	arrInvr[3]	= arrOrig[0];
	return r;
}

float __ixaReaderGetFixed8(STIXA_Reader* rdr){
	IxaSI32 intValue	= __ixaReaderGetSI16(rdr);
	return (float)intValue / (float)0xFF;
}

float __ixaReaderGetFixed16(STIXA_Reader* rdr){
	IxaSI32 intValue	= __ixaReaderGetSI32(rdr);
	return (float)intValue / (float)0xFFFF;
}

float __ixaReaderGetF2Dot14(STIXA_Reader* rdr){
	IxaUI16 value; IxaSI8 intValue; IxaUI16 decValue;
	IXA_ASSERT(rdr->_flujo!=NULL)
	value		= __ixaReaderGetUI16(rdr);
	intValue	= (value >> 14);
	decValue	= (value & 0x3FFF);
	if(intValue & 0x2) intValue |= 0xFE; //Repetir el bit de signo si esta activo
	return ((float)intValue + ((float)decValue / 16384.0f));
}

float __ixaReaderGetF2Dot14Inverted(STIXA_Reader* rdr){
	IxaUI16 value; IxaSI8 intValue; IxaUI16 decValue;
	IXA_ASSERT(rdr->_flujo!=NULL)
	value		= __ixaReaderGetUI16Inverted(rdr);
	intValue	= (value >> 14);
	decValue	= (value & 0x3FFF);
	if(intValue & 0x2) intValue |= 0xFE; //Repetir el bit de signo si esta activo
	return ((float)intValue + ((float)decValue / 16384.0f));
}

//-------------------------------
//-- BINARY STTACHED FILES (JPEGs)
//-------------------------------
void ixaFileBinaryInit(STIXA_FileBits* filebin){
	filebin->tipo			= ENIXA_FileTypeBINARIO;
	filebin->puntero		= NULL;
	filebin->tamano			= 0;
	filebin->punteroAnexo	= NULL;
	filebin->tamanoAnexo	= 0;
}

void ixaFileBinaryFinalize(STIXA_FileBits* filebin){
	if(filebin->puntero!=NULL){ IXA_FREE(filebin->puntero); filebin->puntero = NULL; }
	if(filebin->punteroAnexo!=NULL){ IXA_FREE(filebin->punteroAnexo); filebin->punteroAnexo = NULL; }
}

	
//-------------------------------
//-- BITMAPS (lossless)
//-------------------------------

void ixaBitmapInit(STIXA_Bitmap* bitmap){
	bitmap->bitmapformat	= 0;
	bitmap->color			= ENIXA_ColorUnefined;
	bitmap->width			= 0;
	bitmap->height			= 0;
	bitmap->paletteSize		= 0;
	bitmap->bytesPerPixel	= 0;
	bitmap->bytesPerColorPalette = 0;
	bitmap->bytesPerLine	= 0;
	bitmap->uncompressData	= NULL;
	bitmap->bitmapPointer	= NULL;
	bitmap->palettePointer	= NULL;
}
	
void ixaBitmapFinalize(STIXA_Bitmap* bitmap){
	if(bitmap->uncompressData!=NULL){ IXA_FREE(bitmap->uncompressData); bitmap->uncompressData = NULL; }
}

	
//-------------------------------
//-- FIGURES (part of a form)
//-------------------------------
void ixaFigureInit(STIXA_Figure* fig){
	fig->vertArr		= NULL;
	fig->vertArrUse		= 0;
	fig->vertArrSize	= 0;
	fig->xMin = fig->xMax = fig->yMin = fig->yMax = 0;
}

void ixaFigureFinalize(STIXA_Figure* fig){
	if(fig->vertArr!=NULL){ IXA_FREE(fig->vertArr); }
	fig->vertArr		= NULL;
	fig->vertArrUse		= 0;
	fig->vertArrSize	= 0;
	fig->xMin = fig->xMax = fig->yMin = fig->yMax = 0;
}

void ixaFigureVertexAdd(STIXA_Figure* fig, STIXA_FigVertex* vert){
	IXA_ASSERT(fig->vertArrUse <= fig->vertArrSize) //Assert if memory corrupted
	if(fig->vertArrUse == fig->vertArrSize) ixaArrayResize((void**)&fig->vertArr, &fig->vertArrUse, &fig->vertArrSize, sizeof(STIXA_FigVertex), IXA_FIGURE_VERTEXS_SIZE_GROWTH);
	fig->vertArr[fig->vertArrUse++] = *vert;
	IXA_ASSERT(fig->vertArrUse <= fig->vertArrSize) //Assert if memory corrupted
	//Validate bounds
	if(fig->vertArrUse==1){
		fig->xMax = fig->xMin = vert->x;
		fig->yMax = fig->yMin = vert->y;
	} else {
		if(fig->xMax < vert->x) fig->xMax = vert->x;
		if(fig->xMin > vert->x) fig->xMin = vert->x;
		if(fig->yMax < vert->y) fig->yMax = vert->y;
		if(fig->yMin > vert->y) fig->yMin = vert->y;
	}
}

//-------------------------------
//-- SHAPES (graphics objects)
//-------------------------------
void ixaShapeInit(STIXA_Shape* shape){
	shape->figsArr		= NULL;
	shape->figsArrUse	= 0;
	shape->figsArrSize	= 0;
	shape->xMin = shape->xMax = shape->yMin = shape->yMax = 0;
}

void ixaShapeFinalize(STIXA_Shape* shape){
	if(shape->figsArr!=NULL){
		IxaUI16 i; const IxaUI16 use = shape->figsArrUse;
		for(i=0; i<use; i++) ixaFigureFinalize(&shape->figsArr[i]);
		IXA_FREE(shape->figsArr);
		shape->figsArr = NULL;
	}
	shape->figsArr		= NULL;
	shape->figsArrUse	= 0;
	shape->figsArrSize	= 0;
	shape->xMin = shape->xMax = shape->yMin = shape->yMax = 0;
}

void ixaShapeFigureAdd(STIXA_Shape* shape, STIXA_Figure* fig){ //Dont finalize the given figure after adding it to a form
	IXA_ASSERT(shape->figsArrUse <= shape->figsArrSize) //Assert if memory corrupted
	if(shape->figsArrUse == shape->figsArrSize) ixaArrayResize((void**)&shape->figsArr, &shape->figsArrUse, &shape->figsArrSize, sizeof(STIXA_Figure), IXA_SHAPE_FIGURES_SIZE_GROWTH);
	shape->figsArr[shape->figsArrUse++] = *fig;
	IXA_ASSERT(shape->figsArrUse <= shape->figsArrSize) //Assert if memory corrupted
	//Validate bounds
	if(shape->figsArrUse==1){
		shape->xMax = fig->xMax; shape->xMin = fig->xMin;
		shape->yMax = fig->yMax; shape->yMin = fig->yMin;
	} else {
		if(shape->xMax < fig->xMax) shape->xMax = fig->xMax;
		if(shape->xMin > fig->xMin) shape->xMin = fig->xMin;
		if(shape->yMax < fig->yMax) shape->yMax = fig->yMax;
		if(shape->yMin > fig->yMin) shape->yMin = fig->yMin;
	}
}

//-------------------------------
//-- GLYPH DEFINITIONS
//-------------------------------
void ixaGlyphInit(STIXA_Glyph* glyph){
	glyph->code		= 0;
	glyph->advace	= 0;
	ixaShapeInit(&glyph->shape);
}

void ixaGlyphFinalize(STIXA_Glyph* glyph){
	ixaShapeFinalize(&glyph->shape);
}

//-------------------------------
//-- FONT DEFINITIONS
//-------------------------------
void ixaFontInit(STIXA_Font* font){
	font->isBold			= IXA_FALSE;
	font->isItalic			= IXA_FALSE;
	font->emBoxSize			= 0;
	font->ascendent			= 0;
	font->descendent		= 0;
	font->lineHeightGuide	= 0;
	font->fontName			= NULL;
	font->glyphsArr			= NULL;
	font->glyphsArrUse		= 0;
	font->glyphsArrSize		= 0;
}

void ixaFontFinalize(STIXA_Font* font){
	if(font->fontName!=NULL){ IXA_FREE(font->fontName); }
	if(font->glyphsArr!=NULL){
		IxaUI16 i; const IxaUI16 use = font->glyphsArrUse;
		for(i=0; i<use; i++) ixaGlyphFinalize(&font->glyphsArr[i]);
		IXA_FREE(font->glyphsArr);
	}
}

//-------------------------------
//-- SOUNDS (compressed or uncompressed)
//-------------------------------
void ixaSoundInit(STIXA_Sound* sound){
	sound->soundFormat		= 0;
	sound->samplerateType	= 0;
	sound->sampleBitsType	= 0;
	sound->channelsType		= 0;
	sound->samplesCount		= 0;
	sound->soundData		= NULL;
	sound->soundDataSize	= 0;
}

void ixaSoundFinalize(STIXA_Sound* sound){
	if(sound->soundData!=NULL){ IXA_FREE(sound->soundData); sound->soundData = NULL; }
}

//-------------------------------
//-- SPRITES (movie clips)
//-------------------------------
void ixaSpriteInit(STIXA_Sprite* sprite){
	sprite->framesCount			= 0;
	//
	sprite->elementsArr			= NULL;		//Elements inside this sprite
	sprite->elementsArrUse		= 0;
	sprite->elementsArrSize		= 0;
	//
	sprite->elementsPropsArr	= NULL;	//Propierties of each element by frame
	sprite->elementsPropsArrUse	= 0;
	sprite->elementsPropsArrSize= 0;
	//
	sprite->soundEventsArr		= NULL;	//Sound events on this sprite
	sprite->soundEventsArrUse	= 0;
	sprite->soundEventsArrSize	= 0;
}

void ixaSpriteFinalize(STIXA_Sprite* sprite){
	sprite->framesCount = 0;
	if(sprite->elementsArr!=NULL){
		IxaUI16 i; const IxaUI16 use = sprite->elementsArrUse;
		for(i=0; i<use; i++){
			if(sprite->elementsArr[i].nombre!=NULL){
				IXA_FREE(sprite->elementsArr[i].nombre)
			}
		}
		IXA_FREE(sprite->elementsArr); sprite->elementsArr = NULL;
	}
	if(sprite->elementsPropsArr!=NULL){
		IXA_FREE(sprite->elementsPropsArr); sprite->elementsPropsArr = NULL;
	}
	if(sprite->soundEventsArr!=NULL){
		IxaUI16 i; const IxaUI16 use = sprite->soundEventsArrUse;
		for(i=0; i<use; i++){
			if(sprite->soundEventsArr[i].datosEvento.puntosVolumen!=NULL){
				IXA_FREE(sprite->soundEventsArr[i].datosEvento.puntosVolumen);
			}
		}
		IXA_FREE(sprite->soundEventsArr); sprite->soundEventsArr = NULL;
	}
}

void ixaSpriteFrameAdd(STIXA_Sprite* sprite){
	const IxaUI16 arrUse = sprite->elementsPropsArrUse;
	if(arrUse!=0){
		IxaUI16 i, propsToClone = 0; const IxaUI16 lastFrameIndex = sprite->framesCount - 1;
		//Count propierties to clone
		for(i=0; i<arrUse; i++){
			if(sprite->elementsPropsArr[i].frame==lastFrameIndex) propsToClone++;
		}
		//Clone all properties from last frame (the new frame defaults)
		if(propsToClone!=0){
			IXA_ASSERT(sprite->elementsPropsArrUse <= sprite->elementsPropsArrSize) //Assert if memory corrupted
			if((sprite->elementsPropsArrUse + propsToClone) > sprite->elementsPropsArrSize){
				ixaArrayResize((void**)&sprite->elementsPropsArr, &sprite->elementsPropsArrUse, &sprite->elementsPropsArrSize, sizeof(STIXA_SpriteElemProps), (sprite->elementsPropsArrUse + propsToClone) - sprite->elementsPropsArrSize);
			}
			for(i=0; i<arrUse; i++){
				if(sprite->elementsPropsArr[i].frame==lastFrameIndex){
					sprite->elementsPropsArr[sprite->elementsPropsArrUse] = sprite->elementsPropsArr[i];
					sprite->elementsPropsArr[sprite->elementsPropsArrUse++].frame = lastFrameIndex + 1;
				}
			}
		}
	}
	sprite->framesCount++;
}

void ixaSpriteSoundEventAdd(STIXA_Sprite* sprite, const STIXA_SoundEvent* soundEvent){
	IXA_ASSERT(sprite->soundEventsArrUse <= sprite->soundEventsArrSize) //Assert if memory corrupted
	if(sprite->soundEventsArrUse == sprite->soundEventsArrSize){
		ixaArrayResize((void**)&sprite->soundEventsArr, &sprite->soundEventsArrUse, &sprite->soundEventsArrSize, sizeof(STIXA_SoundEvent), IXA_SPRITE_SOUNDEVENTS_SIZE_GROWTH);
	}
	sprite->soundEventsArr[sprite->soundEventsArrUse++] = *soundEvent;
}

void ixaSpriteObjectAdd(STIXA_Sprite* sprite, const IxaUI16 depth, const IxaUI16 charID, const char* name){
	IxaUI32 nameLen = 0; IxaUI32 i; IxaSI32 destIndex = -1; const IxaUI16 elemsUse = sprite->elementsArrUse;
	STIXA_SpriteElem nuevoElemento;
	if(name!=NULL) nameLen = ixaStringLenght(name);
	nuevoElemento.enDisplayList	= IXA_TRUE;
	nuevoElemento.idElemento	= sprite->elementsArrSize;
	nuevoElemento.charID		= charID;
	nuevoElemento.profundidad	= depth;
	IXA_MALLOC(nuevoElemento.nombre, char, sizeof(char) * (nameLen + 1), "SpriteElem.nombre")
	if(nameLen!=0) memcpy(nuevoElemento.nombre, name, nameLen);
	nuevoElemento.nombre[nameLen] = '\0';
	//Determine the destination index
	for(i=0; i<elemsUse; i++){
		if(sprite->elementsArr[i].profundidad > depth){
			destIndex = i;
			break;
		}
	}
	/*#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	for(i=0; i<_elementos->conteo; i++){
		STIXA_SpriteElem elemento = _elementos->elemento[i];
		IXA_ASSERT(elemento.profundidad!=profundidad || !elemento.enDisplayList) //Ya hay un elemento en la misma profundidad y activo
	}
	#endif*/
	if(destIndex==-1) destIndex = elemsUse;
	IXA_ASSERT(sprite->elementsArrUse <= sprite->elementsArrSize) //Assert if memory corrupted
	if(sprite->elementsArrUse == sprite->elementsArrSize){
		ixaArrayResize((void**)&sprite->elementsArr, &sprite->elementsArrUse, &sprite->elementsArrSize, sizeof(STIXA_SpriteElem), IXA_SPRITE_ELEMS_SIZE_GROWTH);
	}
	//Make room for new element
	{
		IxaUI16 i = elemsUse;
		while(i!=destIndex){
			sprite->elementsArr[i] = sprite->elementsArr[i-1];
			i--;
		}
	}
	sprite->elementsArr[destIndex] = nuevoElemento;
	sprite->elementsArrUse++;
}

void ixaSpriteObjectRemove(STIXA_Sprite* sprite, const IxaUI16 depth){
	//Make visible all elements at the same depth, in the actual frame
	IxaUI32 i; IxaSI32 foundAtIndex = -1; const IxaUI16 lastFrameIndex = sprite->framesCount - 1;
	STIXA_SpriteElem* elemsArr = sprite->elementsArr; const IxaUI16 elemsUse = sprite->elementsArrUse;
	for(i=0; i<elemsUse; i++){
		if(elemsArr[i].profundidad==depth){
			if(elemsArr[i].enDisplayList){
				IXA_ASSERT(foundAtIndex==-1) //Two elements in displayList at the same depth?
				foundAtIndex = i;
			}
			elemsArr[i].enDisplayList = IXA_FALSE;
			{
				IxaUI32 j; const IxaUI16 elemsPropsUse = sprite->elementsPropsArrUse; STIXA_SpriteElemProps* elemsPropsArr = sprite->elementsPropsArr;
				for(j=0; j<elemsPropsUse; j++){
					if(elemsPropsArr[j].frame==lastFrameIndex && elemsPropsArr[j].idElemento==elemsArr[i].idElemento){
						elemsPropsArr[j].visible = IXA_FALSE;
					}
				}
			}
		}
	}
	IXA_ASSERT(foundAtIndex!=-1) //No element in the specified depth?
}

IxaBOOL ixaSpriteObjectReplace(STIXA_Sprite* sprite, const IxaUI16 depth, const IxaUI16 charID, const char* name){
	//Look for the active object in that depth (use its actual properties)
	IxaSI32 idElementAtDepth = -1;
	IxaUI32 i; STIXA_SpriteElem* elemsArr = sprite->elementsArr; const IxaUI16 elemsUse = sprite->elementsArrUse;
	for(i=0; i<elemsUse; i++){
		if(elemsArr[i].profundidad==depth && elemsArr[i].enDisplayList){
			idElementAtDepth = elemsArr[i].idElemento;
			//break;
		}
	}
	IXA_ASSERT(idElementAtDepth!=-1) //There was no active object int he specified depth
	if(idElementAtDepth!=-1){
		IxaBOOL propsFound = IXA_FALSE;
		STIXA_SpriteElemProps propsToUse;
		const IxaUI16 lastFrameIndex = sprite->framesCount - 1;
		IxaUI32 j; STIXA_SpriteElemProps* elemsProps = sprite->elementsPropsArr; const IxaUI16 elemsPropsUse = sprite->elementsPropsArrUse;
		for(j=0; j<elemsPropsUse; j++){
			if(elemsProps[j].frame==lastFrameIndex && elemsProps[j].idElemento==idElementAtDepth){
				IXA_ASSERT(!propsFound) //More than one props found!
				propsToUse = elemsProps[j];
				propsFound = IXA_TRUE;
			}
		}
		IXA_ASSERT(propsFound)
		//
		ixaSpriteObjectRemove(sprite, depth);
		ixaSpriteObjectAdd(sprite, depth, charID, name);
		ixaSpriteObjectSetProps(sprite, depth, &propsToUse.matriz, &propsToUse.color);
		return IXA_TRUE;
	}
	return IXA_FALSE;
}

void ixaSpriteObjectGetProps(const STIXA_Sprite* sprite, STIXA_SpriteElemProps* dest, const IxaUI16 idElement, const IxaUI16 iFrame){
	IxaUI16 i; const IxaUI16 useProps = sprite->elementsPropsArrUse;
	for(i=0; i<useProps; i++){
		if(sprite->elementsPropsArr[i].idElemento==idElement && sprite->elementsPropsArr[i].frame==iFrame){
			*dest = sprite->elementsPropsArr[i];
			return;
		}
	}
	dest->visible = IXA_FALSE;
}
	
void ixaSpriteObjectSetProps(STIXA_Sprite* sprite, const IxaUI16 depth, const STIXA_Matrix* matrix, const STIXA_CXForm* color){
	//Look for the active object in that depth
	IxaSI32 idElementAtDepth = -1;
	IxaUI32 i; STIXA_SpriteElem* elemsArr = sprite->elementsArr; const IxaUI16 elemsUse = sprite->elementsArrUse;
	for(i=0; i<elemsUse; i++){
		if(elemsArr[i].profundidad==depth && elemsArr[i].enDisplayList){
			idElementAtDepth = elemsArr[i].idElemento;
			//break;
		}
	}
	IXA_ASSERT(idElementAtDepth!=-1)
	if(idElementAtDepth!=-1){
		//Try to update existing properties
		const IxaUI16 lastFrameIndex = sprite->framesCount - 1;
		IxaBOOL propsUpdated = IXA_FALSE;
		IxaUI32 j; STIXA_SpriteElemProps* arregloProp = sprite->elementsPropsArr; const IxaUI16 elemsPropsUse = sprite->elementsPropsArrUse;
		for(j=0; j<elemsPropsUse; j++){
			if(arregloProp[j].frame==lastFrameIndex && arregloProp[j].idElemento==idElementAtDepth){
				arregloProp[j].visible					= IXA_TRUE;
				if(matrix!=NULL)arregloProp[j].matriz	= *matrix;
				if(color!=NULL)	arregloProp[j].color	= *color;
				propsUpdated = IXA_TRUE;
				//break;
			}
		}
		//Add new properties
		if(!propsUpdated){
			STIXA_SpriteElemProps nuevasPropiedades;
			nuevasPropiedades.visible							= IXA_TRUE;
			nuevasPropiedades.frame 							= lastFrameIndex;
			nuevasPropiedades.idElemento						= idElementAtDepth;
			if(matrix!=NULL){
				nuevasPropiedades.matriz						= *matrix;
			} else {
				//
				nuevasPropiedades.matriz.tieneEscala 			= 0;
				nuevasPropiedades.matriz.bitsPorEscala 			= 0;
				nuevasPropiedades.matriz.escalaX 				= 1.0;
				nuevasPropiedades.matriz.escalaY 				= 1.0;
				//
				nuevasPropiedades.matriz.tieneRotacion			= 0;
				nuevasPropiedades.matriz.bitsPorRotacion		= 0;
				nuevasPropiedades.matriz.skew0					= 0.0;
				nuevasPropiedades.matriz.skew1					= 0.0;
				//
				nuevasPropiedades.matriz.bitsPorTranslacion		= 0;
				nuevasPropiedades.matriz.traslacionX			= 0;
				nuevasPropiedades.matriz.traslacionY			= 0;
			}
			if(color!=NULL){
				nuevasPropiedades.color							= *color;
			} else {
				nuevasPropiedades.color.tieneAdiciones			= 0;
				nuevasPropiedades.color.tieneMultiplicaciones	= 0;
				nuevasPropiedades.color.nBits					= 0;
				nuevasPropiedades.color.multiplicadorpR			= 1;
				nuevasPropiedades.color.multiplicadorpG			= 1;
				nuevasPropiedades.color.multiplicadorpB			= 1;
				nuevasPropiedades.color.multiplicadorpA			= 1;
				nuevasPropiedades.color.adicionR				= 0;
				nuevasPropiedades.color.adicionG				= 0;
				nuevasPropiedades.color.adicionB				= 0;
				nuevasPropiedades.color.adicionA				= 0;
			}
			IXA_ASSERT((sprite->elementsPropsArrUse <= sprite->elementsPropsArrSize)) //Assert if memory corrupted
			if(sprite->elementsPropsArrUse==sprite->elementsPropsArrSize){
				ixaArrayResize((void**)&sprite->elementsPropsArr, &sprite->elementsPropsArrUse, &sprite->elementsPropsArrSize, sizeof(STIXA_SpriteElemProps), IXA_SPRITE_ELEMSPROPS_SIZE_GROWTH);
			}
			sprite->elementsPropsArr[sprite->elementsPropsArrUse++] = nuevasPropiedades;
		}
	}
}

//-------------------------------
//-- SWF FILE
//-------------------------------
void ixaSwfFileInit(STIXA_SwfFile* swf){
	swf->_swfDescomprimido			= NULL;
	//
	swf->_posPrimerTag					= 0;
	swf->_conteoTags					= 0;
	ixaSpriteInit(&swf->_mainSprite);
	//
	swf->_encabezado.firma1				= ' ';
	swf->_encabezado.firma2				= ' ';
	swf->_encabezado.firma3				= ' ';
	swf->_encabezado.version			= 0;
	swf->_encabezado.bytesDatos			= 0;
	swf->_encabezado2.tamanoTwips.Nbits = 0;
	swf->_encabezado2.tamanoTwips.Xmax 	= 0;
	swf->_encabezado2.tamanoTwips.Xmin 	= 0;
	swf->_encabezado2.tamanoTwips.Ymin	= 0;
	swf->_encabezado2.tamanoTwips.Ymax	= 0;
	swf->_encabezado2.framerate			= 0.0f;
	swf->_encabezado2.totalFrames		= 0;
	swf->_JPEGSharedTable.puntero		= NULL;
	swf->_JPEGSharedTable.tamano		= 0;
	//
	swf->_diccArr						= NULL;
	swf->_diccArrUse					= 0;
	swf->_diccArrSize					= 0;
}

void ixaSwfFileFinalize(STIXA_SwfFile* swf){
	ixaSpriteFinalize(&swf->_mainSprite);
	if(swf->_swfDescomprimido!=NULL){
		IXA_FREE(swf->_swfDescomprimido); swf->_swfDescomprimido = NULL;
	}
	if(swf->_diccArr!=NULL){
		IxaUI16 i; const IxaUI16 use = swf->_diccArrUse;
		for(i=0; i<use; i++){
			if(swf->_diccArr[i].charPointer!=NULL){
				switch (swf->_diccArr[i].charType) {
					case ENIXA_CharType_Sprite: ixaSpriteFinalize((STIXA_Sprite*)swf->_diccArr[i].charPointer); break;
					case ENIXA_CharType_Bitmap: ixaBitmapFinalize((STIXA_Bitmap*)swf->_diccArr[i].charPointer); break;
					case ENIXA_CharType_Shape:	ixaShapeFinalize((STIXA_Shape*)swf->_diccArr[i].charPointer); break;
					case ENIXA_CharType_Font:	ixaFontFinalize((STIXA_Font*)swf->_diccArr[i].charPointer); break;
					case ENIXA_CharType_Sound:	ixaSoundFinalize((STIXA_Sound*)swf->_diccArr[i].charPointer); break;
					case ENIXA_CharType_FileBits:	ixaFileBinaryFinalize((STIXA_FileBits*)swf->_diccArr[i].charPointer); break;
					default: IXA_ASSERT(0) break;
				}
				IXA_FREE(swf->_diccArr[i].charPointer);
			}
		}
		IXA_FREE(swf->_diccArr); swf->_diccArr = NULL;
	}
	if(swf->_JPEGSharedTable.puntero!=NULL){
		IXA_FREE(swf->_JPEGSharedTable.puntero); swf->_JPEGSharedTable.puntero = NULL; swf->_JPEGSharedTable.tamano = 0;
	}
}

void ixaSwfDictAddCharacter(STIXA_SwfFile* swf, const ENIXA_CharType charType, const IxaUI16 charID, void* charPointer){
	IXA_ASSERT(swf->_diccArrUse <= swf->_diccArrSize) //Assert if memory corrupted
	if(swf->_diccArrUse == swf->_diccArrSize){
		ixaArrayResize((void**)&swf->_diccArr, &swf->_diccArrUse, &swf->_diccArrSize, sizeof(STIXA_DiccElem), IXA_SWF_DICC_SIZE_GROWTH);
	}
	swf->_diccArr[swf->_diccArrUse].charType		= charType;
	swf->_diccArr[swf->_diccArrUse].charID			= charID;
	swf->_diccArr[swf->_diccArrUse++].charPointer	= charPointer;
}

IxaBOOL ixaSwfDictGetElemByCharID(const STIXA_SwfFile* swf, STIXA_DiccElem* dest, const IxaUI16 charID){
	IxaUI16 i; const IxaUI16 use = swf->_diccArrUse;
	for(i=0; i<use; i++){
		if(swf->_diccArr[i].charID==charID){
			*dest = swf->_diccArr[i];
			return IXA_TRUE;
		}
	}
	return IXA_FALSE;
}
	
IxaBOOL ixaSwfLoadFile(STIXA_SwfFile* swf, const char* path, PTRIXA_zlibUncompress funcUncompress){
	IxaBOOL r = IXA_FALSE;
	FILE* file = fopen(path, "rb");
	if(file==NULL){
		PRINTF_ERROR("fopen error for: '%s'.\n", path);
	} else {
		//Read first header (the rest of the file could be compressed)
		if(fread(&swf->_encabezado, sizeof(swf->_encabezado), 1, file)!=1){
			PRINTF_ERROR("fread on headerBase error for: '%s'.\n", path);
		} else if(swf->_encabezado.bytesDatos==0){
			PRINTF_ERROR("swf_body is zero bytes according to headerBase: '%s'.\n", path);
		} else {
			STIXA_Reader ixaRdr; IxaBOOL swfDataLoaded;
			IXA_MALLOC(swf->_swfDescomprimido, IxaUI8, sizeof(IxaUI8) * swf->_encabezado.bytesDatos, "_swfDescomprimido");
			//Copy the header to the uncompressed buffer (the uncompressed file size include the already parsed header)
			*((STIXA_HeaderBase*)swf->_swfDescomprimido) = swf->_encabezado;
			__ixaReaderInit(&ixaRdr, swf->_swfDescomprimido);
			__ixaReaderIgnoreBytes(&ixaRdr, sizeof(swf->_encabezado));
			//Read data
			swfDataLoaded = IXA_FALSE;
			if(swf->_encabezado.firma1 != 'C'){
				//Read uncompressed data
				if(fread(&(swf->_swfDescomprimido[sizeof(swf->_encabezado)]), swf->_encabezado.bytesDatos - sizeof(swf->_encabezado), 1, file)!=1){
					PRINTF_ERROR("fread failed when reading the uncompressed data for: '%s'.\n", path);
				} else {
					swfDataLoaded = IXA_TRUE;
				}
			} else if(funcUncompress==NULL){
				PRINTF_ERROR("SWF is zlib compressed and a uncompression funcPointer was not provided, for: '%s'.\n", path);
			} else {
				//Read compressed data
				//const IxaUI32 bytesCompressedData = swf->_encabezado.bytesDatos - sizeof(swf->_encabezado);
				IxaBYTE* compressedBuffer; IxaSI32 bytesCompressedRead;
				IXA_MALLOC(compressedBuffer, IxaBYTE, sizeof(IxaUI8) * swf->_encabezado.bytesDatos, "compressedBuffer")
				bytesCompressedRead	= (IxaSI32)fread(compressedBuffer, 1, swf->_encabezado.bytesDatos, file);
				if(bytesCompressedRead<=0){
					PRINTF_ERROR("fread failed when reading the compressed data for: '%s'.\n", path);
				} else {
					if(!(*funcUncompress)(&(swf->_swfDescomprimido[sizeof(swf->_encabezado)]), swf->_encabezado.bytesDatos, compressedBuffer, bytesCompressedRead)){
						PRINTF_ERROR("SWF is zlib compressed and the provided uncompression funcPointer failed, for: '%s'.\n", path);
					} else {
						swfDataLoaded = IXA_TRUE;
					}
				}
				IXA_FREE(compressedBuffer);
			}
			//Parse the uncompressed data
			if(swfDataLoaded){
				swf->_encabezado2.tamanoTwips 	= __ixaLoadRECT(&ixaRdr);
				swf->_encabezado2.framerate		= __ixaReaderGetFixed8(&ixaRdr);
				swf->_encabezado2.totalFrames	= __ixaReaderGetUI16(&ixaRdr);
				//PRINTF_INFO("Bits por coordenada %u (xMin=%d, xMax=%d, yMin=%d, yMax=%d)\n", _encabezado2.tamanoTwips.Nbits, _encabezado2.tamanoTwips.Xmin, _encabezado2.tamanoTwips.Xmax, _encabezado2.tamanoTwips.Ymin, _encabezado2.tamanoTwips.Ymax);
				//PRINTF_INFO("Framerate: %f\n", swf->_encabezado2.framerate);
				//PRINTF_INFO("Total de frames: %u\n", swf->_encabezado2.totalFrames);
				//posicion del primer TAG
				swf->_posPrimerTag				= ixaRdr._posicionEnFlujo;
				if(ixaRdr._bitsRestantes!=0) swf->_posPrimerTag++;
				//PRINTF_INFO("Primer TAG en posicion %d\n", swf->_posPrimerTag);
				//Parse SWF file
				if(__ixaSwfParseTags(swf, &ixaRdr, &swf->_mainSprite, funcUncompress, 0)){
					IXA_ASSERT(ixaRdr._conteoTotalBytesConsumidos==swf->_encabezado.bytesDatos)
					if(ixaRdr._conteoTotalBytesConsumidos==swf->_encabezado.bytesDatos){
						r = IXA_TRUE;
					}
				}
				PRINTF_INFO("%d bytes of %d read from SWF.\n", ixaRdr._conteoTotalBytesConsumidos, swf->_encabezado.bytesDatos);
			}
			__ixaReaderFinalize(&ixaRdr);
		}
		fclose(file);
	}
	return r;
}

const char* __ixaTagName(const ENIXA_TagID tagId){
	switch(tagId){
		case ENIXA_TagID_End: return "End";
		case ENIXA_TagID_ShowFrame: return "ShowFrame";
		case ENIXA_TagID_DefineShape: return "DefineShape";
		case ENIXA_TagID_PlaceObject: return "PlaceObject";
		case ENIXA_TagID_RemoveObject: return "RemoveObject";
		case ENIXA_TagID_DefineBits: return "DefineBits";
		case ENIXA_TagID_DefineButton: return "DefineButton";
		case ENIXA_TagID_JPEGTables: return "JPEGTables";
		case ENIXA_TagID_SetBackgroundColor: return "SetBackgroundColor";
		case ENIXA_TagID_DefineFont: return "DefineFont";
		case ENIXA_TagID_DefineText: return "DefineText";
		case ENIXA_TagID_DoAction: return "DoAction";
		case ENIXA_TagID_DefineFontInfo: return "DefineFontInfo";
		case ENIXA_TagID_DefineSound: return "DefineSound";
		case ENIXA_TagID_StartSound: return "StartSound";
		case ENIXA_TagID_DefineButtonSound: return "DefineButtonSound";
		case ENIXA_TagID_SoundStreamHead: return "SoundStreamHead";
		case ENIXA_TagID_SoundStreamBlock: return "SoundStreamBlock";
		case ENIXA_TagID_DefineBitsLossless: return "DefineBitsLossless";
		case ENIXA_TagID_DefineBitsJPEG2: return "DefineBitsJPEG2";
		case ENIXA_TagID_DefineShape2: return "DefineShape2";
		case ENIXA_TagID_DefineButtonCxform: return "DefineButtonCxform";
		case ENIXA_TagID_Protect: return "Protect";
		case ENIXA_TagID_PlaceObject2: return "PlaceObject2";
		case ENIXA_TagID_RemoveObject2: return "RemoveObject2";
		case ENIXA_TagID_DefineShape3: return "DefineShape3";
		case ENIXA_TagID_DefineText2: return "DefineText2";
		case ENIXA_TagID_DefineButton2: return "DefineButton2";
		case ENIXA_TagID_DefineBitsJPEG3: return "DefineBitsJPEG3";
		case ENIXA_TagID_DefineBitsLossless2: return "DefineBitsLossless2";
		case ENIXA_TagID_DefineEditText: return "DefineEditText";
		case ENIXA_TagID_DefineSprite: return "DefineSprite";
		case ENIXA_TagID_FrameLabel: return "FrameLabel";
		case ENIXA_TagID_SoundStreamHead2: return "SoundStreamHead2";
		case ENIXA_TagID_DefineMorphShape: return "DefineMorphShape";
		case ENIXA_TagID_DefineFont2: return "DefineFont2";
		case ENIXA_TagID_ExportAssets: return "ExportAssets";
		case ENIXA_TagID_ImportAssets: return "ImportAssets";
		case ENIXA_TagID_EnableDebugger: return "EnableDebugger";
		case ENIXA_TagID_DoInitAction: return "DoInitAction";
		case ENIXA_TagID_DefineVideoStream: return "DefineVideoStream";
		case ENIXA_TagID_VideoFrame: return "VideoFrame";
		case ENIXA_TagID_DefineFontInfo2: return "DefineFontInfo2";
		case ENIXA_TagID_EnableDebugger2: return "EnableDebugger2";
		case ENIXA_TagID_ScriptLimits: return "ScriptLimits";
		case ENIXA_TagID_SetTabIndex: return "SetTabIndex";
		case ENIXA_TagID_FileAttributes: return "FileAttributes";
		case ENIXA_TagID_PlaceObject3: return "PlaceObject3";
		case ENIXA_TagID_ImportAssets2: return "ImportAssets2";
		case ENIXA_TagID_DefineFontAlignZones: return "DefineFontAlignZones";
		case ENIXA_TagID_CSMTextSettings: return "CSMTextSettings";
		case ENIXA_TagID_DefineFont3: return "DefineFont3";
		case ENIXA_TagID_SymbolClass: return "SymbolClass";
		case ENIXA_TagID_Metadata: return "Metadata";
		case ENIXA_TagID_DefineScalingGrid: return "DefineScalingGrid";
		case ENIXA_TagID_DoABC: return "DoABC";
		case ENIXA_TagID_DefineShape4: return "DefineShape4";
		case ENIXA_TagID_DefineMorphShape2: return "DefineMorphShape2";
		case ENIXA_TagID_DefineSceneAndFrameLabelData: return "DefineSceneAndFrameLabelData";
		case ENIXA_TagID_DefineBinaryData: return "DefineBinaryData";
		case ENIXA_TagID_DefineFontName: return "DefineFontName";
		case ENIXA_TagID_StartSound2: return "StartSound2";
		case ENIXA_TagID_DefineBitsJPEG4: return "DefineBitsJPEG4";
		case ENIXA_TagID_DefineFont4: return "DefineFont4";
		default: return NULL;
	}
	return NULL;
}

IxaBOOL __ixaSwfParseTags(STIXA_SwfFile* swf, STIXA_Reader* rdr, STIXA_Sprite* sprite, PTRIXA_zlibUncompress funcUncompress, const IxaUI16 currIdentLevel){ //Identation
	IxaUI16 i;
	IxaUI32 conteoTags = 0;
	IxaUI16 encabezadoTag;
	IxaUI16 tagType;
	IxaSI32 bytesTag;
	IxaBOOL tagValido, interpretacionCorrecta;
	IxaSI32 currFrameIndex = -1;
	IxaBOOL currFrameActive = IXA_FALSE;
	const char* tagName = NULL;
	char* strIdent;
	IXA_MALLOC(strIdent, char, sizeof(char) * (currIdentLevel + 1), "strIdent")
	for(i=0; i<currIdentLevel; i++) strIdent[i] = '-'; strIdent[currIdentLevel] = '\0';
	do {
		conteoTags++;
		//PRINTF_INFO("Interpretando TAG en pos (%d)\n", rdr->_posicionEnFlujo);
		__ixaReaderGetBytes(rdr, &encabezadoTag, sizeof(encabezadoTag));
		tagType		= (encabezadoTag >> 6);
		bytesTag	= (encabezadoTag & 0x3F);
		tagName		= __ixaTagName((ENIXA_TagID)tagType);
		tagValido	= (tagName!=NULL);
		if(bytesTag == 0x3F){ //63
			__ixaReaderGetBytes(rdr, &bytesTag, sizeof(bytesTag));
			//PRINTF_INFO("%slongTAG '%s' (%u): %d bytes in data\n", strIdent, tagName, tagType, bytesTag);
		} else {
			//PRINTF_INFO("%sshortTAG '%s' (%u): %u bytes in data\n", strIdent, tagName, tagType, bytesTag);
		}
		if(bytesTag<0){
			PRINTF_ERROR("ERROR interpretando tamano de TAG SWF '%s': %d bytes?\n", tagName, bytesTag);
			IXA_ASSERT(0);
		}
		if(!tagValido){
			PRINTF_ERROR("ERROR TAG SWF '%d' no reconocible (de %d bytes)\n", tagType, bytesTag);
			IXA_ASSERT(0);
		} else {
			//procesar TAG
			interpretacionCorrecta = IXA_TRUE;
			rdr->_conteoTmpBytesConsumidos = 0;
			if(tagType==ENIXA_TagID_DefineSprite){
				STIXA_Reader subRdr; IxaUI16 spriteID; IxaUI16 framesCount; STIXA_Sprite* subSprite;
				__ixaReaderInit(&subRdr, __ixaReaderCurrentByte(rdr));
				__ixaReaderGetBytes(&subRdr, &spriteID, sizeof(spriteID));
				__ixaReaderGetBytes(&subRdr, &framesCount, sizeof(framesCount)); //este framecount no representa la cantidad de frames finales en la spriteSwf (todos los frames consecutivos sin cambios cuentan como uno solo)
				//PRINTF_INFO("Frame %d, Sprite definida id(%d) frames(%d)\n", indiceFrameActual, spriteID, framesCount);
				IXA_MALLOC(subSprite, STIXA_Sprite, sizeof(STIXA_Sprite), "subSprite")
				ixaSpriteInit(subSprite);
				if(__ixaSwfParseTags(swf, &subRdr, subSprite, funcUncompress, currIdentLevel + 1)){
					ixaSwfDictAddCharacter(swf, ENIXA_CharType_Sprite, spriteID, subSprite);
				} else {
					ixaSpriteFinalize(subSprite);
					IXA_FREE(subSprite);
					interpretacionCorrecta = IXA_FALSE;
				}
				//
				__ixaReaderIgnoreBytes(rdr, subRdr._conteoTotalBytesConsumidos);
				__ixaReaderFinalize(&subRdr);
			} else if(tagType == ENIXA_TagID_JPEGTables){
				__ixaSwfDictLoadJPEGSharedTable(swf, rdr, bytesTag);
			} else if(tagType==ENIXA_TagID_DefineBits || tagType==ENIXA_TagID_DefineBitsJPEG2 || tagType==ENIXA_TagID_DefineBitsJPEG3 || tagType==ENIXA_TagID_DefineBitsJPEG4 || tagType==ENIXA_TagID_DefineBinaryData){
				__ixaSwfDictLoadFile(swf, rdr, (ENIXA_TagID)tagType, bytesTag);
			} else if(tagType==ENIXA_TagID_DefineBitsLossless || tagType==ENIXA_TagID_DefineBitsLossless2) {
				__ixaSwfDictLoadBitmap(swf, rdr, (ENIXA_TagID)tagType, bytesTag, funcUncompress);
			} else if (tagType==ENIXA_TagID_DefineShape || tagType==ENIXA_TagID_DefineShape2 || tagType==ENIXA_TagID_DefineShape3 || tagType==ENIXA_TagID_DefineShape4){
				__ixaSwfDictLoadShape(swf, rdr, (ENIXA_TagID)tagType, bytesTag);
				//PRINTF_INFO("Frame %d, Shape definida ID(%d)\n", indiceFrameActual, _diccIDs->elemento[_diccIDs->conteo-1]);
			} else if (tagType==ENIXA_TagID_DefineFont || tagType==ENIXA_TagID_DefineFont2 || tagType==ENIXA_TagID_DefineFont3){
				__ixaSwfDictLoadFont(swf, rdr, (ENIXA_TagID)tagType, bytesTag);
			} else if(tagType==ENIXA_TagID_DefineSound){
				__ixaSwfDictLoadSound(swf, rdr, (ENIXA_TagID)tagType, bytesTag);
			//} else if (tagType==ENIXA_TagID_DefineText || tagType==ENIXA_TagID_DefineText2){
				//	dicc_AgregarTexto((ENIXA_TagID)tagType, bytesTag, lector);
			} else if (tagType==ENIXA_TagID_PlaceObject2){
				if(!currFrameActive){
					ixaSpriteFrameAdd(sprite);
					currFrameActive = IXA_TRUE;
					currFrameIndex++;
				}
				//PRINTF_INFO("Frame %d, ENIXA_TagID_PlaceObject2\n", indiceFrameActual);
				__ixaSwfFramePlaceObject(swf, rdr, sprite, (ENIXA_TagID)tagType, bytesTag);
			} else if (tagType==ENIXA_TagID_RemoveObject || tagType==ENIXA_TagID_RemoveObject2){
				if(!currFrameActive){
					ixaSpriteFrameAdd(sprite);
					currFrameActive = IXA_TRUE;
					currFrameIndex++;
				}
				//PRINTF_INFO("Frame %d, ENIXA_TagID_RemoveObject\n", indiceFrameActual);
				__ixaSwfFrameRemoveObject(swf, rdr, sprite, (ENIXA_TagID)tagType, bytesTag);
			} else if (tagType==ENIXA_TagID_ShowFrame){
				if(!currFrameActive){
					ixaSpriteFrameAdd(sprite);
					currFrameActive = IXA_TRUE;
					currFrameIndex++;
				}
				currFrameActive = IXA_FALSE;
				//PRINTF_INFO("%sFrame %d, ENIXA_TagID_ShowFrame (nivel %d)\n", strIdent, indiceFrameActual, nivelIdentacion);
			/*} else if(tagType==ENIXA_TagID_StartSound){
				if(nuevoFrameCreado==false){
					spriteActual->agregarFrame();
					nuevoFrameCreado = true;
					indiceFrameActual++;
				}
				STIXA_SoundEvent eventoSonido;
				eventoSonido.iFrame			= indiceFrameActual;
				eventoSonido.soundID		= __ixaReaderGetUI16(rdr);
				eventoSonido.datosEvento	= leerSOUNDINFO(lector);
				spriteActual->agregarEventoSonido(eventoSonido);*/
			} else {
				__ixaReaderIgnoreBytes(rdr, bytesTag);
			}
			//comprar los bytes consumidos con el total del TAG
			__ixaReaderPositionInByteAligned(rdr);
			if(bytesTag != rdr->_conteoTmpBytesConsumidos){
				PRINTF_INFO("%sError al interpretar el TAG '%s' de %d bytes (se consumieron %d, faltaron %d bytes)\n", strIdent, tagName, bytesTag, rdr->_conteoTmpBytesConsumidos, bytesTag - rdr->_conteoTmpBytesConsumidos);
				interpretacionCorrecta = IXA_FALSE;
				IXA_ASSERT(0)
			}
		}
	} while(tagType != ENIXA_TagID_End && tagValido && interpretacionCorrecta);
	//PRINTF_INFO("%s%d TAGs (nivel %d)\n", strIdent, conteoTags, currIdentLevel);
	IXA_FREE(strIdent);
	return interpretacionCorrecta;
}

IxaBOOL __ixaSwfDictLoadJPEGSharedTable(STIXA_SwfFile* swf, STIXA_Reader* rdr, const IxaUI32 bytesTag){
	IxaBOOL r = IXA_FALSE;
	IXA_ASSERT(swf->_JPEGSharedTable.puntero == NULL)
	if(swf->_JPEGSharedTable.puntero != NULL){
		__ixaReaderIgnoreBytes(rdr, bytesTag);
	} else {
		IXA_MALLOC(swf->_JPEGSharedTable.puntero, IxaBYTE, sizeof(IxaBYTE) * bytesTag, "_JPEGSharedTable.puntero")
		swf->_JPEGSharedTable.tamano	= bytesTag;
		__ixaReaderGetBytes(rdr, swf->_JPEGSharedTable.puntero, bytesTag);
		r = IXA_TRUE;
	}
	return r;
}

IxaBOOL __ixaSwfDictLoadFile(STIXA_SwfFile* swf, STIXA_Reader* rdr, ENIXA_TagID tagType, IxaSI32 bytesTag){
	if(tagType==ENIXA_TagID_DefineBits){
		//JPEG SIN TABLA
		STIXA_FileBits* filebin; IxaUI16 charID;
		IXA_MALLOC(filebin, STIXA_FileBits, sizeof(STIXA_FileBits), "FileBinary")
		ixaFileBinaryInit(filebin);
		__ixaReaderGetBytes(rdr, &charID, sizeof(charID));
		filebin->tipo 		= ENIXA_FileTypeJPEG_SIN_TABLA;
		filebin->tamano		= bytesTag - sizeof(charID);
		IXA_MALLOC(filebin->puntero, IxaBYTE, filebin->tamano, "FileBinary.puntero")
		__ixaReaderGetBytes(rdr, filebin->puntero, filebin->tamano);
		/*STIXA_Size tamano		=*/ ixaBitmapSize(filebin->puntero, filebin->tamano, filebin->tipo);
		ixaSwfDictAddCharacter(swf, ENIXA_CharType_FileBits, charID, filebin);
		return IXA_TRUE;
	} else if(tagType==ENIXA_TagID_DefineBitsJPEG2){
		//JPEG COMPLETO
		STIXA_FileBits* filebin; IxaUI16 charID; IxaUI32 bytesIgnorar;
		IXA_MALLOC(filebin, STIXA_FileBits, sizeof(STIXA_FileBits), "FileBinary")
		ixaFileBinaryInit(filebin);
		__ixaReaderGetBytes(rdr, &charID, sizeof(charID));
		bytesIgnorar			= 0;
		filebin->tipo			= ixaBitmapFileType(__ixaReaderCurrentByte(rdr), &bytesIgnorar); __ixaReaderIgnoreBytes(rdr, bytesIgnorar);
		filebin->tamano			= bytesTag - sizeof(charID) - bytesIgnorar;
		IXA_MALLOC(filebin->puntero, IxaBYTE, sizeof(IxaBYTE) * filebin->tamano, "FileBinary.puntero")
		__ixaReaderGetBytes(rdr, filebin->puntero, filebin->tamano);
		/*STIXA_Size tamano		=*/ ixaBitmapSize(filebin->puntero, filebin->tamano, filebin->tipo);
		ixaSwfDictAddCharacter(swf, ENIXA_CharType_FileBits, charID, filebin);
		return IXA_TRUE;
	} else if(tagType==ENIXA_TagID_DefineBitsJPEG3){
		//JPEG COMPLETO CON BITMAP GRAYSACLE ANEXO (para el alpha)
		STIXA_FileBits* filebin; IxaUI16 charID; IxaUI32 bytesImg; IxaUI32 bytesIgnorar = 0; long tamanoAnexoComprimido;
		IXA_MALLOC(filebin, STIXA_FileBits, sizeof(STIXA_FileBits), "FileBinary")
		ixaFileBinaryInit(filebin);
		__ixaReaderGetBytes(rdr, &charID, sizeof(charID));
		__ixaReaderGetBytes(rdr, &bytesImg, sizeof(bytesImg));
		filebin->tipo			= ixaBitmapFileType(__ixaReaderCurrentByte(rdr), &bytesIgnorar); __ixaReaderIgnoreBytes(rdr, bytesIgnorar);
		filebin->tamano			= bytesImg - bytesIgnorar;
		IXA_MALLOC(filebin->puntero, IxaBYTE, sizeof(IxaBYTE) * filebin->tamano, "FileBinary.puntero") __ixaReaderGetBytes(rdr, filebin->puntero, filebin->tamano);
		tamanoAnexoComprimido 	= bytesTag - sizeof(charID) - sizeof(bytesImg) - bytesIgnorar - filebin->tamano;
		//BYTE* punteroAnexo	= __ixaReaderCurrentByte(rdr);
		__ixaReaderIgnoreBytes(rdr, (int)tamanoAnexoComprimido);
		//Intentar descomprimir el anexo
		/*STIXA_Size tamanoJPEG			= ixaBitmapSize(descripcion->puntero, descripcion->tamano, descripcion->tipo);
		if(tamanoJPEG.width==0 || tamanoJPEG.height==0){
			PRINTF_ERROR("no se pudo extraer tamano de JPEG en ENIXA_TagID_DefineBitsJPEG3: ancho(%d) alto(%d)\n", tamanoJPEG.width, tamanoJPEG.height);
		} else {
			if(tamanoAnexoComprimido>0){
				long tamanoAnexoDescomprimido = (long)(tamanoJPEG.width * tamanoJPEG.height);
				BYTE* anexoDescomprimido = (BYTE*)IXA_MALLOC(tamanoAnexoDescomprimido);
				if(0 != ezuncompress(anexoDescomprimido, &tamanoAnexoDescomprimido, punteroAnexo, tamanoAnexoComprimido)){
					PRINTF_ERROR("descomprimiendo el mapa alpha anexo a JPEG.\n");
					NBGestorMemoria::liberarMemoria(anexoDescomprimido);
				} else {
					descripcion->tamanoAnexo 	= (IxaUI32)tamanoAnexoDescomprimido;
					descripcion->punteroAnexo 	= anexoDescomprimido;
				}
			}
		}*/
		ixaSwfDictAddCharacter(swf, ENIXA_CharType_FileBits, charID, filebin);
		return IXA_TRUE;
	} else if(tagType==ENIXA_TagID_DefineBitsJPEG4){
		//JPEG COMPLETO CON BITMAP GRAYSACLE ANEXO (para el alpha)
		STIXA_FileBits* filebin; long tamanoAnexoComprimido;
		IxaUI16 charID; IxaUI32 bytesImg; IxaUI16 deblockParam; IxaUI32 bytesIgnorar = 0;
		IXA_MALLOC(filebin, STIXA_FileBits, sizeof(STIXA_FileBits), "FileBinary")
		ixaFileBinaryInit(filebin);
		__ixaReaderGetBytes(rdr, &charID, sizeof(charID));
		__ixaReaderGetBytes(rdr, &bytesImg, sizeof(bytesImg));
		__ixaReaderGetBytes(rdr, &deblockParam, sizeof(deblockParam));
		filebin->tipo 			= ixaBitmapFileType(__ixaReaderCurrentByte(rdr), &bytesIgnorar); __ixaReaderIgnoreBytes(rdr, bytesIgnorar);
		filebin->tamano			= bytesImg - bytesIgnorar;
		IXA_MALLOC(filebin->puntero, IxaBYTE, filebin->tamano, "FileBinary.puntero") __ixaReaderGetBytes(rdr, filebin->puntero, filebin->tamano);
		tamanoAnexoComprimido 	= bytesTag - sizeof(charID) - sizeof(bytesImg) - bytesIgnorar - filebin->tamano;
		//BYTE* punteroAnexo	= __ixaReaderCurrentByte(rdr);
		__ixaReaderIgnoreBytes(rdr, (int)tamanoAnexoComprimido);
		//Intentar descomprimir el anexo
		/*STIXA_Size tamanoJPEG			= ixaBitmapSize(descripcion->puntero, descripcion->tamano, descripcion->tipo);
		if(tamanoJPEG.ancho==0.0 || tamanoJPEG.alto==0.0){
			PRINTF_ERROR("no se pudo extraer tamano de JPEG en ENIXA_TagID_DefineBitsJPEG3: ancho(%d) alto(%d)\n", (int)tamanoJPEG.ancho, (int)tamanoJPEG.alto);
		} else {
			if(tamanoAnexoComprimido>0){
				long tamanoAnexoDescomprimido = (long)(tamanoJPEG.ancho * tamanoJPEG.alto);
				BYTE* anexoDescomprimido = (BYTE*)NBGestorMemoria::reservarMemoria(tamanoAnexoDescomprimido, (ENMemoriaTipo)this->_tipoMemoriaResidencia); NB_DEFINE_NOMBRE_PUNTERO(anexoDescomprimido, "AUSwf::anexoDescomprimido")
				if(0 != ezuncompress(anexoDescomprimido, &tamanoAnexoDescomprimido, punteroAnexo, tamanoAnexoComprimido)){
					PRINTF_ERROR("descomprimiendo el mapa alpha anexo a JPEG.\n");
					NBGestorMemoria::liberarMemoria(anexoDescomprimido);
				} else {
					descripcion->tamanoAnexo 	= (IxaUI32)tamanoAnexoDescomprimido;
					descripcion->punteroAnexo 	= anexoDescomprimido;
				}
			}
		}*/
		ixaSwfDictAddCharacter(swf, ENIXA_CharType_FileBits, charID, filebin);
		return IXA_TRUE;
	} else if(tagType==ENIXA_TagID_DefineBinaryData){
		//ARCHIVO BINARIO ARBITRARIO
		//JPEG COMPLETO
		STIXA_FileBits* filebin; IxaUI16 charID; IxaUI32 resev; IxaUI32 bytesIgnorar = 0;
		IXA_MALLOC(filebin, STIXA_FileBits, sizeof(STIXA_FileBits), "FileBinary")
		ixaFileBinaryInit(filebin);
		__ixaReaderGetBytes(rdr, &charID, sizeof(charID));
		__ixaReaderGetBytes(rdr, &resev, sizeof(resev));
		filebin->tipo 		= ENIXA_FileTypeBINARIO;
		filebin->tamano		= bytesTag - sizeof(charID) - sizeof(resev) - bytesIgnorar;
		IXA_MALLOC(filebin->puntero, IxaBYTE, filebin->tamano, "FileBinary.puntero") __ixaReaderGetBytes(rdr, filebin->puntero, filebin->tamano);
		filebin->tamanoAnexo = 0;
		filebin->punteroAnexo = NULL;
		ixaSwfDictAddCharacter(swf, ENIXA_CharType_FileBits, charID, filebin);
		return IXA_TRUE;
	}
	return IXA_FALSE;
}

IxaBOOL __ixaSwfDictLoadBitmap(STIXA_SwfFile* swf, STIXA_Reader* rdr, ENIXA_TagID tagType, IxaSI32 bytesTag, PTRIXA_zlibUncompress funcUncompress){
	IxaBOOL r = IXA_FALSE;
	if(tagType == ENIXA_TagID_DefineBitsLossless || tagType == ENIXA_TagID_DefineBitsLossless2){
		//Extraer propiedades y datos comprimidos
		IxaUI8 formatoBitMap; IxaUI16 charID, ancho, alto;
		IxaUI32 bytesDatosComprimidos = bytesTag, tamanoPaleta = 0;
		const IxaBYTE* datosComprimidos; ENIXA_Color color; IxaUI8 bytesPorColorPixel, bytesPorColorPaleta;
		__ixaReaderGetBytes(rdr, &charID, sizeof(charID)); bytesDatosComprimidos -= sizeof(charID);
		__ixaReaderGetBytes(rdr, &formatoBitMap, sizeof(formatoBitMap)); bytesDatosComprimidos -= sizeof(formatoBitMap);
		__ixaReaderGetBytes(rdr, &ancho, sizeof(ancho)); bytesDatosComprimidos -= sizeof(ancho);
		__ixaReaderGetBytes(rdr, &alto, sizeof(alto)); bytesDatosComprimidos -= sizeof(alto);
		if(formatoBitMap==3) { //8bits with color palete
			IxaUI8 tamanoPaletaMenosUno = 0;
			__ixaReaderGetBytes(rdr, &tamanoPaletaMenosUno, sizeof(tamanoPaletaMenosUno));
			bytesDatosComprimidos -= sizeof(tamanoPaletaMenosUno);
			tamanoPaleta	= tamanoPaletaMenosUno + 1;
		}
		//PRINTF_INFO("TamBitmap (%d x %d) formato(%d)\n", ancho, alto, formatoBitMap);
		datosComprimidos	= __ixaReaderCurrentByte(rdr); __ixaReaderIgnoreBytes(rdr, bytesDatosComprimidos);
		//Descomprimir
		color				= ENIXA_ColorUnefined;
		bytesPorColorPixel 	= 0;
		bytesPorColorPaleta	= 0;
		if(formatoBitMap==3){	//8-bits with palette
			color 				= (tagType==ENIXA_TagID_DefineBitsLossless2 ? ENIXA_ColorRGBA8 : ENIXA_ColorRGB8);
			bytesPorColorPixel 	= 1;
			bytesPorColorPaleta	= (tagType==ENIXA_TagID_DefineBitsLossless2 ? 4 : 3);
		} else if(formatoBitMap==4){ //mapa de 16-bits (1 bit reservado, y 5 bits por cada color)
			color				= ENIXA_ColorPIX15;
			bytesPorColorPixel 	= 2;
		} else if(formatoBitMap==5){ //mapa de TrueColor de 24 o 32 bits
			color				= (tagType==ENIXA_TagID_DefineBitsLossless2 ? ENIXA_ColorARGB8 : ENIXA_ColorPIX24);
			bytesPorColorPixel 	= (tagType==ENIXA_TagID_DefineBitsLossless2 ? 4 : 4);
		} else {
			PRINTF_INFO("Error, el BitmapFormat '%d' no forma parte de las especificaciones SWF\n", formatoBitMap);
		}
		if(bytesPorColorPixel!=0){
			IxaUI32 bytesPorLinea, tamanoDatosDescomprimidos;
			IxaBYTE* datosDescomprimidos;
			bytesPorLinea				= (ancho * bytesPorColorPixel); while((bytesPorLinea % 4) != 0){bytesPorLinea++;} //pad to 32 bits
			tamanoDatosDescomprimidos	= (tamanoPaleta * bytesPorColorPaleta) + (bytesPorLinea * alto);
			IXA_MALLOC(datosDescomprimidos, IxaBYTE, sizeof(IxaBYTE) * tamanoDatosDescomprimidos, "STIXA_Bitmap.uncompressData");
			if(!((*funcUncompress)(datosDescomprimidos, tamanoDatosDescomprimidos, datosComprimidos, bytesDatosComprimidos))){
				IXA_FREE(datosDescomprimidos)
			} else {
				STIXA_Bitmap* newLossless;
				IXA_MALLOC(newLossless, STIXA_Bitmap, sizeof(STIXA_Bitmap), "newLossless")
				ixaBitmapInit(newLossless);
				newLossless->bitmapformat	= formatoBitMap;
				newLossless->color			= color;
				newLossless->width			= ancho;
				newLossless->height			= alto;
				newLossless->paletteSize		= tamanoPaleta;
				newLossless->bytesPerPixel	= bytesPorColorPixel;
				newLossless->bytesPerColorPalette = bytesPorColorPaleta;
				newLossless->bytesPerLine	= bytesPorLinea;
				newLossless->uncompressData	= datosDescomprimidos;
				newLossless->bitmapPointer	= &(datosDescomprimidos[bytesPorColorPaleta * tamanoPaleta]);
				newLossless->palettePointer	= (tamanoPaleta!=0? datosDescomprimidos : NULL);
				ixaSwfDictAddCharacter(swf, ENIXA_CharType_Bitmap, charID, newLossless);
				r = IXA_TRUE;
			}
		}
	}
	return r;
}

IxaBOOL __ixaSwfDictLoadShape(STIXA_SwfFile* swf, STIXA_Reader* rdr, const ENIXA_TagID tagType, const IxaSI32 bytesTag){
	IxaBOOL r = IXA_FALSE;
	if(bytesTag>0 && (tagType==ENIXA_TagID_DefineShape || tagType==ENIXA_TagID_DefineShape2 || tagType==ENIXA_TagID_DefineShape3 || tagType==ENIXA_TagID_DefineShape4)){
		//
		IxaUI16 idForma; STIXA_Rect limitesFigura; ENIXA_Shape tipoForma; STIXA_Shape* newShape;
		__ixaReaderGetBytes(rdr, &idForma, sizeof(idForma)); //PRINTF_INFO("   Id forma(%d)\n", idForma);
		limitesFigura = __ixaLoadRECT(rdr); //PRINTF_INFO("   Limites de figura (%d, %d, %d, %d) dimensiones(%d, %d)\n", limitesFigura.Xmin, limitesFigura.Ymin, limitesFigura.Xmax, limitesFigura.Ymax, limitesFigura.Xmax - limitesFigura.Xmin + 1, limitesFigura.Ymax - limitesFigura.Ymin + 1);
		if(tagType==ENIXA_TagID_DefineShape4){
			/*STIXA_Rect limitesSinBordes =*/ __ixaLoadRECT(rdr);
			__ixaReaderGetBitsUnsigned(rdr, 5); 	//reservado
			__ixaReaderGetBitsUnsigned(rdr, 1);	//usaReglaRellenoWinding
			__ixaReaderGetBitsUnsigned(rdr, 1);	//usaStrokesSinEscalar
			__ixaReaderGetBitsUnsigned(rdr, 1);	//usaStrokeEscalados
			//PRINTF_INFO("   Limites de bordes (%d, %d, %d, %d)\n", limitesBordes.Xmin, limitesBordes.Ymin, limitesBordes.Xmax, limitesBordes.Ymax);
		}
		//AUMapaBitsLienzo* lienzo = new(this) AUMapaBitsLienzo(limitesFigura.Xmax-limitesFigura.Xmin+1, limitesFigura.Ymax-limitesFigura.Ymin+1);
		//lienzo->formatearLienzo(255, 255, 255, 255);
		//PRINTF_INFO("   Lienzo de %d KBs\n", (int)((((limitesFigura.Xmax-limitesFigura.Xmin+1) * limitesFigura.Ymax-limitesFigura.Ymin+1) * 6) / 1024));
		//SHAPEWITHSTYLE
		tipoForma = ENIXA_ShapeNO_DEFINIDA;
		if(tagType==ENIXA_TagID_DefineShape){
			tipoForma = ENIXA_Shape1;
		} else if(tagType==ENIXA_TagID_DefineShape2){
			tipoForma = ENIXA_Shape2;
		} else if(tagType==ENIXA_TagID_DefineShape3){
			tipoForma = ENIXA_Shape3;
		} else if(tagType==ENIXA_TagID_DefineShape4){
			tipoForma = ENIXA_Shape4;
		}
		IXA_MALLOC(newShape, STIXA_Shape, sizeof(STIXA_Shape), "newShape")
		ixaShapeInit(newShape);
		__ixaLoadSHAPE(swf, rdr, newShape, tipoForma, IXA_TRUE);
		newShape->xMin = limitesFigura.Xmin; newShape->xMax = limitesFigura.Xmax;
		newShape->yMin = limitesFigura.Ymin; newShape->yMax = limitesFigura.Ymax;
		//PRINTF_INFO("ShapeBounds x(%d, %d) y(%d, %d)\n", newShape->xMin, newShape->xMax, newShape->yMin, newShape->yMax);
		//Add to SWF dictionary
		ixaSwfDictAddCharacter(swf, ENIXA_CharType_Shape, idForma, newShape);
		r = IXA_TRUE;
	}
	IXA_ASSERT(r)
	return r;
}

IxaBOOL __ixaSwfDictLoadFont(STIXA_SwfFile* swf, STIXA_Reader* rdr, const ENIXA_TagID tagType, const IxaSI32 bytesTag){
	IxaBOOL r = IXA_FALSE;
	//if(this->_encabezado.version>=7) PRINTF_WARNING("considere usar archivos SWF 6 y previos para la definicion de fuentes (especifican las cajas de glyphs)\n");
	if(bytesTag > 0 && (tagType==ENIXA_TagID_DefineFont || tagType==ENIXA_TagID_DefineFont2 || tagType==ENIXA_TagID_DefineFont3)){ //ENIXA_TagID_DefineFont4 no esta soportado (fuentes dinamicas en formato CFF)
		//Nota: ENIXA_TagID_DefineFont3 multiplica por 20 las cajas EM de los glyphs
		const IxaUI16 fontCharID = __ixaReaderGetUI16(rdr); //PRINTF_INFO("   fontID: %d\n", idFuente);
		if(tagType==ENIXA_TagID_DefineFont){
			//tabla de shapes
			IxaUI16 i;
			IxaUI16 offsetPrimerShape 	= __ixaReaderGetUI16(rdr);
			IxaUI16 conteoGlyphs		= (offsetPrimerShape/2); //PRINTF_INFO("Glyphs: %d\n", conteoGlyphs);
			IxaUI32 posTablaOffsets		= rdr->_posicionEnFlujo;
			IxaUI32* glyphsOffsets;
			IXA_MALLOC(glyphsOffsets, IxaUI32, sizeof(IxaUI32) * (conteoGlyphs + 1), "glyphsOffsets")
			glyphsOffsets[0]			= offsetPrimerShape;
			//PRINTF_INFO("Glyph offset[%d]: %d\n", 0, glyphsOffsets[0]);
			for(i=1; i<conteoGlyphs; i++){ //read the other offsets (the first was already read)
				glyphsOffsets[i] = __ixaReaderGetUI16(rdr);
				//PRINTF_INFO("Glyph offset[%d]: %d\n", i, glyphsOffsets[i]);
			}
			for(i=0; i<conteoGlyphs; i++){ //consumir los shapes
				STIXA_Shape glyphShape;
				ixaShapeInit(&glyphShape);
				//asegurar la posicion segun tabla de offsets
				__ixaReaderIgnoreBytes(rdr, glyphsOffsets[i] - (rdr->_posicionEnFlujo-posTablaOffsets));
				//GLYPH/SHAPE
				__ixaLoadSHAPE(swf, rdr, &glyphShape, ENIXA_Shape1, IXA_FALSE);
				//Ignore shape
				ixaShapeFinalize(&glyphShape);
			}
			if(glyphsOffsets!=NULL){ IXA_FREE(glyphsOffsets); glyphsOffsets = NULL; }
			r = IXA_TRUE;
		} else if(tagType==ENIXA_TagID_DefineFont2 || tagType==ENIXA_TagID_DefineFont3){
			STIXA_Font* newFont;
			IxaUI32 tienePlantilla;
			IxaUI32 offsetsAnchos;
			IxaUI32 codigosAnchos;
			IxaUI32 estiloCurvada;
			IxaUI32 estiloNegrilla;
			IxaUI8	tamanoNombreFuente;
			IxaUI16 conteoGlyphs;
			IxaSI32 posTablaOffsets;
			IxaUI32* glyphsOffsets;
			IxaUI16 i;
			IXA_MALLOC(newFont, STIXA_Font, sizeof(STIXA_Font), "STIXA_Font")
			ixaFontInit(newFont);
			newFont->emBoxSize	= (tagType==ENIXA_TagID_DefineFont3 ? (1024*20) : 1024); //Font3's emBox is 20 times larger
			tienePlantilla		= __ixaReaderGetBitsUnsigned(rdr, 1);
			/*IxaUI32 tieneShiftJIS	=*/ __ixaReaderGetBitsUnsigned(rdr, 1);
			/*IxaUI32 textoPequeno	=*/ __ixaReaderGetBitsUnsigned(rdr, 1);
			/*IxaUI32 esANSI	=*/ __ixaReaderGetBitsUnsigned(rdr, 1);
			offsetsAnchos		= __ixaReaderGetBitsUnsigned(rdr, 1);
			codigosAnchos		= __ixaReaderGetBitsUnsigned(rdr, 1);
			estiloCurvada		= __ixaReaderGetBitsUnsigned(rdr, 1);
			estiloNegrilla		= __ixaReaderGetBitsUnsigned(rdr, 1);
			/*IxaUI8 codigoDeLenguaje	=*/ __ixaReaderGetUI8(rdr);	//LANGCODE
			tamanoNombreFuente	= __ixaReaderGetUI8(rdr);
			IXA_MALLOC(newFont->fontName, char, sizeof(char) * (tamanoNombreFuente + 1), "STIXA_Font.fontName")
			{
				IxaUI8 i;
				for(i=0; i<tamanoNombreFuente; i++) newFont->fontName[i] = __ixaReaderGetUI8(rdr);
				newFont->fontName[i]	= '\0';
				//PRINTF_INFO("Font name: '%s'\n", newFont->fontName);
			}
			newFont->isBold			= (estiloNegrilla != 0 ? IXA_TRUE : IXA_FALSE);
			newFont->isItalic		= (estiloCurvada != 0 ? IXA_TRUE : IXA_FALSE);
			//PRINTF_INFO("   fontName: '%s'\n", nombreFuente->str());
			conteoGlyphs			= __ixaReaderGetUI16(rdr);		//es cero en caso de fuentes del sistema (sin Glyphs)
			posTablaOffsets			= rdr->_posicionEnFlujo;
			IXA_MALLOC(glyphsOffsets, IxaUI32, sizeof(IxaUI32) * (conteoGlyphs + 1), "glyphsOffsets");
			for(i=0; i<conteoGlyphs; i++){						//leer los offsets
				if(offsetsAnchos==1){
					glyphsOffsets[i] = __ixaReaderGetUI32(rdr);
				} else {
					glyphsOffsets[i] = __ixaReaderGetUI16(rdr);
				}
			}
			//IxaUI32 offsetTablaCodigos;
			if(offsetsAnchos==1){
				/*offsetTablaCodigos =*/ __ixaReaderGetUI32(rdr);
			} else {
				/*offsetTablaCodigos =*/ __ixaReaderGetUI16(rdr);
			}
			newFont->glyphsArrSize	= conteoGlyphs + 1;
			newFont->glyphsArrUse	= conteoGlyphs;
			IXA_MALLOC(newFont->glyphsArr, STIXA_Glyph, sizeof(STIXA_Glyph) * newFont->glyphsArrSize, "STIXA_Font.glyphsArr")
			for(i=0; i<conteoGlyphs; i++){ //consumir los shapes
				IXA_ASSERT(i<conteoGlyphs)
				ixaGlyphInit(&newFont->glyphsArr[i]);
				//asegurar la posicion segun tabla de offsets
				__ixaReaderIgnoreBytes(rdr, glyphsOffsets[i] - (rdr->_posicionEnFlujo - posTablaOffsets));
				//GLYPH-SHAPE
				__ixaLoadSHAPE(swf, rdr, &newFont->glyphsArr[i].shape, (tagType==ENIXA_TagID_DefineFont2 ? ENIXA_Shape2 : ENIXA_Shape3), IXA_FALSE);
			}
			for(i=0; i<conteoGlyphs; i++){ //consumir los codigos
				IXA_ASSERT(i<conteoGlyphs)
				if(codigosAnchos==1 || tagType==ENIXA_TagID_DefineFont3){ //en el DefineFont3 siempre son IxaUI16, en el DefineFont2 pueden ser IxaUI16 o IxaUI8
					newFont->glyphsArr[i].code 	= __ixaReaderGetUI16(rdr);
				} else {
					newFont->glyphsArr[i].code 	= __ixaReaderGetUI8(rdr);
				}
				newFont->glyphsArr[i].advace	= 0;
				//PRINTF_INFO("Codigo de glyph %d: %d\n", i, (IxaSI32)codigos[i]);
			}
			if(tienePlantilla==1){
				//PRINTF_INFO("   SI tiene plantilla\n");
				IxaUI16 espacioKerning;
				newFont->ascendent		= __ixaReaderGetSI16(rdr);
				newFont->descendent		= __ixaReaderGetSI16(rdr);
				newFont->lineHeightGuide = __ixaReaderGetSI16(rdr);
				for(i=0; i<conteoGlyphs; i++){
					newFont->glyphsArr[i].advace = __ixaReaderGetSI16(rdr);
				}
				for(i=0; i<conteoGlyphs; i++){						//no usado de Flash PLayer 7 en adelante
					STIXA_Rect limitesFuente = __ixaLoadRECT(rdr);
					if(swf->_encabezado.version<7 && limitesFuente.Xmin!=limitesFuente.Xmax && limitesFuente.Ymin!=limitesFuente.Ymax){
						//PRINTF_INFO("Limites de glyph: (%d, %d)-(%d, %d)\n", limitesFuente.Xmin, limitesFuente.Ymin, limitesFuente.Xmax, limitesFuente.Ymax);
						newFont->glyphsArr[i].shape.xMin = limitesFuente.Xmin;
						newFont->glyphsArr[i].shape.xMax = limitesFuente.Xmax;
						newFont->glyphsArr[i].shape.yMin = limitesFuente.Ymin;
						newFont->glyphsArr[i].shape.yMax = limitesFuente.Ymax;
					}
				}
				espacioKerning		= __ixaReaderGetUI16(rdr);	//no usado de Flash PLayer 7 en adelante
				for(i=0; i<espacioKerning; i++){					//no usado de Flash PLayer 7 en adelante
					/*STIXA_Kerning kerning =*/ __ixaLoadKERNING(rdr, (codigosAnchos == 1 ? IXA_TRUE : IXA_FALSE));
				}
			}
			//Add to SWF dictionary
			ixaSwfDictAddCharacter(swf, ENIXA_CharType_Font, fontCharID, newFont);
			if(glyphsOffsets!=NULL){ IXA_FREE(glyphsOffsets); glyphsOffsets = NULL; }
			r = IXA_TRUE;
		}
	}
	IXA_ASSERT(r)
	return r;
}

IxaBOOL __ixaSwfDictLoadSound(STIXA_SwfFile* swf, STIXA_Reader* rdr, const ENIXA_TagID tagType, const IxaSI32 bytesTag){
	IxaBOOL r = IXA_FALSE;
	if(tagType==ENIXA_TagID_DefineSound){
		IxaUI16 charID; STIXA_Sound* newSound;	IXA_MALLOC(newSound, STIXA_Sound, sizeof(STIXA_Sound), "STIXA_Sound")
		ixaSoundInit(newSound);
		charID					= __ixaReaderGetUI16(rdr);
		newSound->soundFormat	= __ixaReaderGetBitsUnsigned(rdr, 4); //3 = uncompressed, little-endian
		newSound->samplerateType = __ixaReaderGetBitsUnsigned(rdr, 2); //0=5.5KHz, 1=11KHz, 2=22KHz, 3=44KHz
		newSound->sampleBitsType = __ixaReaderGetBitsUnsigned(rdr, 1); //0=8 bits, 1=16 bits
		newSound->channelsType	= __ixaReaderGetBitsUnsigned(rdr, 1); //0=Mono, 1=Stereo
		newSound->samplesCount	= __ixaReaderGetUI32(rdr); //PRINTF_INFO("Conteo de muestras: %u\n", conteoMuestras);
		newSound->soundDataSize	= bytesTag - 7; //el encabezado suma 7 bytes en total
		if(newSound->soundDataSize!=0){
			IXA_MALLOC(newSound->soundData, IxaBYTE, sizeof(IxaBYTE) * newSound->soundDataSize, "STIXA_Sound.soundData")
			__ixaReaderGetBytes(rdr, newSound->soundData, newSound->soundDataSize);
		}
		ixaSwfDictAddCharacter(swf, ENIXA_CharType_Sound, charID, newSound);
		r = IXA_TRUE;
	}
	return r;
}

IxaBOOL __ixaSwfFramePlaceObject(STIXA_SwfFile* swf, STIXA_Reader* rdr, STIXA_Sprite* sprite, const ENIXA_TagID tagType, const IxaSI32 bytesTag){
	IxaBOOL r = IXA_FALSE;
	if(tagType==ENIXA_TagID_PlaceObject2){
		IxaUI32 hasClipActions 		= __ixaReaderGetBitsUnsigned(rdr, 1);
		IxaUI32 hasClipDepth		= __ixaReaderGetBitsUnsigned(rdr, 1);
		IxaUI32 hasName				= __ixaReaderGetBitsUnsigned(rdr, 1);
		IxaUI32 hasRatio			= __ixaReaderGetBitsUnsigned(rdr, 1);
		IxaUI32 hasColorTransform	= __ixaReaderGetBitsUnsigned(rdr, 1);
		IxaUI32 hasMatrix			= __ixaReaderGetBitsUnsigned(rdr, 1);
		IxaUI32 hasCharacter		= __ixaReaderGetBitsUnsigned(rdr, 1);
		IxaUI32 move				= __ixaReaderGetBitsUnsigned(rdr, 1);
		IxaUI16 depth				= __ixaReaderGetUI16(rdr);
		IxaUI16 charID				= 0;
		char* name					= NULL;
		STIXA_Matrix* matrizAplicar	= NULL;
		STIXA_Matrix matriz;
		STIXA_CXForm* colorAplicar	= NULL;
		STIXA_CXForm colorM;
		//
		if(hasCharacter==1){
			charID				= __ixaReaderGetUI16(rdr);
		}
		if(hasMatrix==1){
			matriz				= __ixaLoadMATRIX(rdr);
			matrizAplicar 		= &matriz;
		}
		if(hasColorTransform==1){
			colorM 				= __ixaLoadCXFORM(rdr, IXA_TRUE);
			colorAplicar		= &colorM;
		}
		if(hasRatio==1){
			/*IxaUI16 ratio		=*/ __ixaReaderGetUI16(rdr);
		}
		if(hasName==1){
			name				= __ixaLoadSTRING(rdr);
		}
		if(hasClipDepth){
			/*IxaUI16 clipDepth	=*/ __ixaReaderGetUI16(rdr);
		}
		IXA_ASSERT(rdr->_conteoTmpBytesConsumidos <= bytesTag)
		if(hasClipActions==1){
			IxaUI32 thisActFlag;
			IxaUI32 thisActRecSize;
			/*IxaUI16 reservado1		=*/ __ixaReaderGetUI16(rdr);
			/*IxaUI32 allActionsFlags	=*/ __ixaLoadCLIPEVENTFLAGS(rdr, swf->_encabezado.version);
			do {
				thisActFlag			= __ixaLoadCLIPEVENTFLAGS(rdr, swf->_encabezado.version);
				if(thisActFlag!=0){
					thisActRecSize = __ixaReaderGetUI32(rdr);
					__ixaReaderIgnoreBytes(rdr, thisActRecSize);
				}
				IXA_ASSERT(rdr->_conteoTmpBytesConsumidos <= bytesTag)
			} while(thisActFlag!=0);
		}
		if(hasCharacter==1){
			if(move==0){
				//PRINTF_INFO("   AddObjectInDepth(%d)\n", depth);
				ixaSpriteObjectAdd(sprite, depth, charID, name);
			} else {
				//PRINTF_INFO("   ReplaceObjectInDepth(%d)\n", depth);
				ixaSpriteObjectReplace(sprite, depth, charID, name); //al reemplazar el objeto se conservan sus propiedades en escena
			}
		}
		ixaSpriteObjectSetProps(sprite, depth, matrizAplicar, colorAplicar);
		//
		if(name!=NULL){ IXA_FREE(name); name = NULL; }
		r = IXA_TRUE;
	}
	return r;
}

IxaBOOL __ixaSwfFrameRemoveObject(STIXA_SwfFile* swf, STIXA_Reader* rdr, STIXA_Sprite* sprite, const ENIXA_TagID tagType, const IxaSI32 bytesTag){
	IxaBOOL r = IXA_FALSE;
	if(tagType==ENIXA_TagID_RemoveObject || tagType==ENIXA_TagID_RemoveObject2){
		IxaUI16 depth;
		if(tagType==ENIXA_TagID_RemoveObject){
			/*IxaUI16 charID =*/ __ixaReaderGetUI16(rdr);
		}
		depth = __ixaReaderGetUI16(rdr);
		ixaSpriteObjectRemove(sprite, depth);
		r = IXA_TRUE;
	}
	return r;
}


char* __ixaLoadSTRING(STIXA_Reader* rdr){
	IxaUI16 buffUse	= 0;
	IxaUI16 buffSize	= 32;
	char* buff;
	IxaUI8 c;
	IXA_MALLOC(buff, char, sizeof(char) * buffSize, "swfSTRING")
	do {
		c = __ixaReaderGetUI8(rdr);
		if(buffUse==buffSize) ixaArrayResize((void**)&buff, &buffUse, &buffSize, sizeof(char), 32);
		buff[buffUse++] = c;
	} while(c!=0);
	//Last char must be NULL
	if(c!='\0'){
		if(buffUse==buffSize) ixaArrayResize((void**)&buff, &buffUse, &buffSize, sizeof(char), 32);
		buff[buffUse++] = '\0';
	}
	return buff;
}

STIXA_Rect __ixaLoadRECT(STIXA_Reader* rdr){
	STIXA_Rect rect;
	__ixaReaderPositionInByteAligned(rdr);
	rect.Nbits 	= __ixaReaderGetBitsUnsigned(rdr, 5);
	rect.Xmin 	= __ixaReaderGetBitsSigned(rdr, rect.Nbits);
	rect.Xmax 	= __ixaReaderGetBitsSigned(rdr, rect.Nbits);
	rect.Ymin 	= __ixaReaderGetBitsSigned(rdr, rect.Nbits);
	rect.Ymax 	= __ixaReaderGetBitsSigned(rdr, rect.Nbits);
	return rect;
}

STIXA_Kerning __ixaLoadKERNING(STIXA_Reader* rdr, const IxaBOOL wideCodes){
	STIXA_Kerning kerning;
	if(wideCodes){
		kerning.codigo1	= __ixaReaderGetUI16(rdr);
		kerning.codigo2	= __ixaReaderGetUI16(rdr);
	} else {
		kerning.codigo1	= __ixaReaderGetUI8(rdr);
		kerning.codigo2	= __ixaReaderGetUI8(rdr);
	}
	kerning.ajuste		= __ixaReaderGetBitsSigned(rdr, 16);
	return kerning;
}

STIXA_Matrix __ixaLoadMATRIX(STIXA_Reader* rdr){
	STIXA_Matrix matriz;
	__ixaReaderPositionInByteAligned(rdr);	//lo especifica la documentacion
	//
	//PRINTF_INFO("--------------MATRIZ--------------\n");
	matriz.tieneEscala 			= __ixaReaderGetBitsUnsigned(rdr, 1);
	if(matriz.tieneEscala == 1){
		matriz.bitsPorEscala 	= __ixaReaderGetBitsUnsigned(rdr, 5);
		matriz.escalaX			= __ixaReaderGetBitsFixed16(rdr, matriz.bitsPorEscala);
		matriz.escalaY			= __ixaReaderGetBitsFixed16(rdr, matriz.bitsPorEscala);
		//PRINTF_INFO("      Matriz: tiene escalacion (%d bits cada una) x(%f) y(%f)\n", matriz.bitsPorEscala, matriz.escalaX, matriz.escalaY);
	} else {
		matriz.bitsPorEscala 	= 0;
		matriz.escalaX			= 1.0;
		matriz.escalaY			= 1.0;
	}
	matriz.tieneRotacion		= __ixaReaderGetBitsUnsigned(rdr, 1);
	if(matriz.tieneRotacion == 1){
		matriz.bitsPorRotacion	= __ixaReaderGetBitsUnsigned(rdr, 5);
		matriz.skew0		= __ixaReaderGetBitsFixed16(rdr, matriz.bitsPorRotacion);
		matriz.skew1		= __ixaReaderGetBitsFixed16(rdr, matriz.bitsPorRotacion);
		//PRINTF_INFO("      Matriz: tiene rotacion (%d bits cada una) skew0(%f) skew1(%f)\n", matriz.bitsPorRotacion, matriz.rotacion0, matriz.rotacion1);
	} else {
		matriz.bitsPorRotacion	= 0;
		matriz.skew0			= 0.0;
		matriz.skew1			= 0.0;
	}
	matriz.bitsPorTranslacion	= __ixaReaderGetBitsUnsigned(rdr, 5);
	matriz.traslacionX			= __ixaReaderGetBitsSigned(rdr, matriz.bitsPorTranslacion);
	matriz.traslacionY			= __ixaReaderGetBitsSigned(rdr, matriz.bitsPorTranslacion);
	//PRINTF_INFO("      Matriz: traslacion (%d, %d) (%d bits cada una)\n", matriz.traslacionX, matriz.traslacionY, matriz.bitsPorTranslacion);
	return matriz;
}

STIXA_Gradient __ixaLoadGRADIENT(STIXA_Reader* rdr, const ENIXA_Shape tipoForma, const IxaBOOL isFocal){
	STIXA_Gradient gradiente; IxaUI32 i;
	__ixaReaderPositionInByteAligned(rdr);	//no lo especifica la documentacion
	gradiente.spreadMode 		= __ixaReaderGetBitsUnsigned(rdr, 2);
	gradiente.interpolationMode	= __ixaReaderGetBitsUnsigned(rdr, 2);
	gradiente.numGradients		= __ixaReaderGetBitsUnsigned(rdr, 4);
	//PRINTF_INFO("      Gradiente: %s\n", (esFocal?"FOCAL": "NO FOCAL"));
	//PRINTF_INFO("      Gradiente: spreadMode(%d) interpolationMode(%d) numGradients(%d)\n", gradiente.spreadMode, gradiente.interpolationMode, gradiente.numGradients);
	//registros
	for(i=0; i<gradiente.numGradients; i++){
		__ixaReaderGetUI8(rdr);		//record::ratio
		if(tipoForma == ENIXA_Shape1 || tipoForma == ENIXA_Shape2){
			/*BYTE r =*/ __ixaReaderGetUI8(rdr);	//record::color::r
			/*BYTE g =*/ __ixaReaderGetUI8(rdr);	//record::color::g
			/*BYTE b =*/ __ixaReaderGetUI8(rdr);	//record::color::b
			//PRINTF_INFO("      Gradiente: colorRGB(%d, %d, %d)\n", r, g, b);
		} else {
			/*BYTE r =*/ __ixaReaderGetUI8(rdr);	//record::color::r
			/*BYTE g =*/ __ixaReaderGetUI8(rdr);	//record::color::g
			/*BYTE b =*/ __ixaReaderGetUI8(rdr);	//record::color::b
			/*BYTE a =*/ __ixaReaderGetUI8(rdr);	//record::color::a
			//PRINTF_INFO("      Gradiente: colorRGB(%d, %d, %d, %d)\n", r, g, b, a);
		}
	}
	if(isFocal){
		gradiente.focalPoint 	= (__ixaReaderGetUI16(rdr));
		//PRINTF_INFO("      Gradiente: puntoFocal(%d)\n", gradiente.focalPoint);
	} else {
		gradiente.focalPoint	= 0;
	}
	return gradiente;
}

STIXA_StyleLine __ixaLoadLINESTYLE(STIXA_Reader* rdr, const ENIXA_Shape formType, const IxaBOOL isLineStyle2){
	STIXA_StyleLine estiloLinea;
	estiloLinea.anchoLinea 	= (__ixaReaderGetUI16(rdr));
	estiloLinea.color.r 	= 0;
	estiloLinea.color.g 	= 0;
	estiloLinea.color.b 	= 0;
	estiloLinea.color.a 	= 0;
	//PRINTF_INFO("      Estilo linea: ancho(%d)\n", estiloLinea.anchoLinea);
	if(isLineStyle2){
		IxaUI32 jointStyle, hasFill;
		__ixaReaderGetBitsUnsigned(rdr, 2);	//StartCapStyle
		jointStyle	=__ixaReaderGetBitsUnsigned(rdr, 2);	//JointStyle
		hasFill 	= __ixaReaderGetBitsUnsigned(rdr, 1);	//HasFillFlag
		__ixaReaderGetBitsUnsigned(rdr, 1);	//NoHScaleFlag
		__ixaReaderGetBitsUnsigned(rdr, 1);	//NoVScaleFlag
		__ixaReaderGetBitsUnsigned(rdr, 1);	//PixelHintingFlag
		__ixaReaderGetBitsUnsigned(rdr, 5);	//Reserved
		__ixaReaderGetBitsUnsigned(rdr, 1);	//NoClose
		__ixaReaderGetBitsUnsigned(rdr, 2);	//EndCapStyle
		if(jointStyle==2){
			__ixaReaderGetUI16(rdr);			//MiterLimitFactor
		}
		if(hasFill==0){
			estiloLinea.color.r = __ixaReaderGetUI8(rdr);	//color::r
			estiloLinea.color.g = __ixaReaderGetUI8(rdr);	//color::g
			estiloLinea.color.b = __ixaReaderGetUI8(rdr);	//color::b
			estiloLinea.color.a = __ixaReaderGetUI8(rdr);	//color::a
			//PRINTF_INFO("      Estilo linea: colorRGB(%d, %d, %d, %d)\n", estiloLinea.color.r, estiloLinea.color.g, estiloLinea.color.b, estiloLinea.color.a);
		} else {
			__ixaLoadFILLSTYLE(rdr, formType);
		}
	} else {
		if(formType == ENIXA_Shape1 || formType == ENIXA_Shape2){
			estiloLinea.color.r = __ixaReaderGetUI8(rdr);	//color::r
			estiloLinea.color.g = __ixaReaderGetUI8(rdr);	//color::g
			estiloLinea.color.b = __ixaReaderGetUI8(rdr);	//color::b
			estiloLinea.color.a = 255;
			//PRINTF_INFO("      Estilo linea: colorRGB(%d, %d, %d)\n", estiloLinea.color.r, estiloLinea.color.g, estiloLinea.color.b);
		} else {
			estiloLinea.color.r = __ixaReaderGetUI8(rdr);	//color::r
			estiloLinea.color.g = __ixaReaderGetUI8(rdr);	//color::g
			estiloLinea.color.b = __ixaReaderGetUI8(rdr);	//color::b
			estiloLinea.color.a = __ixaReaderGetUI8(rdr);	//color::a
			//PRINTF_INFO("      Estilo linea: colorRGB(%d, %d, %d, %d)\n", estiloLinea.color.r, estiloLinea.color.g, estiloLinea.color.b, estiloLinea.color.a);
		}
	}
	return estiloLinea;
}

STIXA_StyleFill __ixaLoadFILLSTYLE(STIXA_Reader* rdr, const ENIXA_Shape formType){
	STIXA_StyleFill relleno;
	relleno.tipoRelleno		= (ENIXA_StyleFill)__ixaReaderGetUI8(rdr);
	relleno.color.r	 		= 0;
	relleno.color.g	 		= 0;
	relleno.color.b	 		= 0;
	relleno.color.a	 		= 0;
	//PRINTF_INFO("LEYENDO ESTILO DE RELLENO\n");
	//PRINTF_INFO("      Estilo relleno: tipo (%d) (%s)\n", relleno.tipoRelleno, (relleno.tipoRelleno == ENIXA_StyleFillSolido || relleno.tipoRelleno == ENIXA_StyleFillgradienteLinear || relleno.tipoRelleno == ENIXA_StyleFillgradienteRadial || relleno.tipoRelleno == ENIXA_StyleFillgradienteRadialFocal || relleno.tipoRelleno == ENIXA_StyleFillmapaDeBitsRepetido || relleno.tipoRelleno == ENIXA_StyleFillmapaDeBitsRecortado || relleno.tipoRelleno == ENIXA_StyleFillmapaDeBitsRepetidoNoSuavizado || relleno.tipoRelleno == ENIXA_StyleFillmapaDeBitsRecortadoNoSuavizado?"ok": "NO VALIDO"));
	if(relleno.tipoRelleno == ENIXA_StyleFillSolido){
		if(formType == ENIXA_Shape1 || formType == ENIXA_Shape2){
			relleno.color.r	 = __ixaReaderGetUI8(rdr);
			relleno.color.g	 = __ixaReaderGetUI8(rdr);
			relleno.color.b	 = __ixaReaderGetUI8(rdr);
			relleno.color.a	 = 255;
			//PRINTF_INFO("      Estilo relleno: colorRGB (%d, %d, %d)\n", relleno.color.r, relleno.color.g, relleno.color.b);
		} else {
			relleno.color.r	 = __ixaReaderGetUI8(rdr);
			relleno.color.g	 = __ixaReaderGetUI8(rdr);
			relleno.color.b	 = __ixaReaderGetUI8(rdr);
			relleno.color.a	 = __ixaReaderGetUI8(rdr);
			//PRINTF_INFO("      Estilo relleno: colorRGBA (%d, %d, %d, %d)\n", relleno.color.r, relleno.color.g, relleno.color.b, relleno.color.a);
		}
	}
	if(relleno.tipoRelleno == ENIXA_StyleFillgradienteLinear || relleno.tipoRelleno == ENIXA_StyleFillgradienteRadial || relleno.tipoRelleno == ENIXA_StyleFillgradienteRadialFocal){
		//PRINTF_INFO("      Estilo relleno: incluye gradiente (matriz y gradiente)\n");
		relleno.matrizGradiente = __ixaLoadMATRIX(rdr);
		relleno.gradiente		= __ixaLoadGRADIENT(rdr, formType, (relleno.tipoRelleno == ENIXA_StyleFillgradienteRadialFocal));
	}
	if(relleno.tipoRelleno == ENIXA_StyleFillmapaDeBitsRepetido || relleno.tipoRelleno == ENIXA_StyleFillmapaDeBitsRecortado || relleno.tipoRelleno == ENIXA_StyleFillmapaDeBitsRepetidoNoSuavizado || relleno.tipoRelleno == ENIXA_StyleFillmapaDeBitsRecortadoNoSuavizado){
		relleno.idMapaBits 		= __ixaReaderGetUI16(rdr);
		//PRINTF_INFO("      Estilo relleno: incluye mapa de bits id(%d) y matriz\n", relleno.idMapaBits);
		relleno.matrizMapaBits 	= __ixaLoadMATRIX(rdr);
	}
	return relleno;
}

STIXA_CXForm __ixaLoadCXFORM(STIXA_Reader* rdr, const IxaBOOL withAlpha){
	STIXA_CXForm cform;
	__ixaReaderPositionInByteAligned(rdr);
	cform.tieneAdiciones		= __ixaReaderGetBitsUnsigned(rdr, 1);
	cform.tieneMultiplicaciones	= __ixaReaderGetBitsUnsigned(rdr, 1);
	cform.nBits					= __ixaReaderGetBitsUnsigned(rdr, 4);
	if(cform.tieneMultiplicaciones==1){
		cform.multiplicadorpR	= __ixaReaderGetBitsSigned(rdr, cform.nBits);
		cform.multiplicadorpG	= __ixaReaderGetBitsSigned(rdr, cform.nBits);
		cform.multiplicadorpB	= __ixaReaderGetBitsSigned(rdr, cform.nBits);
		if(withAlpha){
			cform.multiplicadorpA = __ixaReaderGetBitsSigned(rdr, cform.nBits);
		} else {
			cform.multiplicadorpA = 256;
		}
	} else {
		cform.multiplicadorpR	= 256;
		cform.multiplicadorpG	= 256;
		cform.multiplicadorpB	= 256;
		cform.multiplicadorpA 	= 256;
	}
	if(cform.tieneAdiciones==1){
		cform.adicionR			= __ixaReaderGetBitsSigned(rdr, cform.nBits);
		cform.adicionG			= __ixaReaderGetBitsSigned(rdr, cform.nBits);
		cform.adicionB			= __ixaReaderGetBitsSigned(rdr, cform.nBits);
		if(withAlpha){
			cform.adicionA		= __ixaReaderGetBitsSigned(rdr, cform.nBits);
		} else {
			cform.adicionA		= 0;
		}
	} else {
		cform.adicionR			= 0;
		cform.adicionG			= 0;
		cform.adicionB			= 0;
		cform.adicionA			= 0;
	}
	return cform;
}

IxaUI32 __ixaLoadCLIPEVENTFLAGS(STIXA_Reader* rdr, const IxaUI32 versionFlash){
	//In SWF 5 and earlier, CLIPEVENTFLAGS is 2 bytes; in SWF 6 and later, it is 4 bytes.
	if(versionFlash<=5) return (IxaUI32)__ixaReaderGetUI16(rdr);
	return __ixaReaderGetUI32(rdr);
}

STIXA_SoundEnv __ixaLoadSOUNDENVELOPE(STIXA_Reader* rdr){
	STIXA_SoundEnv datosVolumen;
	datosVolumen.pos44		= __ixaReaderGetUI32(rdr);
	datosVolumen.volumenIzq	= __ixaReaderGetUI16(rdr);
	datosVolumen.volumenDer	= __ixaReaderGetUI16(rdr);
	return datosVolumen;
}

STIXA_SoundInfo __ixaLoadSOUNDINFO(STIXA_Reader* rdr){
	STIXA_SoundInfo datosSonido;
	__ixaReaderPositionInByteAligned(rdr);
	/*IxaUI32 reservado				=*/ __ixaReaderGetBitsUnsigned(rdr, 2); //reservado
	datosSonido.syncStop			= (__ixaReaderGetBitsUnsigned(rdr, 1)!=0); //PRINTF_INFO("Stop (%d)\n", datosSonido.syncStop);
	datosSonido.syncNoMultiple		= (__ixaReaderGetBitsUnsigned(rdr, 1)!=0); //PRINTF_INFO("NoMultiple (%d)\n", datosSonido.syncNoMultiple);
	datosSonido.tieneInfoVolumen	= (__ixaReaderGetBitsUnsigned(rdr, 1)!=0); //PRINTF_INFO("Tiene info Volumen (%d)\n", datosSonido.tieneInfoVolumen);
	datosSonido.tieneRepeticiones	= (__ixaReaderGetBitsUnsigned(rdr, 1)!=0); //PRINTF_INFO("Tiene info Repeticiones (%d)\n", datosSonido.tieneRepeticiones);
	datosSonido.tienePuntoSalida	= (__ixaReaderGetBitsUnsigned(rdr, 1)!=0); //PRINTF_INFO("Tiene info PuntoSalida (%d)\n", datosSonido.tienePuntoSalida);
	datosSonido.tienePuntoEntrada	= (__ixaReaderGetBitsUnsigned(rdr, 1)!=0); //PRINTF_INFO("Tiene info PuntoEntrada (%d)\n", datosSonido.tienePuntoEntrada);
	//
	datosSonido.puntoEntrada		= 0;
	datosSonido.puntoSalida			= 0;
	datosSonido.repeticiones		= 0;
	datosSonido.conteoPuntosVolumen	= 0;
	datosSonido.puntosVolumen		= NULL;
	//
	if(datosSonido.tienePuntoEntrada)	datosSonido.puntoEntrada	= __ixaReaderGetUI32(rdr);
	if(datosSonido.tienePuntoSalida)	datosSonido.puntoSalida		= __ixaReaderGetUI32(rdr);
	if(datosSonido.tieneRepeticiones)	datosSonido.repeticiones	= __ixaReaderGetUI16(rdr); //PRINTF_INFO("Repeticiones: %d\n", (IxaSI32)datosSonido.repeticiones);
	if(datosSonido.tieneInfoVolumen){
		datosSonido.conteoPuntosVolumen = __ixaReaderGetUI8(rdr);
		//PRINTF_INFO("Conteo de puntos de volumen: %d\n", (IxaSI32)datosSonido.conteoPuntosVolumen);
		if(datosSonido.conteoPuntosVolumen!=0){
			IxaUI8 iVolumen;
			IXA_MALLOC(datosSonido.puntosVolumen, STIXA_SoundEnv, sizeof(STIXA_SoundEnv) * datosSonido.conteoPuntosVolumen, "STIXA_SoundInfo.puntosVolumen")
			for(iVolumen=0; iVolumen<datosSonido.conteoPuntosVolumen; iVolumen++){
				datosSonido.puntosVolumen[iVolumen] = __ixaLoadSOUNDENVELOPE(rdr);
				//PRINTF_INFO("PuntoVolumen[%d], pos(%d) volIzq(%d) volDer(%d)\n", iVolumen, datosSonido.puntosVolumen[iVolumen].pos44, datosSonido.puntosVolumen[iVolumen].volumenIzq, datosSonido.puntosVolumen[iVolumen].volumenDer);
			}
		}
	}
	return datosSonido;
}

void __ixaLoadSHAPE(STIXA_SwfFile* swf, STIXA_Reader* rdr, STIXA_Shape* dstShape, const ENIXA_Shape formType, const IxaBOOL withStyle){
	//Styles
	STIXA_StyleLine* lineStylesArr	= NULL;
	IxaUI16 lineStylesArrUse		= 0;
	IxaUI16 lineStylesArrSize		= 0;
	STIXA_StyleFill* fillStylesArr	= NULL;
	IxaUI16 fillStylesArrUse		= 0;
	IxaUI16 fillStylesArrSize		= 0;
	//
	IxaSI32 anchoLineaActivo 		= 0;
	STIXA_RGBA colorVacio, colorLineaActivo, colorRellenoIzqActivo, colorRellenoDerActivo;
	IxaBOOL estiloLineaExplicito, estiloRellenoIzqExplicito, esitloRellenoDerExplicito;
	IxaUI16 mapaBitsRellenoIzq		= 0; //charID
	IxaUI16 mapaBitsRellenoDer		= 0; //charID
	//
	IxaUI32 bitsEnRellenos; IxaUI32 bitsEnLineas;
	STIXA_Figure newFigure; IxaBOOL newFigureIsActive = IXA_FALSE;
	IxaSI32 posXTmp = 0, posYTmp = 0;
	IxaBOOL contCicle = IXA_TRUE;
	//
	IXA_ASSERT(dstShape->figsArrUse==0 && dstShape->figsArrSize==0 && dstShape->figsArr==NULL)
	//Init colors
	colorVacio.r = colorVacio.g = colorVacio.b = colorVacio.a = 0;
	colorLineaActivo = colorVacio;
	colorRellenoIzqActivo = colorVacio;
	colorRellenoDerActivo = colorVacio;
	estiloLineaExplicito = estiloRellenoIzqExplicito = esitloRellenoDerExplicito = IXA_FALSE;
	//Read initial styles
	if(withStyle){
		IxaUI32 i; IxaUI32 conteoEstilosRelleno; IxaUI32 conteoEstilosLineas;
		//FILLSTYLEARRAY
		conteoEstilosRelleno = __ixaReaderGetUI8(rdr); //PRINTF_INFO("   Conteo estilos rellenos CORTO (%d)\n", conteoEstilosRelleno);
		if(conteoEstilosRelleno==0xFF){
			conteoEstilosRelleno = __ixaReaderGetUI16(rdr); //PRINTF_INFO("   Conteo estilos rellenos LARGO (%d)\n", conteoEstilosRelleno);
		}
		//PRINTF_INFO("   Conteo de estilos de relleno iniciales: %d\n", conteoEstilosRelleno);
		for(i=0; i<conteoEstilosRelleno; i++){
			IXA_ASSERT(fillStylesArrUse <= fillStylesArrSize) //Assert if memory corrupted
			if(fillStylesArrUse == fillStylesArrSize){
				ixaArrayResize((void**)&fillStylesArr, &fillStylesArrUse, &fillStylesArrSize, sizeof(STIXA_StyleFill), 8);
			}
			fillStylesArr[fillStylesArrUse++] = __ixaLoadFILLSTYLE(rdr, formType);
		}
		//LINESTYLEARRAY
		conteoEstilosLineas = __ixaReaderGetUI8(rdr); //PRINTF_INFO("   Conteo estilos lineas CORTO (%d)\n", conteoEstilosLineas);
		if(conteoEstilosLineas==0xFF){
			conteoEstilosLineas = __ixaReaderGetUI16(rdr); //PRINTF_INFO("   Conteo estilos lineas LARGO (%d)\n", conteoEstilosLineas);
		}
		//PRINTF_INFO("   Conteo de estilos de lineas iniciales: %d\n", conteoEstilosLineas);
		for(i=0; i<conteoEstilosLineas; i++){
			IXA_ASSERT(lineStylesArrUse <= lineStylesArrSize) //Assert if memory corrupted
			if(lineStylesArrUse == lineStylesArrSize){
				ixaArrayResize((void**)&lineStylesArr, &lineStylesArrUse, &lineStylesArrSize, sizeof(STIXA_StyleLine), 8);
			}
			lineStylesArr[lineStylesArrUse++] = __ixaLoadLINESTYLE(rdr, formType, (formType==ENIXA_Shape4));
		}
	} else {
		//PRINTF_INFO("   Figura no tiene estilos iniciales\n");
	}
	bitsEnRellenos	= __ixaReaderGetBitsUnsigned(rdr, 4);	//PRINTF_INFO("   Bits por estilo relleno (%d)\n", bitsEnRellenos);
	bitsEnLineas 	= __ixaReaderGetBitsUnsigned(rdr, 4);	//PRINTF_INFO("   Bits por estilo lineas (%d)\n", bitsEnLineas);
	//SHAPE RECORDS
	__ixaReaderPositionInByteAligned(rdr);
	//PRINTF_INFO("   ShapeRecords...\n");
	do {
		//__ixaReaderPositionInByteAligned(rdr); //aunque la DOC oficial diga que debene star byte-alineados, en la practica no es asi.
		IxaUI32 esEdge = __ixaReaderGetBitsUnsigned(rdr, 1);
		if(esEdge == 0){
			IxaUI32 StateNewStyles		= __ixaReaderGetBitsUnsigned(rdr, 1);
			IxaUI32 StateLineStyle		= __ixaReaderGetBitsUnsigned(rdr, 1);
			IxaUI32 StateFillStyle1	= __ixaReaderGetBitsUnsigned(rdr, 1);
			IxaUI32 StateFillStyle0	= __ixaReaderGetBitsUnsigned(rdr, 1);
			IxaUI32 StateMoveTo		= __ixaReaderGetBitsUnsigned(rdr, 1);
			if(StateNewStyles==0 && StateLineStyle==0 && StateFillStyle1==0 && StateFillStyle0==0 && StateMoveTo==0){
				//ENDSHAPERECORD
				//PRINTF_INFO("   ENDSHAPERECORD\n");
				contCicle = IXA_FALSE;
				//
				if(!newFigureIsActive){
					//This can happend when the figure is empty
					ixaFigureInit(&newFigure);
					newFigureIsActive = IXA_TRUE;
				}
				ixaShapeFigureAdd(dstShape, &newFigure); //no need to call ixaFigureFinalize(newFigure) when is added to a form.
				newFigureIsActive = IXA_FALSE;
				//posXTmp = 0;
				//posYTmp = 0;
				//PRINTF_INFO("Despues de ENDSHAPERECORD quedaron %d bits\n", lector->bitsSinLeerDeByteActual());
				__ixaReaderPositionInByteAligned(rdr);
			} else {
				//STYLECHANGERECORD
				//PRINTF_INFO("   STYLECHANGERECORD\n");
				IxaUI32 FillStyle0 = 0; IxaUI32 FillStyle1 = 0; IxaUI32 LineStyle = 0;
				//PRIMERO CARGAR LOS DATOS DEL REGISTRO
				if(StateMoveTo!=0){
					IxaUI32 MoveBits 	= __ixaReaderGetBitsUnsigned(rdr, 5);
					IxaSI32 MoveDeltaX	= __ixaReaderGetBitsSigned(rdr, MoveBits);
					IxaSI32 MoveDeltaY = __ixaReaderGetBitsSigned(rdr, MoveBits);
					posXTmp = MoveDeltaX; //pendiente: es sumar a la posicion actual o establecer como posicion actual?
					posYTmp = MoveDeltaY; //pendiente: es sumar a la posicion actual o establecer como posicion actual?
					//PRINTF_INFO("   Establecer movimiento: x(%d) y(%d), %d bits leidos\n", MoveDeltaX, MoveDeltaY, MoveBits);
					//primera figura o cambio de figura
					if(newFigureIsActive){
						ixaShapeFigureAdd(dstShape, &newFigure); //no need to call ixaFigureFinalize(newFigure) when is added to a form.
						newFigureIsActive = IXA_FALSE;
					}
				}
				if(StateFillStyle0!=0){
					FillStyle0 = __ixaReaderGetBitsUnsigned(rdr, bitsEnRellenos);
				}
				if(StateFillStyle1!=0){
					FillStyle1 = __ixaReaderGetBitsUnsigned(rdr, bitsEnRellenos);
				}
				if(StateLineStyle!=0){
					LineStyle = __ixaReaderGetBitsUnsigned(rdr, bitsEnLineas);
				}
				if(StateNewStyles!=0){
					IxaUI32 conteoEstilosRelleno; IxaUI32 i; IxaUI32 conteoEstilosLineas;
					IxaUI32 NumFillBits; IxaUI32 NumLineBits;
					lineStylesArrUse = 0;
					fillStylesArrUse = 0;
					//Nuevo FILLSTYLEARRAY
					conteoEstilosRelleno = __ixaReaderGetUI8(rdr); //PRINTF_INFO("   Conteo nuevos estilos rellenos CORTO (%d)\n", conteoEstilosRelleno);
					if(conteoEstilosRelleno==0xFF){
						conteoEstilosRelleno = __ixaReaderGetUI16(rdr); //PRINTF_INFO("   Conteo nuevos estilos rellenos LARGO (%d)\n", conteoEstilosRelleno);
					}
					//PRINTF_INFO("   Conteo de estilos de relleno cambiando: %d\n", conteoEstilosRelleno);
					for(i=0; i<conteoEstilosRelleno; i++){
						IXA_ASSERT(fillStylesArrUse <= fillStylesArrSize) //Assert if memory corrupted
						if(fillStylesArrUse == fillStylesArrSize){
							ixaArrayResize((void**)&fillStylesArr, &fillStylesArrUse, &fillStylesArrSize, sizeof(STIXA_StyleFill), 8);
						}
						fillStylesArr[fillStylesArrUse++] = __ixaLoadFILLSTYLE(rdr, formType);
					}
					//Nuevo LINESTYLEARRAY
					conteoEstilosLineas = __ixaReaderGetUI8(rdr); //PRINTF_INFO("   Conteo nuevos estilos lineas CORTO (%d)\n", conteoEstilosLineas);
					if(conteoEstilosLineas==0xFF){
						conteoEstilosLineas = __ixaReaderGetUI16(rdr); //PRINTF_INFO("   Conteo nuevos estilos lineas LARGO (%d)\n", conteoEstilosLineas);
					}
					//PRINTF_INFO("   Conteo de estilos de linea cambiando: %d\n", conteoEstilosLineas);
					for(i=0; i<conteoEstilosLineas; i++){
						IXA_ASSERT(lineStylesArrUse <= lineStylesArrSize) //Assert if memory corrupted
						if(lineStylesArrUse == lineStylesArrSize){
							ixaArrayResize((void**)&lineStylesArr, &lineStylesArrUse, &lineStylesArrSize, sizeof(STIXA_StyleLine), 8);
						}
						lineStylesArr[lineStylesArrUse++] = __ixaLoadLINESTYLE(rdr, formType, (formType==ENIXA_Shape4));
					}
					//
					NumFillBits = __ixaReaderGetBitsUnsigned(rdr, 4); //PRINTF_INFO("   Nuevos bits por estilo relleno (%d)\n", NumFillBits);
					NumLineBits = __ixaReaderGetBitsUnsigned(rdr, 4); //PRINTF_INFO("   Nuevos bits por estilo linea (%d)\n", NumLineBits);
					if(bitsEnRellenos != NumFillBits) bitsEnRellenos = NumFillBits;
					if(bitsEnLineas != NumLineBits) bitsEnLineas = NumLineBits;
				}
				//UNA VEZ INTERPRETADOS LOS DATOS APLICAR A LA INVERSA
				if(StateFillStyle0!=0){
					if(FillStyle0==0){
						estiloRellenoIzqExplicito	= IXA_FALSE;
						colorRellenoIzqActivo 		= colorVacio;
						mapaBitsRellenoIzq			= 0;
					} else if(FillStyle0 > fillStylesArrUse){ //PENDIENTE resolver como evitar que "FillStyle0 > fillStylesArrUse"
						estiloRellenoIzqExplicito	= IXA_FALSE;
						colorRellenoIzqActivo 		= colorVacio;
						mapaBitsRellenoIzq			= 0;
						//PRINTF_INFO("   ADVERTENCIA swfSHAPE, se intento establecer el estilo de relleno0 #%d de %d (estableciendo vacio)\n", FillStyle0, fillStylesArrUse);
					} else {
						//PRINTF_INFO("   Cambiando estilo de lleneno-0 #%d de %d\n", FillStyle0, estilosRelleno->conteo);
						STIXA_StyleFill estiloRelleno = fillStylesArr[FillStyle0 - 1];
						IXA_ASSERT(FillStyle0>0 && FillStyle0<=fillStylesArrUse)
						if(estiloRelleno.tipoRelleno == ENIXA_StyleFillSolido){
							estiloRellenoIzqExplicito	= IXA_TRUE;
							mapaBitsRellenoIzq			= 0;
							colorRellenoIzqActivo.r 	= estiloRelleno.color.r;
							colorRellenoIzqActivo.g 	= estiloRelleno.color.g;
							colorRellenoIzqActivo.b 	= estiloRelleno.color.b;
							colorRellenoIzqActivo.a 	= estiloRelleno.color.a;
							//PRINTF_INFO("SWF RellenoIzq activado: (%d, %d, %d, %d)\n", colorRellenoIzqActivo.r, colorRellenoIzqActivo.g, colorRellenoIzqActivo.b, colorRellenoIzqActivo.a);
						} else if(estiloRelleno.tipoRelleno == ENIXA_StyleFillmapaDeBitsRecortado || estiloRelleno.tipoRelleno == ENIXA_StyleFillmapaDeBitsRecortadoNoSuavizado || estiloRelleno.tipoRelleno == ENIXA_StyleFillmapaDeBitsRepetido || estiloRelleno.tipoRelleno == ENIXA_StyleFillmapaDeBitsRepetidoNoSuavizado){
							//Nota, cuando un mapa de bits es encapsulado en un grafico SWF se pinta con relleno "ENIXA_StyleFillmapaDeBitsRecortadoNoSuavizado"
							estiloRellenoIzqExplicito	= IXA_TRUE;
							colorRellenoIzqActivo 		= colorVacio;
							mapaBitsRellenoIzq 			= estiloRelleno.idMapaBits;
						}
					}
					//PRINTF_INFO("   Establecer FillStyle0(%d), %d bits leidos\n", FillStyle0, bitsEnRellenos);
				}
				if(StateFillStyle1!=0){
					if(FillStyle1==0){ //Pendiente resolver como evitar que "FillStyle1>estilosRelleno->conteo"
						esitloRellenoDerExplicito	= IXA_FALSE;
						colorRellenoDerActivo 		= colorVacio;
						mapaBitsRellenoDer			= 0;
					} else if(FillStyle1 > fillStylesArrUse){ //PENDIENTE resolver como evitar que "FillStyle1 > fillStylesArrUse"
						esitloRellenoDerExplicito	= IXA_FALSE;
						colorRellenoDerActivo 		= colorVacio;
						mapaBitsRellenoDer			= 0;
						//PRINTF_INFO("   ADVERTENCIA swfSHAPE, se intento establecer el estilo de relleno1 #%d de %d (estableciendo vacio)\n", FillStyle1, fillStylesArrUse);
					} else {
						//PRINTF_INFO("   Cambiando estilo de lleneno-1 #%d de %d\n", FillStyle1, estilosRelleno->conteo);
						STIXA_StyleFill estiloRelleno = fillStylesArr[FillStyle1 - 1];
						IXA_ASSERT(FillStyle1>0 && FillStyle1<=fillStylesArrUse)
						if(estiloRelleno.tipoRelleno == ENIXA_StyleFillSolido){
							esitloRellenoDerExplicito	= IXA_TRUE;
							mapaBitsRellenoDer			= 0;
							colorRellenoDerActivo.r 	= estiloRelleno.color.r;
							colorRellenoDerActivo.g 	= estiloRelleno.color.g;
							colorRellenoDerActivo.b 	= estiloRelleno.color.b;
							colorRellenoDerActivo.a 	= estiloRelleno.color.a;
							//PRINTF_INFO("SWF RellenoDer activado: (%d, %d, %d, %d)\n", colorRellenoDerActivo.r, colorRellenoDerActivo.g, colorRellenoDerActivo.b, colorRellenoDerActivo.a);
						} else if(estiloRelleno.tipoRelleno == ENIXA_StyleFillmapaDeBitsRecortado || estiloRelleno.tipoRelleno == ENIXA_StyleFillmapaDeBitsRecortadoNoSuavizado || estiloRelleno.tipoRelleno == ENIXA_StyleFillmapaDeBitsRepetido || estiloRelleno.tipoRelleno == ENIXA_StyleFillmapaDeBitsRepetidoNoSuavizado){
							//Nota, cuando un mapa de bits es encapsulado en un grafico SWF se pinta con relleno "ENIXA_StyleFillmapaDeBitsRecortadoNoSuavizado"
							esitloRellenoDerExplicito	= IXA_TRUE;
							colorRellenoDerActivo 		= colorVacio;
							mapaBitsRellenoDer 			= estiloRelleno.idMapaBits;
						}
					}
					//PRINTF_INFO("   Establecer FillStyle1(%d), %d bits leidos\n", FillStyle1, bitsEnRellenos);
				}
				if(StateLineStyle!=0){
					if(LineStyle==0){ //Pendiente resolver como evitar que "LineStyle>estilosLinea->conteo"
						estiloLineaExplicito	= IXA_FALSE;
						anchoLineaActivo 		= 0;
						colorLineaActivo 		= colorVacio;
					} else if(LineStyle > lineStylesArrUse){ //PENDIENTE resolver como evitar que "LineStyle > lineStylesArrUse"
						estiloLineaExplicito	= IXA_TRUE;
						anchoLineaActivo 		= 0;
						colorLineaActivo		= colorVacio;
						//PRINTF_INFO("   ADVERTENCIA swfSHAPE, se intento establecer le estilo de linea #%d de %d (estableciendo vacio)\n", LineStyle, lineStylesArrUse);
					} else {
						STIXA_StyleLine estiloLinea = lineStylesArr[LineStyle - 1];
						IXA_ASSERT(LineStyle>0 && LineStyle<=lineStylesArrUse)
						estiloLineaExplicito	= IXA_TRUE;
						anchoLineaActivo 		= estiloLinea.anchoLinea;
						colorLineaActivo.r 		= estiloLinea.color.r;
						colorLineaActivo.g 		= estiloLinea.color.g;
						colorLineaActivo.b 		= estiloLinea.color.b;
						colorLineaActivo.a 		= estiloLinea.color.a;
						//PRINTF_INFO("   SWF ColorLinea activado: (%d, %d, %d, %d)\n", colorLineaActivo.r, colorLineaActivo.g, colorLineaActivo.b, colorLineaActivo.a);
					}
					//PRINTF_INFO("   Establecer LineStyle(%d), %d bits leidos\n", LineStyle, bitsEnLineas);
				}
				if(StateMoveTo!=0){
					STIXA_FigVertex newVertex;
					newVertex.isCurve			= IXA_FALSE;
					newVertex.lineStyleExplicit	= estiloLineaExplicito;
					newVertex.fillLeftExplicit	= estiloRellenoIzqExplicito;
					newVertex.fillRightExplicit	= estiloRellenoIzqExplicito;
					newVertex.x					= posXTmp;
					newVertex.y					= posYTmp;
					newVertex.lineWidth			= anchoLineaActivo;
					newVertex.lineColor			= colorLineaActivo;
					newVertex.fillLeftColor		= colorRellenoIzqActivo;
					newVertex.fillRightColor	= colorRellenoDerActivo;
					newVertex.fillLeftBitmap	= mapaBitsRellenoIzq;	//charID
					newVertex.fillRightBitmap	= mapaBitsRellenoDer;	//charID
					IXA_ASSERT(!newFigureIsActive)
					ixaFigureInit(&newFigure);
					newFigureIsActive = IXA_TRUE;
					ixaFigureVertexAdd(&newFigure, &newVertex);
				}
			}
		} else {
			IxaUI32 esRecto = __ixaReaderGetBitsUnsigned(rdr, 1);
			if(esRecto==1){
				//PRINTF_INFO("   STRAIGHTEDGERECORD\n");
				IxaUI32 NumBitsMenosDos= __ixaReaderGetBitsUnsigned(rdr, 4);
				IxaUI32 esLineaGeneral = __ixaReaderGetBitsUnsigned(rdr, 1);
				IxaUI32 esVertical		= 0;
				IxaSI32 DeltaX			= 0;
				IxaSI32 DeltaY			= 0;
				IxaSI32 nuevaPosX, nuevaPosY;;
				if(esLineaGeneral==0){
					esVertical = __ixaReaderGetBitsSigned(rdr, 1);
				}
				if(esLineaGeneral==1 || (esLineaGeneral==0 && esVertical==0)){
					DeltaX = __ixaReaderGetBitsSigned(rdr, NumBitsMenosDos + 2);
				}
				if(esLineaGeneral==1 || (esLineaGeneral==0 && esVertical==1)){
					DeltaY = __ixaReaderGetBitsSigned(rdr, NumBitsMenosDos + 2);
				}
				//PRINTF_INFO("   Linea recta en delta +x(%d) +y(%d)\n", DeltaX, DeltaY);
				//
				if(!newFigureIsActive){
					//esto nunca deberia ocurrir, pero es mejor estar seguros
					ixaFigureInit(&newFigure);
					newFigureIsActive = IXA_TRUE;
				}
				if(newFigure.vertArrUse==0){
					STIXA_FigVertex newVertex;
					newVertex.isCurve			= IXA_FALSE;
					newVertex.lineStyleExplicit	= estiloLineaExplicito;
					newVertex.fillLeftExplicit	= estiloRellenoIzqExplicito;
					newVertex.fillRightExplicit	= estiloRellenoIzqExplicito;
					newVertex.x					= posXTmp;
					newVertex.y					= posYTmp;
					newVertex.lineWidth			= anchoLineaActivo;
					newVertex.lineColor			= colorLineaActivo;
					newVertex.fillLeftColor		= colorRellenoIzqActivo;
					newVertex.fillRightColor	= colorRellenoDerActivo;
					newVertex.fillLeftBitmap	= mapaBitsRellenoIzq;	//charID
					newVertex.fillRightBitmap	= mapaBitsRellenoDer;	//charID
					ixaFigureVertexAdd(&newFigure, &newVertex);
				}
				nuevaPosX 	= posXTmp + DeltaX;
				nuevaPosY	= posYTmp + DeltaY;
				{
					STIXA_FigVertex newVertex;
					newVertex.isCurve			= IXA_FALSE;
					newVertex.lineStyleExplicit	= estiloLineaExplicito;
					newVertex.fillLeftExplicit	= estiloRellenoIzqExplicito;
					newVertex.fillRightExplicit	= estiloRellenoIzqExplicito;
					newVertex.x					= nuevaPosX;
					newVertex.y					= nuevaPosY;
					newVertex.lineWidth			= anchoLineaActivo;
					newVertex.lineColor			= colorLineaActivo;
					newVertex.fillLeftColor		= colorRellenoIzqActivo;
					newVertex.fillRightColor	= colorRellenoDerActivo;
					newVertex.fillLeftBitmap	= mapaBitsRellenoIzq;	//charID
					newVertex.fillRightBitmap	= mapaBitsRellenoDer;	//charID
					ixaFigureVertexAdd(&newFigure, &newVertex);
				}
				posXTmp = nuevaPosX;
				posYTmp = nuevaPosY;
			} else {
				//PRINTF_INFO("   CURVEDEDGERECORD\n");
				IxaUI32 NumBitsMenosDos= __ixaReaderGetBitsUnsigned(rdr, 4);
				IxaSI32 ControlDeltaX	= __ixaReaderGetBitsSigned(rdr, NumBitsMenosDos+2);
				IxaSI32 ControlDeltaY	= __ixaReaderGetBitsSigned(rdr, NumBitsMenosDos+2);
				IxaSI32 AnchorDeltaX	= __ixaReaderGetBitsSigned(rdr, NumBitsMenosDos+2);
				IxaSI32 AnchorDeltaY	= __ixaReaderGetBitsSigned(rdr, NumBitsMenosDos+2);
				IxaSI32 curvaRefX, curvaRefY, nuevaPosX, nuevaPosY;
				//PRINTF_INFO("   Linea curva en delta +x(%d) +y(%d) con punto de control +x(%d) +y(%d)\n", AnchorDeltaX, AnchorDeltaY, ControlDeltaX, ControlDeltaY);
				//
				if(!newFigureIsActive){
					//esto nunca deberia ocurrir, pero es mejor estar seguros
					ixaFigureInit(&newFigure);
					newFigureIsActive = IXA_TRUE;
				}
				if(newFigure.vertArrUse==0){
					STIXA_FigVertex newVertex;
					newVertex.isCurve			= IXA_FALSE;
					newVertex.lineStyleExplicit	= estiloLineaExplicito;
					newVertex.fillLeftExplicit	= estiloRellenoIzqExplicito;
					newVertex.fillRightExplicit	= estiloRellenoIzqExplicito;
					newVertex.x					= posXTmp;
					newVertex.y					= posYTmp;
					newVertex.lineWidth			= anchoLineaActivo;
					newVertex.lineColor			= colorLineaActivo;
					newVertex.fillLeftColor		= colorRellenoIzqActivo;
					newVertex.fillRightColor	= colorRellenoDerActivo;
					newVertex.fillLeftBitmap	= mapaBitsRellenoIzq;	//charID
					newVertex.fillRightBitmap	= mapaBitsRellenoDer;	//charID
					ixaFigureVertexAdd(&newFigure, &newVertex);
				}
				curvaRefX 	= posXTmp + ControlDeltaX;
				curvaRefY	= posYTmp + ControlDeltaY;
				nuevaPosX 	= posXTmp + ControlDeltaX + AnchorDeltaX;
				nuevaPosY	= posYTmp + ControlDeltaY + AnchorDeltaY;
				{
					STIXA_FigVertex newVertex;
					newVertex.isCurve			= IXA_TRUE;
					newVertex.lineStyleExplicit	= estiloLineaExplicito;
					newVertex.fillLeftExplicit	= estiloRellenoIzqExplicito;
					newVertex.fillRightExplicit	= estiloRellenoIzqExplicito;
					newVertex.x					= curvaRefX;
					newVertex.y					= curvaRefY;
					newVertex.lineWidth			= anchoLineaActivo;
					newVertex.lineColor			= colorLineaActivo;
					newVertex.fillLeftColor		= colorRellenoIzqActivo;
					newVertex.fillRightColor	= colorRellenoDerActivo;
					newVertex.fillLeftBitmap	= mapaBitsRellenoIzq;	//charID
					newVertex.fillRightBitmap	= mapaBitsRellenoDer;	//charID
					ixaFigureVertexAdd(&newFigure, &newVertex);
					newVertex.isCurve			= IXA_FALSE;
					newVertex.lineStyleExplicit	= estiloLineaExplicito;
					newVertex.fillLeftExplicit	= estiloRellenoIzqExplicito;
					newVertex.fillRightExplicit	= estiloRellenoIzqExplicito;
					newVertex.x					= nuevaPosX;
					newVertex.y					= nuevaPosY;
					newVertex.lineWidth			= anchoLineaActivo;
					newVertex.lineColor			= colorLineaActivo;
					newVertex.fillLeftColor		= colorRellenoIzqActivo;
					newVertex.fillRightColor	= colorRellenoDerActivo;
					newVertex.fillLeftBitmap	= mapaBitsRellenoIzq;	//charID
					newVertex.fillRightBitmap	= mapaBitsRellenoDer;	//charID
					ixaFigureVertexAdd(&newFigure, &newVertex);
				}
				posXTmp = nuevaPosX;
				posYTmp = nuevaPosY;
			}
		}
		//__ixaReaderPositionInByteAligned(rdr); //aunque la DOC oficial diga que debene star byte-alineados, en la practica no es asi. ---porque escribi esto???
	} while(contCicle);
	//This should never happend (a not-null figure after the while, but better avoid memory leaking)
	if(newFigureIsActive){
		ixaShapeFigureAdd(dstShape, &newFigure); //no need to call ixaFigureFinalize(newFigure) when is added to a form.
		newFigureIsActive = IXA_FALSE;
	}
	if(lineStylesArr!=NULL){ IXA_FREE(lineStylesArr); }
	if(fillStylesArr!=NULL){ IXA_FREE(fillStylesArr); }
}


