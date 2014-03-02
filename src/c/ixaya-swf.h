//
//  ixaya-swf.h
//  ixaya
//
//  Created by Marcos Ortega on 24/02/14.
//  Copyright (c) 2014 NIBSA. All rights reserved.
//
// This entire notice must be retained in this source code.
// This source code is under LGLP v2.1 Licence.
// This software is provided "as is", with no warranty.
// Latest fixes enhancements and documentation at https://github.com/nicaraguabinary/ixaya-swf
//

#ifndef ixaya_ixaya_swf_h
#define ixaya_ixaya_swf_h

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------
//---------------------------------
//-- Configurable
//---------------------------------
//---------------------------------

//
// You can customize ixaya's memory management
// by defining this MACROS before
// "ixaya-swf.c" get included or compiled.
//
//#define IXA_MALLOC(POINTER_DEST, POINTER_TYPE, SIZE_BYTES, STR_HINT)
//#define IXA_FREE(POINTER)
//#define IXA_FIGURE_VERTEXS_SIZE_GROWTH
//#define IXA_SHAPE_FIGURES_SIZE_GROWTH
//#define IXA_SPRITE_ELEMS_SIZE_GROWTH
//#define IXA_SPRITE_ELEMSPROPS_SIZE_GROWTH
//#define IXA_SPRITE_SOUNDEVENTS_SIZE_GROWTH
//#define IXA_SWF_DICC_SIZE_GROWTH

//---------------------------------
//---------------------------------
//-- Data types
//---------------------------------
//---------------------------------
typedef unsigned char 		IxaBOOL;	//IxaBOOL, Unsigned 8-bit integer value
typedef unsigned char 		IxaBYTE;	//BYTE, Unsigned 8-bit integer value
typedef char 				IxaSI8;		//IxaSI8, Signed 8-bit integer value
typedef	short int 			IxaSI16;	//IxaSI16, Signed 16-bit integer value
typedef	int 				IxaSI32;	//IxaSI32, Signed 32-bit integer value
typedef	long long 			IxaSI64;	//IxaSI64, Signed 64-bit integer value
typedef unsigned char 		IxaUI8;		//IxaUI8, Unsigned 8-bit integer value
typedef	unsigned short int 	IxaUI16;	//IxaUI16, Unsigned 16-bit integer value
typedef	unsigned int 		IxaUI32;	//IxaUI32, Unsigned 32-bit integer value
typedef	unsigned long long	IxaUI64;	//IxaUI64[n], Unsigned 64-bit array—n is the number of array elements
typedef float				IxaFLOAT;	//float

#define IXA_FALSE			0
#define IXA_TRUE			1

//---------------------------------
//---------------------------------
//-- Structures defined in SWF
//-- format specifications.
//---------------------------------
//---------------------------------

//SWF TAGs ID
typedef enum {
	ENIXA_TagID_End = 0,				//soportado (FIN DE FLUJO)
	ENIXA_TagID_ShowFrame=1,
	ENIXA_TagID_DefineShape=2,			//soportado con limitaciones en estilos especiales de rellenos y lineas
	ENIXA_TagID_PlaceObject=4,
	ENIXA_TagID_RemoveObject=5,
	ENIXA_TagID_DefineBits=6,			//soportado (JPEG sin tabla)
	ENIXA_TagID_DefineButton=7,
	ENIXA_TagID_JPEGTables=8,			//soportado (tabla compartida de los JPEG definidos con 'DefineBits')
	ENIXA_TagID_SetBackgroundColor=9,
	ENIXA_TagID_DefineFont=10,			//soportado sin probar
	ENIXA_TagID_DefineText=11,			//soportado
	ENIXA_TagID_DoAction=12,
	ENIXA_TagID_DefineFontInfo=13,
	ENIXA_TagID_DefineSound=14,
	ENIXA_TagID_StartSound=15,
	ENIXA_TagID_DefineButtonSound=17,
	ENIXA_TagID_SoundStreamHead=18,
	ENIXA_TagID_SoundStreamBlock=19,
	ENIXA_TagID_DefineBitsLossless=20,	//soportado (mapa de bits sin transparencia)
	ENIXA_TagID_DefineBitsJPEG2=21,		//soportado (JPEG/PNG/GIF completo)
	ENIXA_TagID_DefineShape2=22,		//soportado con limitaciones en estilos especiales de rellenos y lineas
	ENIXA_TagID_DefineButtonCxform=23,
	ENIXA_TagID_Protect=24,
	ENIXA_TagID_PlaceObject2=26,
	ENIXA_TagID_RemoveObject2=28,
	ENIXA_TagID_DefineShape3=32,		//soportado con limitaciones en estilos especiales de rellenos y lineas
	ENIXA_TagID_DefineText2=33,			//soportado
	ENIXA_TagID_DefineButton2=34,
	ENIXA_TagID_DefineBitsJPEG3=35,		//soportado (PNG/GIF/JPG con mapa de bits grayscale anexo para trasnaparencia)
	ENIXA_TagID_DefineBitsLossless2=36,	//soportado (mapa de bits con transparencia)
	ENIXA_TagID_DefineEditText=37,
	ENIXA_TagID_DefineSprite=39,		//soportado
	ENIXA_TagID_FrameLabel=43,
	ENIXA_TagID_SoundStreamHead2=45,
	ENIXA_TagID_DefineMorphShape=46,
	ENIXA_TagID_DefineFont2=48,			//soportado (fuente del sistema o por glyphs)
	ENIXA_TagID_ExportAssets=56,
	ENIXA_TagID_ImportAssets=57,
	ENIXA_TagID_EnableDebugger=58,
	ENIXA_TagID_DoInitAction=59,
	ENIXA_TagID_DefineVideoStream=60,
	ENIXA_TagID_VideoFrame=61,
	ENIXA_TagID_DefineFontInfo2=62,
	ENIXA_TagID_EnableDebugger2=64,
	ENIXA_TagID_ScriptLimits=65,
	ENIXA_TagID_SetTabIndex=66,
	ENIXA_TagID_FileAttributes=69,		//soportado (atributos del archivo SWF)
	ENIXA_TagID_PlaceObject3=70,
	ENIXA_TagID_ImportAssets2=71,
	ENIXA_TagID_DefineFontAlignZones=73,
	ENIXA_TagID_CSMTextSettings=74,
	ENIXA_TagID_DefineFont3=75,			//soportado (fuente del sistema o por glyphs)
	ENIXA_TagID_SymbolClass=76,
	ENIXA_TagID_Metadata=77,
	ENIXA_TagID_DefineScalingGrid=78,
	ENIXA_TagID_DoABC=82,
	ENIXA_TagID_DefineShape4=83,		//soportado con limitaciones en estilos especiales de rellenos y lineas
	ENIXA_TagID_DefineMorphShape2=84,
	ENIXA_TagID_DefineSceneAndFrameLabelData=86,
	ENIXA_TagID_DefineBinaryData=87,
	ENIXA_TagID_DefineFontName=88,
	ENIXA_TagID_StartSound2=89,
	ENIXA_TagID_DefineBitsJPEG4=90,		//soportado sin probar aun (JPEG con preblocking)
	ENIXA_TagID_DefineFont4=91
} ENIXA_TagID;

typedef enum {
	ENIXA_CharType_Undef = 0,
	ENIXA_CharType_Sprite,
	ENIXA_CharType_FileBits,	//JPGs
	ENIXA_CharType_Bitmap,		//lossless
	ENIXA_CharType_Shape,
	ENIXA_CharType_Font,
	ENIXA_CharType_Sound
} ENIXA_CharType;

//COLORS
typedef struct {
	IxaUI8 r, g, b;
} STIXA_RGB;

typedef struct {
	IxaUI8 r, g, b, a;
} STIXA_RGBA;

typedef struct {
	IxaUI8 a, r, g, b;
} STIXA_ARGB;

typedef struct {
	IxaUI16 rgb;	//bits: 1 reserved, 5 red, 5 green, 5 blue
} STIXA_PIX15;

typedef struct {
	IxaUI8 x, r, g, b; //x is allways zero
} STIXA_PIX24;

typedef enum {
	ENIXA_ColorUnefined,
	ENIXA_ColorAlpha8,		//solo es alpha
	ENIXA_ColorGray8,		//escala de grises de 8 bits
	ENIXA_ColorGrayAlpha8,	//escala de grises con alpha (8 bits cada uno)
	ENIXA_ColorRGB4,		//RGB de 4 bits cada componente
	ENIXA_ColorRGB8,		//RGB de 8 bits cada componente
	ENIXA_ColorRGBA4,		//RGBA de 4 bits cada componente
	ENIXA_ColorRGBA8,		//RGBA de 8 bits cada componente
	ENIXA_ColorARGB4,		//ARGB de 4 bits cada componente
	ENIXA_ColorARGB8,		//ARGB de 8 bits cada componente
	ENIXA_ColorPIX15,		//
	ENIXA_ColorPIX24		//
} ENIXA_Color;

//RECTANGLE
typedef struct {
	IxaUI8  Nbits;					 //UB[5] Bits used for each subsequent field
	IxaSI32 Xmin, Xmax, Ymin, Ymax; //SB[Nbits] in twips
} STIXA_Rect;

typedef struct {
	IxaSI32 width, height;
} STIXA_Size;

//KERNING
typedef struct {
	IxaUI16 codigo1;	//Character code of the left character.
	IxaUI16 codigo2;	//Character code of the right character.
	IxaSI16 ajuste;	//Adjustment relative to left character’s advance value.
} STIXA_Kerning;

//MATRIX
typedef struct {
	IxaUI32 tieneEscala;
	IxaUI32 bitsPorEscala;		//solo si tiene escala
	float	escalaX;			//solo si tiene escala
	float	escalaY;			//solo si tiene escala
	IxaUI32 tieneRotacion;
	IxaUI32 bitsPorRotacion;	//solo si tiene rotacion
	float	skew0;			//solo si tiene rotacion
	float	skew1;			//solo si tiene rotacion
	IxaUI32 bitsPorTranslacion;
	IxaSI32 traslacionX;
	IxaSI32 traslacionY;
} STIXA_Matrix;

//GRADIENT
typedef struct {
	IxaUI32 spreadMode;
	IxaUI32 interpolationMode;
	IxaUI32 numGradients;
	//faltan los records aqui (por ahora los ignoro)
	IxaUI32 focalPoint;
} STIXA_Gradient;

//LINE STYLE
typedef struct {
	IxaUI16		anchoLinea;
	//faltan los estilos en caso de LINESTYLE2 (ignorando aqui: startCapStyle, JointStyle, HasFillFlag, NoHScaleFlag, NoVScaleFlag, PixelHintingFlag, Reserved, NoClose, EnCapStyle, MiterLimitFactor)
	STIXA_RGBA 	color;
	//falta aqui FillType
} STIXA_StyleLine;

//FILL TYPE
typedef enum {
	ENIXA_StyleFillNinguno 							= 0xFF, //not part of SWF specifications
	ENIXA_StyleFillSolido 							= 0x00,
	ENIXA_StyleFillgradienteLinear 					= 0x10,
	ENIXA_StyleFillgradienteRadial 					= 0x12,
	ENIXA_StyleFillgradienteRadialFocal 			= 0x13,
	ENIXA_StyleFillmapaDeBitsRepetido 				= 0x40,	//swf 8 or higher
	ENIXA_StyleFillmapaDeBitsRecortado				= 0x41, //swf 8 or higher
	ENIXA_StyleFillmapaDeBitsRepetidoNoSuavizado 	= 0x42, //swf 8 or higher
	ENIXA_StyleFillmapaDeBitsRecortadoNoSuavizado 	= 0x43  //swf 8 or higher
} ENIXA_StyleFill;

//FILL STYLE
typedef struct {
	ENIXA_StyleFill tipoRelleno;		//IxaUI8
	STIXA_RGBA 		color;				// (solo si tipoRelleno es ENIXA_StyleFillSolido)
	STIXA_Matrix 	matrizGradiente;	// (solo si tipoRelleno es ENIXA_StyleFillgradienteLinear, ENIXA_StyleFillgradienteRadial, ENIXA_StyleFillgradienteRadialFocal)
	STIXA_Gradient 	gradiente;			// (solo si tipoRelleno es ENIXA_StyleFillgradienteLinear, ENIXA_StyleFillgradienteRadial, ENIXA_StyleFillgradienteRadialFocal)
	IxaUI16			idMapaBits;			// (solo si tipoRelleno es ENIXA_StyleFillmapaDeBitsRepetido, ENIXA_StyleFillmapaDeBitsRecortado, ENIXA_StyleFillmapaDeBitsRepetidoNoSuavizado, ENIXA_StyleFillmapaDeBitsRecortadoNoSuavizado)
	STIXA_Matrix	matrizMapaBits;		// (solo si tipoRelleno es ENIXA_StyleFillmapaDeBitsRepetido, ENIXA_StyleFillmapaDeBitsRecortado, ENIXA_StyleFillmapaDeBitsRepetidoNoSuavizado, ENIXA_StyleFillmapaDeBitsRecortadoNoSuavizado)
} STIXA_StyleFill;

//CX FORM
typedef struct {
	IxaUI32 tieneAdiciones;
	IxaUI32 tieneMultiplicaciones;
	IxaUI32	nBits;
	IxaSI32	multiplicadorpR;
	IxaSI32	multiplicadorpG;
	IxaSI32	multiplicadorpB;
	IxaSI32	multiplicadorpA;
	IxaSI32	adicionR;
	IxaSI32	adicionG;
	IxaSI32	adicionB;
	IxaSI32	adicionA;
} STIXA_CXForm;

//CLIP EVENTS
typedef enum {
	ENIXA_ClipEventKeyUp			= 1,
	ENIXA_ClipEventKeyDown			= 2,
	ENIXA_ClipEventMouseUp			= 4,
	ENIXA_ClipEventMouseDown		= 8,
	ENIXA_ClipEventMouseMove		= 16,
	ENIXA_ClipEventUnload			= 32,
	ENIXA_ClipEventEnterFrame		= 64,
	ENIXA_ClipEventLoad				= 128,
	ENIXA_ClipEventDragOver			= 256,
	ENIXA_ClipEventRollOut			= 512,
	ENIXA_ClipEventRollOver			= 1024,
	ENIXA_ClipEventReleaseOutside	= 2048,
	ENIXA_ClipEventRelease			= 4096,
	ENIXA_ClipEventPress			= 8192,
	ENIXA_ClipEventInitialize		= 16384,
	ENIXA_ClipEventData				= 32768,
	ENIXA_Reserved1					= 65536,
	ENIXA_ClipEventConstruct		= 131072,
	ENIXA_ClipEventKeyPress			= 262144,
	ENIXA_ClipEventDragOut			= 524288,
	ENIXA_Reserved2					= 1048576
} ENIXA_ClipEvent;

//SWF FILE HEADER (BASE)
typedef struct {
	IxaUI8 	firma1;		//'F' (sin comprimir) o 'C' (comprimido)
	IxaUI8 	firma2;		//'W'
	IxaUI8 	firma3;		//'S'
	IxaUI8 	version;	//version-entera del swf player compatible
	IxaUI32 	bytesDatos;	//tamano en bytes del archivo descomprimido
} STIXA_HeaderBase;

//SWF FILE HEADER (EXTENDED)
typedef struct {
	STIXA_Rect	tamanoTwips;	//20 twips = 1 pixel
	float		framerate; 		//framerate
	IxaUI16		totalFrames;	//cantida total de frames de la escena principal
} STIXA_HeaderExt;

//DATA BLOCK
typedef struct {
	IxaBYTE*	puntero;
	IxaUI32	tamano;
} STIXA_DataBlock;

//ATTACHED FILES TYPES
typedef enum {
	ENIXA_FileTypeBINARIO,
	ENIXA_FileTypeJPEG_SIN_TABLA,
	ENIXA_FileTypeJPEG,
	ENIXA_FileTypeJPEG_CON_ALPHA_ANEXO,
	ENIXA_FileTypePNG,
	ENIXA_FileTypeGIF
} ENIXA_FileType;

//ATTACHED FILES
typedef struct {
	ENIXA_FileType tipo;
	IxaBYTE*	puntero;
	IxaUI32		tamano;
	IxaBYTE*	punteroAnexo;
	IxaUI32		tamanoAnexo;
} STIXA_FileBits;


//BITMAPS (Lossless)
typedef struct {
	IxaUI8		bitmapformat;
	ENIXA_Color	color;
	IxaUI16		width;
	IxaUI16		height;
	IxaUI32		paletteSize;
	IxaUI8		bytesPerPixel;
	IxaUI8		bytesPerColorPalette;
	IxaUI32		bytesPerLine;
	IxaBYTE*	uncompressData;	//bitmap and palette data (must free)
	IxaBYTE*	bitmapPointer;	//Pointer inside uncompressData (dont free)
	IxaBYTE*	palettePointer;	//Pointer inside uncompressData (dont free)
} STIXA_Bitmap;

//FORM TYPE
typedef enum {
	ENIXA_ShapeNO_DEFINIDA,
	ENIXA_Shape1,
	ENIXA_Shape2,
	ENIXA_Shape3,
	ENIXA_Shape4
} ENIXA_Shape;

//PNG HEADER CHUNCK
typedef struct {
	IxaUI8	tipoChunk[4];		//'I', 'H', 'D', 'R', cuatro letras que indentifican el tipo de encabezado
	IxaUI32 ancho;				//ancho en pixeles
	IxaUI32	alto;				//alto en pixeles
	IxaUI8	bitsProfundidad;	//1, 2, 4, 8 o 16, profundidad por color
	IxaUI8	tipoColor;			//0, 2, 3, 4 o 6, la suma de estos valores: 1 (palette used), 2 (color used), and 4 (alpha channel used)
	IxaUI8	metodoCompresion;	//actualmente solamente 0 (deflate/inflate compression)
	IxaUI8	metodoFiltrado;		//0: None, 1: Sub, 2: Up, 3: Average, 4: Paeth
	IxaUI8	metodoEntrelazado;	//0 (no interlace) or 1 (Adam7 interlace)
} STIXA_PngIHDR;

//JFIF HEADER
typedef struct {
	IxaBYTE SOI[2];          /* 00h  Start of Image Marker     */
	IxaBYTE APP0[2];         /* 02h  Application Use Marker    */
	IxaBYTE Length[2];       /* 04h  Length of APP0 Field      */
	IxaBYTE Identifier[5];   /* 06h  "JFIF" (zero terminated) Id String */
	IxaBYTE Version[2];      /* 07h  JFIF Format Revision      */
	IxaBYTE Units;           /* 09h  Units used for Resolution */
	IxaBYTE Xdensity[2];     /* 0Ah  Horizontal Resolution     */
	IxaBYTE Ydensity[2];     /* 0Ch  Vertical Resolution       */
	IxaBYTE XThumbnail;      /* 0Eh  Horizontal Pixel Count    */
	IxaBYTE YThumbnail;      /* 0Fh  Vertical Pixel Count      */
} STIXA_JFIF_HDR;

//JPEG DATA MARKERS
#define M_SOF0  0xC0		/* Start Of Frame N */
#define M_SOF1  0xC1		/* N indicates which compression process */
#define M_SOF2  0xC2		/* Only SOF0-SOF2 are now in common use */
#define M_SOF3  0xC3
#define M_SOF5  0xC5		/* NB: codes C4 and CC are NOT SOF markers */
#define M_SOF6  0xC6
#define M_SOF7  0xC7
#define M_SOF9  0xC9
#define M_SOF10 0xCA
#define M_SOF11 0xCB
#define M_SOF13 0xCD
#define M_SOF14 0xCE
#define M_SOF15 0xCF
#define M_SOI   0xD8		/* Start Of Image (beginning of datastream) */
#define M_EOI   0xD9		/* End Of Image (end of datastream) */
#define M_SOS   0xDA		/* Start Of Scan (begins compressed data) */
#define M_APP0	0xE0		/* Application-specific marker, type N */
#define M_APP12	0xEC		/* (we don't bother to list all 16 APPn's) */
#define M_COM   0xFE		/* COMment */

//---------------------------------
//---------------------------------
//-- Structures defined in SWF
//-- format specifications.
//---------------------------------
//---------------------------------

//Figure (inside a shape/form)
typedef struct {
	IxaBOOL		isCurve;
	IxaBOOL		lineStyleExplicit;
	IxaBOOL		fillLeftExplicit;
	IxaBOOL		fillRightExplicit;
	IxaSI32		x;
	IxaSI32		y;
	IxaSI32		lineWidth;
	STIXA_RGBA	lineColor;
	STIXA_RGBA	fillLeftColor;
	STIXA_RGBA	fillRightColor;
	IxaUI16		fillLeftBitmap;		//charID
	IxaUI16		fillRightBitmap;	//charID
} STIXA_FigVertex;

typedef struct {
	STIXA_FigVertex* vertArr;
	IxaUI16			vertArrUse;
	IxaUI16			vertArrSize;
	IxaSI32			xMin, xMax, yMin, yMax; //bounds
} STIXA_Figure;

//Shape (form)
typedef struct {
	STIXA_Figure*	figsArr;
	IxaUI16			figsArrUse;
	IxaUI16			figsArrSize;
	IxaSI32			xMin, xMax, yMin, yMax; //bounds
} STIXA_Shape;

//Glyph definition
typedef struct {
	IxaUI16		code;
	IxaSI32		advace;
	STIXA_Shape	shape;
} STIXA_Glyph;

//Font definition
typedef struct {
	IxaBOOL		isBold;
	IxaBOOL		isItalic;
	IxaSI32		emBoxSize;
	IxaSI32		ascendent;
	IxaSI32		descendent;
	IxaSI32		lineHeightGuide;
	char*		fontName;
	STIXA_Glyph* glyphsArr;
	IxaUI16		glyphsArrUse;
	IxaUI16		glyphsArrSize;
} STIXA_Font;

//SOUND ENVELOPE
typedef struct {
	IxaUI32	pos44;
	IxaUI16	volumenIzq;
	IxaUI16	volumenDer;
} STIXA_SoundEnv;

//SOUND INFO
typedef struct {
	IxaBOOL	syncStop;
	IxaBOOL	syncNoMultiple;
	IxaBOOL	tieneInfoVolumen;
	IxaBOOL	tieneRepeticiones;
	IxaBOOL	tienePuntoSalida;
	IxaBOOL	tienePuntoEntrada;
	//
	IxaUI32	puntoEntrada;
	IxaUI32	puntoSalida;
	IxaUI16	repeticiones;
	IxaUI8		conteoPuntosVolumen;
	STIXA_SoundEnv* puntosVolumen;
} STIXA_SoundInfo;

//SOUND EVENT
typedef struct {
	IxaUI32			iFrame;
	IxaUI16			soundID;
	STIXA_SoundInfo	datosEvento;
} STIXA_SoundEvent;

//SOUND
typedef struct {
	IxaUI8	soundFormat;	//3 = uncompressed, little-endian
	IxaUI8	samplerateType;	//0=5.5KHz, 1=11KHz, 2=22KHz, 3=44KHz
	IxaUI8	sampleBitsType;	//0=8 bits, 1=16 bits
	IxaUI8	channelsType;	//0=Mono, 1=Stereo
	IxaUI32	samplesCount;	//
	IxaUI8*	soundData;
	IxaUI32	soundDataSize;
} STIXA_Sound;

//Sprite (flash movie clip)
typedef struct {
	IxaUI16	idElemento;
	IxaBOOL enDisplayList;
	IxaUI16	charID;
	IxaUI16	profundidad;
	char*	nombre;
} STIXA_SpriteElem;

typedef struct {
	IxaBOOL			visible;
	IxaUI16			frame;
	IxaUI16			idElemento;
	STIXA_Matrix 	matriz;
	STIXA_CXForm 	color;
} STIXA_SpriteElemProps;

typedef struct {
	IxaUI16				framesCount;
	//
	STIXA_SpriteElem*	elementsArr;		//Elements inside this sprite
	IxaUI16				elementsArrUse;
	IxaUI16				elementsArrSize;
	//
	STIXA_SpriteElemProps*	elementsPropsArr;	//Propierties of each element by frame
	IxaUI16				elementsPropsArrUse;
	IxaUI16				elementsPropsArrSize;
	//
	STIXA_SoundEvent*	soundEventsArr;		//Sund events on this sprite
	IxaUI16				soundEventsArrUse;
	IxaUI16				soundEventsArrSize;
} STIXA_Sprite;

//SwfFile's dictionary element
typedef struct {
	IxaUI16	charType;	//ENIXA_CharType
	IxaUI16	charID;
	void*	charPointer;
} STIXA_DiccElem;

typedef IxaBOOL (*PTRIXA_zlibUncompress)(IxaUI8* destPtr, const IxaUI32 ptrDestLen, const IxaUI8* ptrSrc, const IxaUI32 nSrcLen);

//Swf File
typedef struct {
	STIXA_HeaderBase	_encabezado;
	STIXA_HeaderExt		_encabezado2;
	IxaBYTE*			_swfDescomprimido;
	//
	IxaUI32				_posPrimerTag;
	IxaUI32				_conteoTags;
	STIXA_Sprite		_mainSprite;
	//
	STIXA_DataBlock		_JPEGSharedTable;
	//Dictionary
	STIXA_DiccElem*		_diccArr;
	IxaUI16				_diccArrUse;
	IxaUI16				_diccArrSize;
} STIXA_SwfFile;

//Utilities
void			ixaArrayResize(void** pArr, IxaUI16* pArrUse, IxaUI16* pArrSize, const size_t pItemSize, const IxaUI16 pArrGrowth);
IxaUI32			ixaStringLenght(const char* str);
ENIXA_FileType	ixaBitmapFileType(const IxaBYTE* bitmapData, IxaUI32* bytesToIgnore);
STIXA_Size		ixaBitmapSize(const IxaBYTE* bitmapData, const IxaUI32 sizeBytes, const ENIXA_FileType fileType);
STIXA_Size		ixaBitmapSizeJPEG(const IxaBYTE* bitmapData, const IxaUI32 sizeBytes);
STIXA_Size		ixaBitmapSizePNG(const IxaBYTE* bitmapData, const IxaUI32 sizeBytes);
STIXA_Size		ixaBitmapSizeGIF(const IxaBYTE* bitmapData, const IxaUI32 sizeBytes);
//File Binary
void			ixaFileBinaryInit(STIXA_FileBits* filebin);
void			ixaFileBinaryFinalize(STIXA_FileBits* filebin);
//Bitmap (lossless)
void			ixaBitmapInit(STIXA_Bitmap* bitmap);
void			ixaBitmapFinalize(STIXA_Bitmap* bitmap);
//Figure (part of a shape/form)
void			ixaFigureInit(STIXA_Figure* fig);
void			ixaFigureFinalize(STIXA_Figure* fig);
void			ixaFigureVertexAdd(STIXA_Figure* fig, STIXA_FigVertex* vert);
//Shape (graphics)
void			ixaShapeInit(STIXA_Shape* shape);
void			ixaShapeFinalize(STIXA_Shape* shape);
void			ixaShapeFigureAdd(STIXA_Shape* shape, STIXA_Figure* fig); //Dont finalize the given figure after adding it to a shape
//Glyph (definition)
void			ixaGlyphInit(STIXA_Glyph* glyph);
void			ixaGlyphFinalize(STIXA_Glyph* glyph);
//Font (definition)
void			ixaFontInit(STIXA_Font* font);
void			ixaFontFinalize(STIXA_Font* font);
//Sound (compressed or uncompressed)
void			ixaSoundInit(STIXA_Sound* sound);
void			ixaSoundFinalize(STIXA_Sound* sound);
//Sprites (movie clips)
void			ixaSpriteInit(STIXA_Sprite* sprite);
void			ixaSpriteFinalize(STIXA_Sprite* sprite);
void			ixaSpriteFrameAdd(STIXA_Sprite* sprite);
void			ixaSpriteSoundEventAdd(STIXA_Sprite* sprite, const STIXA_SoundEvent* soundEvent);
void			ixaSpriteObjectAdd(STIXA_Sprite* sprite, const IxaUI16 depth, const IxaUI16 charID, const char* name);
void			ixaSpriteObjectRemove(STIXA_Sprite* sprite, const IxaUI16 depth);
IxaBOOL			ixaSpriteObjectReplace(STIXA_Sprite* sprite, const IxaUI16 depth, const IxaUI16 charID, const char* name);
void			ixaSpriteObjectSetProps(STIXA_Sprite* sprite, const IxaUI16 depth, const STIXA_Matrix* matrix, const STIXA_CXForm* color);
void			ixaSpriteObjectGetProps(const STIXA_Sprite* sprite, STIXA_SpriteElemProps* dest, const IxaUI16 idElement, const IxaUI16 iFrame);
//SwfFile
void			ixaSwfFileInit(STIXA_SwfFile* swf);
void			ixaSwfFileFinalize(STIXA_SwfFile* swf);
IxaBOOL			ixaSwfLoadFile(STIXA_SwfFile* swf, const char* path, PTRIXA_zlibUncompress funcUncompress);
void			ixaSwfDictAddCharacter(STIXA_SwfFile* swf, const ENIXA_CharType charType, const IxaUI16 charID, void* charPointer);
IxaBOOL			ixaSwfDictGetElemByCharID(const STIXA_SwfFile* swf, STIXA_DiccElem* dest, const IxaUI16 charID);

#ifdef __cplusplus
} //extern "C" {
#endif
		
#endif
