//*=*************************************************************************
// Name:    cCGI.cc
// Changes: -
//*=*************************************************************************

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cCGI.h"

  BOOL cCGI::getRequestMethode ( void )
  {
    char *pChar = (char *) getenv ( "REQUEST_METHOD" );
    assert ( pChar != NULL );
    if ( !strncmp(pChar, "GET", 7) )
    {
      eReqMethode = kGet;
      return ( TRUE );
    }
    if ( !strncmp(pChar, "POST", 7) )
    {
      eReqMethode = kPost;
      return ( TRUE );
    }
    assert ( eReqMethode != kInvalid );
    return ( FALSE );  // Der Ordnung (Compiler) halber eingef"ugt.
  }  

  BOOL cCGI::getQueryString ( void )
  {
    char *pChar = (char *) getenv ( "QUERY_STRING" );
    assert ( pChar != NULL );
    strncpy ( szQueryString, pChar, 199 );
    szQueryString[199] = '\0';
    return ( TRUE );
  }  

  BOOL cCGI::unescapeSpace ( char *_pChar )
  {
    while ( *_pChar != '\0' )
    {
      if ( *_pChar == '+' )
        *_pChar = ' ';
      _pChar++;
    }
  }
  
  int cCGI::char2number ( char _cChar )
  {
    if ( (_cChar>='A') && (_cChar<='F') )
    {
      return ( _cChar - 'A' + 10 );
    }
    if ( (_cChar>='0') && (_cChar<='9') )
    {
      return ( _cChar - '0' );
    }
    return ( 16 );
  }      
  
  BOOL cCGI::unescapeHex ( char *_pChar, char *_pRet )
  {
    int iRet = 0;
    int iZR  = 0;
    iRet = char2number ( *_pChar );
//    fprintf ( stderr, "pChar: %c / pRet: %d<P>\n", *_pChar, iRet );
    assert ( iRet <= 15 );
    iZR = char2number ( *(_pChar+1) );
//    fprintf ( stderr, "pChar: %c / cZR: %d<P>\n", *(_pChar+1), iZR );
    assert ( iZR <= 15 );
    iRet *= 16;
    iRet += iZR;
//    fprintf ( stderr, "pRet: %d, %c<P>\n", iRet, (char)iRet );
    *_pRet = (char)iRet;
    return ( TRUE );
  }
  
  BOOL cCGI::getParam ( char *_pOld, char **_pNew )
  {
    char   *pChar2 = strchr ( _pOld, '&' );
    char   *pChar  = strchr ( _pOld, '=' );
    BOOL   fEnd = FALSE;
    sParam *pQuery = NULL;
    assert ( pChar != NULL );
    if ( pChar2 == NULL )
    {
      fEnd = TRUE;
    }
    else
    {
      *pChar2 = '\0';
    }
    pQuery = new sParam;
    assert ( pQuery != NULL );
    *pChar = '\0';
    pQuery->pszName = new char[strlen(_pOld)+2];
    assert ( pQuery->pszName != NULL );
    strcpy ( pQuery->pszName, _pOld );
    pChar++;
    pQuery->pszString = new char[strlen(pChar)+1];
    assert ( pQuery->pszString != NULL );
    _pOld = pQuery->pszString;
    unescapeSpace ( pChar );
    while ( *pChar != '\0' )
    {
      printf ( "NewChar: %c<P>\n", *pChar );
      if ( *pChar == '%' )
      {
        unescapeHex ( pChar+1, _pOld );
        pChar += 2;
      }
      else
        *_pOld = *pChar;
      pChar++;
      _pOld++;
    }
    _pOld = '\0';
    pQuery->pNext = NULL;
    if ( pPFirst == NULL )
    {
      pPFirst = pQuery;
      pPLast  = pQuery;
    }
    else
    {
      assert ( pPLast != NULL );
      pPLast->pNext = pQuery;
      pPLast = pQuery;
    }      
    if ( fEnd )
      return ( FALSE );
    *_pNew = pChar2 + 1;
    return ( TRUE );
  }
  
  BOOL cCGI::getQueryParams ( void )
  {
    // Query Aufbau: name=text&name2=text2
    // Spaces werden mit '+' gekennzeichnet.
    // Sonderzeichen => '%nn', z.B. +, &, ...
    // PARAM          ::== '\0' | NAME '=' TEXT { '&' NAME '=' TEXT }.
    // NAME           ::== BUCHSTABE { BUCHSTABE | ZAHL }.
    // TEXT           ::== (BUCHSTABE | SONDERZEICHEN2 | ZAHL)
    //                      { BUCHSTABE | SONDERZEICHEN2 | ZAHL }.
    // BUCHSTABE      ::== 'A' | 'B' | ... | 'z'.
    // SONDERZEICHEN2 ::== SONDERZEICHEN | '%' | '+'.
    // SONDERZEICHEN  ::== '_'.
    char *pOld;
    char *pNew;
    if ( szQueryString[0] == '\0' )
      return ( FALSE );
    pOld = szQueryString;
    while ( getParam(pOld, &pNew) )
      pOld = pNew;
    return ( TRUE );
  }
    
  BOOL cCGI::getContentLength ( void )
  {
    char *pChar = (char *) getenv ( "CONTENT_LENGTH" );
    iContentLength = atoi ( pChar );
    assert ( iContentLength < 200 );
    return ( TRUE );
  }
  
  cCGI::cCGI ( void )
  {
    pPFirst     = NULL;
    pPLast      = NULL;
    eReqMethode = kInvalid;
    assert ( getRequestMethode() );
    if ( eReqMethode == kGet )
    {
      getQueryString ();
      getQueryParams ();
    }
    else
    {
      assert ( !(eReqMethode == kPost) );
    }    
  }

  cCGI::~cCGI ( void )
  {
    while ( pPFirst != NULL )
    {
      sParam *pOld = pPFirst;
      delete pPFirst->pszName;
      delete pPFirst->pszString;
      pPFirst = pPFirst->pNext;
      delete pOld;
    }
  }

  BOOL cCGI::isQuery ( void )
  {
    if ( szQueryString[0] == '\0' )
      return ( FALSE );
    return ( TRUE );
  }

  BOOL cCGI::getStringNumber ( char *_pszName, int _iNumber, char *_pszString )
  {
    sParam *pSearch = pPFirst;
    sParam *pFound  = NULL;
    for ( int i = 0; i < _iNumber; i++ ) {
      pFound = NULL;
      while ( pSearch != NULL ) {
        if ( !strcmp(pSearch->pszName, _pszName) ) {
          pFound = pSearch;
          break;
        }  
        pSearch = pSearch->pNext;
      }
      if ( pFound == NULL )
        return ( FALSE );
      pSearch = pSearch->pNext;
    }
    strcpy ( _pszString, pFound->pszString );
    return ( TRUE );
  }
  
  BOOL cCGI::getString ( char *_pszName, char *_pszString )
  {
    return ( getStringNumber(_pszName, 1, _pszString) );
  }
  
  void cCGI::htmlHeader ( char *_pszCGIName )
  {
    printf ( "Status: 200 Output follows\n" );
    printf ( "Server: %s\n", _pszCGIName );
    printf ( "Content-type: text/html\n\n" );
    fflush ( stdout );
  }
  
  void cCGI::htmlTitle ( char *_pszTitle )
  {
    printf ( "<!DOCTYPE HTML PUBLIC '-//W30//DTD W3 HTML 3.0//EN'>\n" );
    printf ( "<TITLE>%s</TITLE>\n", _pszTitle );
  }

