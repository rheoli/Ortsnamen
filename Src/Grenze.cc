/* Grenze.cc - Zeichet die Grenzen der in Parameter angegebenen Gruppe. */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "cImage.h"
#include <mysql/mysql.h>

void paintGrenze ( cImage &_oIm, char *_pszName, int _iColor )
{
  MYSQL      mySQL;
  int      iDB;
  int      iRows;
  MYSQL_RES *mQuery;
  MYSQL_ROW    mRow;
  MYSQL_FIELD  *mField;
  char     szQuery[200];

  if ( !mysql_connect(&mySQL, "localhost", "guest", "") )
  {
    fprintf ( stderr, "mySQLError: No Connection\n" );
    exit ( 1 );
  }
  if ( mysql_select_db(&mySQL, "Koordinaten") < 0 )
  {
    fprintf ( stderr, "mySQLError: %s\n", mysql_error(&mySQL) );
    mysql_close ( &mySQL );
    exit ( 1 );
  }
  sprintf ( szQuery, "select * from %s", _pszName );
  if ( mysql_query(&mySQL, szQuery) || !(mQuery=mysql_store_result(&mySQL)) )
  {
    fprintf ( stderr, "mySQLError: %s\n", mysql_error(&mySQL) );
    mysql_close ( &mySQL );
    exit ( 1 );
  }

  _oIm.setColor ( _iColor );
  while ( (mRow=mysql_fetch_row(mQuery)) )
  {
    double x, y;
    switch ( mRow[0][0] )
    {
      case '1':
        x = ((double)atoi(mRow[1]))/100.0;
        y = ((double)atoi(mRow[2]))/100.0;
        _oIm.moveTo ( x, y );
        break;
      case '2':
        x = ((double)atoi(mRow[1]))/100.0;
        y = ((double)atoi(mRow[2]))/100.0;
        _oIm.lineTo ( x, y );
        break;
      default:
        printf ( "Falscher Index\n" );       
    }
  }
  mysql_free_result ( mQuery );
  mysql_close ( &mySQL );
}
