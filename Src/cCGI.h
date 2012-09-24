#ifndef _H_CCGI
#define _H_CCGI

//*=*************************************************************************
// Name:    cCGI.h
// Changes: -
//*=*************************************************************************

#include "Allgemein.h"

struct sParam
{
  char   *pszName;
  char   *pszString;
  sParam *pNext;
};

/** Auswerten der CGI Environement-Variablen.
    @author  Stephan Toggweiler
    @version 1.00.001
 */

class cCGI
{
 private:
  enum eRequestMethode
  {
    kInvalid = 0x00,
    kPost    = 0x01,
    kGet     = 0x02
  };
  
  char            szQueryString[200];
  eRequestMethode eReqMethode;
  int             iContentLength;
  sParam          *pPFirst;
  sParam          *pPLast;

  // RequestMethode Variable auslesen
  BOOL getRequestMethode ( void );

  // QueryString Variable auslesen
  BOOL getQueryString ( void );

  // '+' durch ' ' ersetzen
  BOOL unescapeSpace ( char *_pChar );
  
  // Zahl aus 2 Zeichen -> Zeichen
  int char2number ( char _cChar );
  
  // '%nn' durch Zeichen ersetzen
  BOOL unescapeHex ( char *_pChar, char *_pRet );
  
  // Parameter auswerten und in Struktur einf"ugen
  BOOL getParam ( char *_pOld, char **_pNew );
  
  // Parameter auswerten
  BOOL getQueryParams ( void );
    
  // ContentLength Variable auslesen
  BOOL getContentLength ( void );
  
 public:
  /**@name Public Methoden
   */
  //@{
  /** Konstruktor
      Auslesen und Auswerten der HTML Parameter aus Environement-Variablen
   */
  cCGI ( void );

  /** Destruktor
      L\"oschen der Pointerstruktur
   */
  ~cCGI ( void );

  /** Ist die CGI-Anfrage eine ausgef\"ullte HTML-Form ?
      @return 	TRUE, wenn ja / FALSE wenn nicht
   */
  BOOL isQuery ( void );

  /** String einer bestimmten HTML-Form zur\"uckgeben.
      @param 	_pszName Name der HTML-Form.
      @param	_iNumber -ter Eintrage mit String Name.
      @param 	_pszString Inhalt der HTML-Form.
      @return 	TRUE, wenn Name vorhanden ist / FALSE sonst
   */
  BOOL getStringNumber ( char *_pszName, int _iNumber, char *_pszString );

  /** String einer bestimmten HTML-Form zur\"uckgeben.
      @param 	_pszName Name der HTML-Form.
      @param 	_pszString Inhalt der HTML-Form.
      @return 	TRUE, wenn Name vorhanden ist / FALSE sonst
   */
  BOOL getString ( char *_pszName, char *_pszString );
  
  /** HTML-Info an Client senden.
      @param 	_pszCGIName Name des CGI-Programms.
   */
  void htmlHeader ( char *_pszCGIName );
  
  /** HTML-Titel ausgeben.
      @param 	_pszTitle Titelstring
   */
  void htmlTitle ( char *_pszTitle );

  //@}
};

#endif
