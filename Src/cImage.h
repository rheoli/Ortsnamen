/************************************************************************
    
    cImage.h - Mit Hilfe der gd-Library k"onnen mathematische Daten
                einfach in ein GIF-File ausgegeben werdem.    
    Copyright (c) 1996-97  by Stephan Toggweiler

    "$Id$"

****************************************************************************/

#ifndef _H_CIMAGE
#define _H_CIMAGE

#include "Allgemein.h" 

class cImage
{
 private:
  // Neue Punktstruktur reservieren und enfuegen.
  void setNewPoint ( void );
  
 public:
  // Initialisieren des Objektes 
  cImage ( void );

  // Loeschen des Images
  ~cImage ( void );

  // Neues Image erstellen
  BOOL newImage ( int _iX, int _iY );

  // Aktuelles Image speichern als GIF-Bild
  BOOL saveImage ( char *_pszFilename, char *_pszText );

  // Das Anzeige-Window wird direkt festgelegt
  BOOL setVirtualWindowS ( double _dMin, double _dMax );

  // Das Anzeige-Window wird direkt festgelegt
  BOOL setVirtualWindow ( double _fX, double _fY, double _fX2, double _fY2 );

  // theoretische Pixelgroesse bei bereits gegebenem virtuellem Window
  // berechnen.
  BOOL setVirtualDiff ( void );

  // Vordergrundfarbe setzen
  BOOL setColorS ( int _iColor );

  // Vordergrundfarbe setzen
  BOOL setColor ( int _iColor );

  // Hintergrundfarbe setzen
  BOOL setBackgroundColor ( int _iColor );

  // Aktuelle Cursorposition ver"andern (mit automatischer virtueller Window-
  // groesseberechnung)
  BOOL moveToS ( double _dX, double _dY );

  // Aktuelle Cursorposition ver"andern
  BOOL moveTo ( double _fX, double _fY );

  // Linie von aktueller Cursorposition nach (_dX,_dY) zeichenen (mit
  // automatischer virtueller Windowgroesseberechnung)
  BOOL lineToS ( double _dX, double _dY );

  // Linie von aktueller Cursorposition nach (_fX,_fY) zeichenen 
  BOOL lineTo ( double _fX, double _fY );

  // Viereck mit den Ausmassen von aktueller Cursorposition nach (_dX,_dY)
  // (mit automatischer virtueller Windowgroesseberechnung)
  BOOL rectangleS ( double _dX, double _dY );

  // Viereck mit den Ausmassen von aktueller Cursorposition nach (_fX,_fY) 
  BOOL rectangle ( double _fX, double _fY );

  // Viereck gefuellt mit den Ausmassen von aktueller Cursorposition nach
  // (_dX,_dY) (mit automatischer virtueller Windowgroesseberechnung)
  BOOL rectangleFilledS ( double _dX, double _dY );

  // Viereck gefuellt mit den Ausmassen von aktueller Cursorposition
  // nach (_dX,_dY)
  BOOL rectangleFilled ( double _dX, double _dY );

  // Funktion zum Zeichnen der mit den *S-Funktionen eingegebenen
  // Zeichenstrukturen
  BOOL paintS ( void );

  // Pixel auf Bildschirm zeichen
  BOOL putPixel ( double _fX, double _fY, int _iColor );
   
  // Linie zeichnen
  BOOL putLine ( double _fX, double _fY, double _fX2, double _fY2, int _iColor );

  // Viereck zeichnen
  BOOL putRectangle ( double _fX, double _fY, double _fX2, double _fY2, int _iColor );

  // Viereck zeichnen gefuellt
  BOOL putFilledRectangle ( double _dX, double _dY, double _dX2, double _dY2, int _iColor );
};

#endif // _H_CIMAGE
