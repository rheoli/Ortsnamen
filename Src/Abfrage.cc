
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>
#include "cImage.h"
#include "cCGI.h"
#include "Grenze.h"

#define FILENAME "/local/www/htdocs/tmp/on%d.gif"

#define DEBUG

void main ( int argc, char *argv[] )
{
  MYSQL    mySQL;
  int      iDB;
  int      iRows;
  MYSQL_RES *mQuery;
  MYSQL_ROW    mRow;
  char     szQuery[200];
  cImage   oIm;
  int      iAll = 0;
  char     szSearchString[200];
  char     szFilename[100];
  FILE     *fCount;
  int      iCount;
  cCGI     oCGI;

  oCGI.htmlHeader ( "Abfrage.cgi" );
  oCGI.htmlTitle ( "Ortsnamen in der Schweiz" );

  if ( !oCGI.isQuery() ) {
    printf ( "<H1>Abfrage fuer schweizer Ortsnamen</H1><P>\n" );
    printf ( "<FORM METHOD=GET ACTION=/cgi-bin/Abfrage.cgi>\n" );
    printf ( "<INPUT NAME=\"ortsname\"><P>\n" );
    printf ( "<INPUT TYPE=submit VALUE=\"Ortsname(n) suchen\"><P>\n" );
    printf ( "</FORM>\n" );
    fflush ( stdout );
    exit ( 0 );
  }

  oIm.newImage ( 400, 250 );  // Verhaeltnis 8/5 einhalten !
  oIm.setVirtualWindow ( 480.0, 70.0, 850.0, 300.0 );

  printf ( "<H1>Ortsnamen der Schweiz (Resultat)</H1><P>\n" );
  printf ( "<HR><P>\n" );
  printf ( "<H3>Folgende(r) Ortsname(n) wurde(n) gefunden:</H3><P>\n" );

  if ( !oCGI.getString("ortsname", szSearchString) ) {
    printf ( "<H3>Fehler in Programm!!!!</H3><P>\n" );
    fflush ( stdout );
    exit ( 1 );
  }
  if ( szSearchString[0] == '*' ) {
    iAll = 1;
  }

  fCount = fopen ( "/local/www/count/count.abfrage", "rt" );
  fscanf ( fCount, "%d", &iCount );
  fclose ( fCount );
  fCount = fopen ( "/local/www/count/count.abfrage", "wt" );
  fprintf ( fCount, "%d\n", iCount+1 );
  fclose ( fCount );  
  sprintf ( szFilename, FILENAME, iCount );

  if ( !mysql_connect(&mySQL, "localhost", "guest", "") ) {
    printf ( "mySQLError: No Connection %s<P>\n", mysql_error(&mySQL) );
    exit ( 1 );
  }

  if ( mysql_select_db(&mySQL, "Ortsnamen") < 0 ) {
    printf ( "mySQLError: %s<P>\n", mysql_error(&mySQL) );
    mysql_close ( &mySQL );
    exit ( 1 );
  }
    
  if ( iAll ) {
    sprintf ( szQuery, "select * from OName order by Name" );
  }  
  else {
    sprintf ( szQuery, "select * from OName where Name LIKE '%s' order by Name", szSearchString );
    printf ( "select: %s<P>\n", szSearchString );
  }
  if ( mysql_query(&mySQL, szQuery) || !(mQuery=mysql_store_result(&mySQL)) ) {
    printf ( "mySQLError: %s<P>\n", mysql_error(&mySQL) );
    mysql_close ( &mySQL );
    exit ( 1 );
  }
  paintGrenze ( oIm, "Schweiz", 3 );
//  paintGrenze ( oIm, "KZG", 4 );
//  paintGrenze ( oIm, "KAG", 5 );
//  paintGrenze ( oIm, "KVD", 6 );
//  paintGrenze ( oIm, "KSG", 7 );
  oIm.setColor ( 2 );
  while ( (mRow=mysql_fetch_row(mQuery)) ) {    
    double    x, y;
    MYSQL_RES *mQuery2;
    MYSQL_ROW mRow2;
    sprintf ( szQuery, "select * from Daten where ID=%d", atoi(mRow[0]) );
    if ( mysql_query(&mySQL, szQuery) || !(mQuery2=mysql_store_result(&mySQL)) ) {
      printf ( "mySQLError: %s<P>\n", mysql_error(&mySQL) );
      mysql_close ( &mySQL );
      exit ( 1 );
    }
    if ( (mRow2=mysql_fetch_row(mQuery2)) ) {
      x = ((double)atoi(mRow2[5]))/100.0;
      y = ((double)atoi(mRow2[6]))/100.0;
      oIm.moveTo ( x, y );
      oIm.rectangleFilled ( x+1.0, y+1.0 );
    }
    else {
      printf ( "Koordinaten zu %s nicht gefunden !<P>\n", szQuery );
    }
    mysql_free_result ( mQuery2 );
  }

  if ( iAll ) {
    oIm.saveImage (szFilename, NULL );
  }
  else {
    oIm.saveImage ( szFilename, szSearchString );
  }
  mysql_free_result ( mQuery );
  mysql_close ( &mySQL );
  printf ( "<CENTER>\n" );
  printf ( "<IMG SRC=\"/tmp/on%d.gif\" ALT=\"Ortsnamen der Schweiz\"><P>\n", iCount );
  printf ( "</CENTER>\n" );
  printf ( "<HR>\n" );
  fflush ( stdout );
}
