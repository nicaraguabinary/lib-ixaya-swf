
#----------------------
#ixaya-swf
#----------------------

Ixaya ("abrir los ojos" en Nahuatl) es una librería para la carga de archivos SWF de FLASH. Puede ser utilizada como base para el desarrollo de herramientas que consuman, modifiquen o produzcan archivos SWF.

#----------------------
#Licencia
#----------------------

LGPLv2.1, ver archivo LICENSE

#----------------------
#Origen
#----------------------

Publicada el 01/mar/2014.

Esta librería fue inicialmente desarrollada por Marcos Ortega a partir del documento "SWF FILE FORMAT SPECIFICATION" publicado por Adobe en: http://www.adobe.com 

#----------------------
#Utilidad
#----------------------

Esta librería fue desarrollada con el objetivo de extraer datos desde archivos SWF.

Ixaya-swf aun no extrae la totalidad de objetos dentro de un SWF. La versión actual extrae formas (gráficos vectoriales), recursos binarios (imagenes jpegs y otros), sonidos (mp3s y PCM), lossless (mapas de bits PNG) y movie clips.

#----------------------
#Características de código
#----------------------

Ixaya-swf fue concebida y desarrollada priorizando la facilidad para integrar en otros proyectos, caracterizada por:

- consiste sólo de dos archivos ("ixaya-swf.h" e "ixaya-swf.c").
- obligatoriamente depende de solo estos includes a "stdio.h" y "string.h".
- opcionalmente depende de estos includes "stdlib.h" y "assert.h".
- permite la gestión personalizada de memoria.

Gracias a su estructura actual, Ixaya-swf puede ser integrada a la mayoría de proyectos de código nativo para:

- Windows
- Linux
- Mac
- iOS
- Android
- Blackberry 10
- etc..

#----------------------
#Dependencia para descomprimir ZLIB
#----------------------

Ixaya-swf depende de una función que realice la decompresion de contenido en formato ZLIB. El proyecto donde se vaya a integrara Ixaya-swf deberá proporcionar un callback para descomprimir ZLIB. Ver demos y las invocaciones al método "ixaSwfLoadFile".

#----------------------
#Código de ejemplo
#----------------------

```c
#include "ixaya-swf.h"

...

IxaBOOL uncompressSwfZlib(IxaUI8* ptrDest, const IxaUI32 destLen, const IxaUI8* ptrSrc, const IxaUI32 srcLen);

...

int main(...){
   STIXA_SwfFile swfFile;
   ixaSwfFileInit(&swfFile);
   if(ixaSwfLoadFile(&swfFile, "/swfPath.swf", uncompressSwfZlib)){
      /*
        Explore the STIXA_SwfFile structure:
        movie clips, sounds, shapes, lossless, bits, etc...
      */
   } 
   ixaSwfFileFinalize(&swfFile);
}

```