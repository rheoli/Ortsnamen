/************************************************************************
    
    cImage.cpp - Mit Hilfe der gd-Library k"onnen mathematische Daten
                 einfach in ein GIF-File ausgegeben werdem.
    
    Copyright (C) 1996-97  by Stephan Toggweiler

****************************************************************************/

static const char *imversion = "$Id$";

#include <stdio.h>
#include <stdlib.h>
#include "cImage.h"
extern "C" {
#include <gd.h>
#include <gdfonts.h>
}

enum
{
  kNone            = 0x00,
  kMoveTo          = 0x01,
  kLineTo          = 0x02,
  kRectangle       = 0x03,
  kRectangleFilled = 0x04,
  kSetColor        = 0x05
};

struct tPoint
{
  int    iType;
  double dX;
  double dY;
  tPoint *pNext;
};

struct tVirtualWindow
{
  double dXmin;
  double dYmin;
  double dXmax;
  double dYmax;
  double dDiffX;        // Differenzen zwischen Pixel
  double dDiffY;        //      "          "      "
};

static tPoint         *tFirst;          // Pointer auf Punktestruktur,
static tPoint         *tLast;           // damit automatisch die virtuelle
                                        // Windowgroesse bestimmt werden kann.
static tVirtualWindow tvWindow;         // Virtuelle Windowgroesse
static gdImagePtr     im;               // Image Pointer fuer GD-Library
static int            color[8];         // Maximal 8 Farben benutzbar
static double         dXOld,  dYOld;    // Alte Zeichenposition
static int            iFrontColor;      // Aktuelle Pixelfarbe
static int            iBackColor;       // Hintergrundfarbe
static int            iMaxXPixel;       // max. Aufloesung des Graph-Modes
static int            iMaxYPixel;       // max.     "       "    "  -  "

// Initialisieren des Objektes 
cImage::cImage ( void )
{
  tvWindow.dXmin = 0;
  tvWindow.dXmax = 0;
  tvWindow.dYmin = 0;
  tvWindow.dYmax = 0;
  im             = NULL;
  tFirst         = NULL;
  tLast          = NULL;
  iMaxXPixel     = 0;
  iMaxYPixel     = 0;
  dXOld = dYOld  = 0.0;
}

// Loeschen des Images
cImage::~cImage ( void )
{
  gdImageDestroy ( im );
  if ( tFirst != NULL )
  {
    tPoint *pPoint = tFirst;
    while ( pPoint != NULL )
    {
      tPoint *pFree = pPoint;
      pPoint = pPoint->pNext;
      delete pPoint;
    }
  }
}

// Neues Image erstellen, altes wird geloescht
BOOL cImage::newImage ( int _iX, int _iY )
{
  if ( im != NULL )
    gdImageDestroy ( im );

  // Handle fuer GD-Libarary erstellen
  im = gdImageCreate ( _iX, _iY ); 
   
  // Maximale Pixelanzahl speichern
  iMaxXPixel = _iX;
  iMaxYPixel = _iY;
  dXOld = dYOld = 0.0;

  // Farben initialisieren
  color[0] = gdImageColorAllocate ( im,   0,   0,   0 );
  color[1] = gdImageColorAllocate ( im,   0,   0, 255 );
  color[2] = gdImageColorAllocate ( im,   0, 255,   0 );
  color[3] = gdImageColorAllocate ( im,   0, 255, 255 );
  color[4] = gdImageColorAllocate ( im, 255,   0,   0 );
  color[5] = gdImageColorAllocate ( im, 255,   0, 255 );
  color[6] = gdImageColorAllocate ( im, 255, 255,   0 );
  color[7] = gdImageColorAllocate ( im, 255, 255, 255 );

  return ( TRUE );
}

// Aktuelles Image speichern als GIF-Bild
BOOL cImage::saveImage ( char *_pszFilename, char *_pszText )
{
  FILE *fBild;   
  fBild = fopen ( _pszFilename, "wb" );
  if ( fBild == NULL )
    return ( FALSE );  
  if ( _pszText != NULL )
  {
    gdImageString ( im, gdFontSmall, 2, 10, _pszText, 7 );
  }
  gdImageGif ( im, fBild );  
  fclose ( fBild );   
  return ( TRUE );
}

// Das Anzeige-Window wird direkt festgelegt
BOOL cImage::setVirtualWindowS ( double _dMin, double _dMax )
{
  tvWindow.dXmin = _dMax;
  tvWindow.dXmax = _dMin;
  tvWindow.dYmin = _dMax;
  tvWindow.dYmax = _dMin;
  return ( TRUE );
}

// Das Anzeige-Window wird direkt festgelegt
BOOL cImage::setVirtualWindow ( double _dX, double _dY, double _dX2, double _dY2 )
{
  tvWindow.dXmin  = min(_dX,_dX2);      // Sicherstellen, dass dXmin < dXmax !
  tvWindow.dXmax  = max(_dX,_dX2);      // wird fuer alle Koordinaten durch-
  tvWindow.dYmin  = min(_dY,_dY2);      // gefuehrt
  tvWindow.dYmax  = max(_dY,_dY2);      
  tvWindow.dDiffX = (double)iMaxXPixel / (tvWindow.dXmax-tvWindow.dXmin);
  tvWindow.dDiffY = (double)iMaxYPixel / (tvWindow.dYmax-tvWindow.dYmin);     
  return ( TRUE );
}

// theoretische Pixelgroesse bei bereits gegebenem virtuellem Window
// berechnen.
BOOL cImage::setVirtualDiff ( void )
{
  double dDiff = (tvWindow.dXmax-tvWindow.dXmin)/15.0;
  tvWindow.dXmin -= dDiff;
  tvWindow.dXmax += dDiff;
  dDiff = (tvWindow.dYmax-tvWindow.dYmin)/15.0;
  tvWindow.dYmin -= dDiff;
  tvWindow.dYmax += dDiff;
  tvWindow.dDiffX = (double)iMaxXPixel / (tvWindow.dXmax-tvWindow.dXmin);
  tvWindow.dDiffY = (double)iMaxYPixel / (tvWindow.dYmax-tvWindow.dYmin);     
  return ( TRUE );
}  

// Neue Punktstruktur reservieren und enfuegen.
void cImage::setNewPoint ( void )
{
  if ( tFirst == NULL )
  {
    tFirst = new tPoint;
    tLast  = tFirst;
  }
  else
  {
    tLast->pNext = new tPoint;
    tLast = tLast->pNext;
  }
}

// Vordergrundfarbe setzen
BOOL cImage::setColorS ( int _iColor )
{
  setNewPoint ();
  tLast->iType = kSetColor;
  tLast->dX    = (double)_iColor;
  tLast->dY    = 0.0;
  tLast->pNext = NULL;
  return ( TRUE );
}


// Vordergrundfarbe setzen
BOOL cImage::setColor ( int _iColor )
{
  iFrontColor = _iColor;
  return ( TRUE );
}

// Hintergrundfarbe setzen
BOOL cImage::setBackgroundColor ( int _iColor )
{
  iBackColor = _iColor;
  return ( TRUE );
}

// Aktuelle Cursorposition ver"andern (mit automatischer virtueller Window-
// groesseberechnung)
BOOL cImage::moveToS ( double _dX, double _dY )
{
  setNewPoint ();
  tLast->iType = kMoveTo;
  tLast->dX    = _dX;
  tLast->dY    = _dY;
  tLast->pNext = NULL;
  tvWindow.dXmin = min(tvWindow.dXmin,_dX);
  tvWindow.dXmax = max(tvWindow.dXmax,_dX);
  tvWindow.dYmin = min(tvWindow.dYmin,_dY);
  tvWindow.dYmax = max(tvWindow.dYmax,_dY);
  return ( TRUE );
}

// Aktuelle Cursorposition ver"andern
BOOL cImage::moveTo ( double _dX, double _dY )
{
  dXOld = _dX;
  dYOld = _dY;
  return ( TRUE );
}

// Linie von aktueller Cursorposition nach (_dX,_dY) zeichenen (mit
// automatischer virtueller Windowgroesseberechnung)
BOOL cImage::lineToS ( double _dX, double _dY )
{
  setNewPoint ();
  tLast->iType = kLineTo;
  tLast->dX    = _dX;
  tLast->dY    = _dY;
  tLast->pNext = NULL;
  tvWindow.dXmin = min(tvWindow.dXmin,_dX);
  tvWindow.dXmax = max(tvWindow.dXmax,_dX);
  tvWindow.dYmin = min(tvWindow.dYmin,_dY);
  tvWindow.dYmax = max(tvWindow.dYmax,_dY);
  return ( TRUE );
}

// Linie von aktueller Cursorposition nach (_dX,_dY) zeichenen 
BOOL cImage::lineTo ( double _dX, double _dY )
{
  putLine ( dXOld, dYOld, _dX, _dY, iFrontColor );
  dXOld = _dX;
  dYOld = _dY;
  return ( TRUE );
}

// Viereck mit den Ausmassen von aktueller Cursorposition nach (_dX,_dY)
// (mit automatischer virtueller Windowgroesseberechnung)
BOOL cImage::rectangleS ( double _dX, double _dY )
{
  setNewPoint ();
  tLast->iType = kRectangle;
  tLast->dX    = _dX;
  tLast->dY    = _dY;
  tLast->pNext = NULL;
  tvWindow.dXmin = min(tvWindow.dXmin,_dX);
  tvWindow.dXmax = max(tvWindow.dXmax,_dX);
  tvWindow.dYmin = min(tvWindow.dYmin,_dY);
  tvWindow.dYmax = max(tvWindow.dYmax,_dY);
  return ( TRUE );
}

// Viereck mit den Ausmassen von aktueller Cursorposition nach (_dX,_dY) 
BOOL cImage::rectangle ( double _dX, double _dY )
{
  putRectangle ( dXOld, dYOld, _dX, _dY, iFrontColor );
  dXOld = _dX;
  dYOld = _dY;
  return ( TRUE );
}

// Viereck gefuellt mit den Ausmassen von aktueller Cursorposition nach
// (_dX,_dY) (mit automatischer virtueller Windowgroesseberechnung)
BOOL cImage::rectangleFilledS ( double _dX, double _dY )
{
  setNewPoint ();
  tLast->iType = kRectangleFilled;
  tLast->dX    = _dX;
  tLast->dY    = _dY;
  tLast->pNext = NULL;
  tvWindow.dXmin = min(tvWindow.dXmin,_dX);
  tvWindow.dXmax = max(tvWindow.dXmax,_dX);
  tvWindow.dYmin = min(tvWindow.dYmin,_dY);
  tvWindow.dYmax = max(tvWindow.dYmax,_dY);
  return ( TRUE );
}

// Viereck gefuellt mit den Ausmassen von aktueller Cursorposition
// nach (_dX,_dY)
BOOL cImage::rectangleFilled ( double _dX, double _dY )
{
  putFilledRectangle ( dXOld, dYOld, _dX, _dY, iFrontColor );
  dXOld = _dX;
  dYOld = _dY;
  return ( TRUE );
}

// Funktion zum Zeichnen der mit den *S-Funktionen eingegebenen
// Zeichenstrukturen
BOOL cImage::paintS ( void )
{
  tPoint *pPoint = tFirst;

  setVirtualDiff ();
  while ( pPoint != NULL )
  {
    switch ( pPoint->iType )
    {
      case kMoveTo:
        moveTo ( pPoint->dX, pPoint->dY );
        break;
      case kLineTo:
        lineTo ( pPoint->dX, pPoint->dY );
        break;
      case kRectangle:
        rectangle ( pPoint->dX, pPoint->dY );
        break;
      case kRectangleFilled:
        rectangleFilled ( pPoint->dX, pPoint->dY );
        break;
      case kSetColor:
        setColor ( (int)pPoint->dX );
        break;
    }
    pPoint = pPoint->pNext;
  }
  return ( TRUE );
}

// Pixel auf Bildschirm zeichen
BOOL cImage::putPixel ( double _dX, double _dY, int _iColor )
{
#ifdef FLIP_X
  int idx = iMaxXPixel-((int)((_dX-tvWindow.dXmin)*tvWindow.dDiffX+0.5));
#else
  int idx = (int)((_dX-tvWindow.dXmin)*tvWindow.dDiffX+0.5);
#endif
#ifdef FLIP_Y
  int idy = iMaxYPixel-((int)((_dY-tvWindow.dYmin)*tvWindow.dDiffY+0.5));
#else
  int idy = (int)((_dY-tvWindow.dYmin)*tvWindow.dDiffY+0.5);
#endif
  gdImageSetPixel ( im, idx, idy, color[_iColor] );   
  return ( TRUE );
}
   
// Linie zeichnen
BOOL cImage::putLine ( double _dX, double _dY, double _dX2, double _dY2, int _iColor )
{
#ifdef FLIP_X
  int idx  = iMaxXPixel-((int)((_dX-tvWindow.dXmin)*tvWindow.dDiffX+0.5));
  int idx2 = iMaxXPixel-((int)((_dX2-tvWindow.dXmin)*tvWindow.dDiffX+0.5));
#else
  int idx  = (int)((_dX-tvWindow.dXmin)*tvWindow.dDiffX+0.5);
  int idx2 = (int)((_dX2-tvWindow.dXmin)*tvWindow.dDiffX+0.5);
#endif
#ifdef FLIP_Y
  int idy  = iMaxYPixel-((int)((_dY-tvWindow.dYmin)*tvWindow.dDiffY+0.5));
  int idy2 = iMaxYPixel-((int)((_dY2-tvWindow.dYmin)*tvWindow.dDiffY+0.5));
#else
  int idy  = (int)((_dY-tvWindow.dYmin)*tvWindow.dDiffY+0.5);
  int idy2 = (int)((_dY2-tvWindow.dYmin)*tvWindow.dDiffY+0.5);
#endif
  gdImageLine ( im, idx, idy, idx2, idy2, color[_iColor] );
  return ( TRUE );
}

// Viereck zeichnen
BOOL cImage::putRectangle ( double _dX, double _dY, double _dX2, double _dY2, int _iColor )
{
#ifdef FLIP_X
  int idx  = iMaxXPixel-((int)((_dX-tvWindow.dXmin)*tvWindow.dDiffX+0.5));
  int idx2 = iMaxXPixel-((int)((_dX2-tvWindow.dXmin)*tvWindow.dDiffX+0.5));
#else
  int idx  = (int)((_dX-tvWindow.dXmin)*tvWindow.dDiffX+0.5);
  int idx2 = (int)((_dX2-tvWindow.dXmin)*tvWindow.dDiffX+0.5);
#endif
#ifdef FLIP_Y
  int idy  = iMaxYPixel-((int)((_dY-tvWindow.dYmin)*tvWindow.dDiffY+0.5));
  int idy2 = iMaxYPixel-((int)((_dY2-tvWindow.dYmin)*tvWindow.dDiffY+0.5));
#else
  int idy  = (int)((_dY-tvWindow.dYmin)*tvWindow.dDiffY+0.5);
  int idy2 = (int)((_dY2-tvWindow.dYmin)*tvWindow.dDiffY+0.5);
#endif
  gdImageRectangle ( im, min(idx,idx2), min(idy,idy2), max(idx,idx2), max(idy,idy2), color[_iColor] );
  return ( TRUE );
}

// Viereck zeichnen gefuellt
BOOL cImage::putFilledRectangle ( double _dX, double _dY, double _dX2, double _dY2, int _iColor )
{
#ifdef FLIP_X
  int idx  = iMaxXPixel-((int)((_dX-tvWindow.dXmin)*tvWindow.dDiffX+0.5));
  int idx2 = iMaxXPixel-((int)((_dX2-tvWindow.dXmin)*tvWindow.dDiffX+0.5));
#else
  int idx  = (int)((_dX-tvWindow.dXmin)*tvWindow.dDiffX+0.5);
  int idx2 = (int)((_dX2-tvWindow.dXmin)*tvWindow.dDiffX+0.5);
#endif
#ifdef FLIP_Y
  int idy  = iMaxYPixel-((int)((_dY-tvWindow.dYmin)*tvWindow.dDiffY+0.5));
  int idy2 = iMaxYPixel-((int)((_dY2-tvWindow.dYmin)*tvWindow.dDiffY+0.5));
#else
  int idy  = (int)((_dY-tvWindow.dYmin)*tvWindow.dDiffY+0.5);
  int idy2 = (int)((_dY2-tvWindow.dYmin)*tvWindow.dDiffY+0.5);
#endif
  gdImageFilledRectangle ( im, min(idx,idx2), min(idy,idy2), max(idx,idx2), max(idy,idy2), color[_iColor] );
  return ( TRUE );
}

