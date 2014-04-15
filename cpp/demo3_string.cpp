/**************************************************************
 * Description: string for test
 * Date:
 * Others: 
**************************************************************/

#include <iostream>
using std::cerr;
using std::cout;
using std::endl;

#include <iomanip>
using std::setw;

#include <cstring>
using std::strcmp;
using std::strcpy;
using std::strcat;

#include <cstdlib> 
using std::exit;

#include "demo3_string.hpp"

XString::XString( const char *s ) 
   : length( ( s != 0 ) ? strlen( s ) : 0 )
{
   cout << "Conversion (and default) constructor: " << s << endl;
   setXString( s ); 
} 

// copy constructor
XString::XString( const XString &copy ) 
   : length( copy.length )
{
   cout << "Copy constructor: " << copy.sPtr << endl;
   setXString( copy.sPtr );
} 

// Destructor
XString::~XString()
{
   cout << "Destructor: " << sPtr << endl;
   delete [] sPtr; 
} 

// overloaded = operator; avoids self assignment
const XString &XString::operator=( const XString &right )
{
   cout << "operator= called" << endl;

   if ( &right != this ) 
   {         
      delete [] sPtr;
      length = right.length; 
      setXString( right.sPtr ); 
   } 
   else
      cout << "Attempted assignment of a XString to itself" << endl;

   return *this;
} 
const XString &XString::operator+=( const XString &right )
{
   size_t newLength = length + right.length; 
   char *tempPtr = new char[ newLength + 1 ];

   strcpy( tempPtr, sPtr ); // copy sPtr
   strcpy( tempPtr + length, right.sPtr ); 

   delete [] sPtr; 
   sPtr = tempPtr; 
   length = newLength;
   return *this; 
} 

bool XString::operator!() const
{ 
   return length == 0; 
} 

bool XString::operator==( const XString &right ) const
{ 
   return strcmp( sPtr, right.sPtr ) == 0; 
} 
bool XString::operator<( const XString &right ) const
{ 
   return strcmp( sPtr, right.sPtr ) < 0; 
} 
char &XString::operator[]( int subscript )
{
   if ( subscript < 0 || subscript >= length )
   {
      cerr << "Error: Subscript " << subscript 
         << " out of range" << endl;
      exit( 1 ); 
   } 

   return sPtr[ subscript ]; 
} 

char XString::operator[]( int subscript ) const
{
   if ( subscript < 0 || subscript >= length )
   {
      cerr << "Error: Subscript " << subscript 
           << " out of range" << endl;
      exit( 1 ); 
   } 
   return sPtr[ subscript ]; 
} 

XString XString::operator()( int index, int subLength ) const
{
   if ( index < 0 || index >= length || subLength < 0 )  
      return ""; 

   // determine length of substring
   int len;

   if ( ( subLength == 0 ) || ( index + subLength > length ) )
      len = length - index;
   else
      len = subLength;

   // allocate temporary array for substring and 
   // terminating null character
   char *tempPtr = new char[ len + 1 ];

   // copy substring into char array and terminate string
   strncpy( tempPtr, &sPtr[ index ], len );
   tempPtr[ len ] = '\0';

   // create temporary XString object containing the substring
   XString tempXString( tempPtr );
   delete [] tempPtr;
   return tempXString; 
} // end function operator()

int XString::getLength() const 
{ 
   return length; 
} 

// utility function called by constructors and operator=
void XString::setXString( const char *string2 )
{
   sPtr = new char[ length + 1 ];

   if ( string2 != 0 ) 
      strcpy( sPtr, string2 );
   else 
      sPtr[ 0 ] = '\0';
} 

// overloaded output operator
ostream &operator<<( ostream &output, const XString &s )
{
   output << s.sPtr;
   return output;
} 

// overloaded input operator
istream &operator>>( istream &input, XString &s )
{
   char temp[ 100 ]; 
   input >> setw( 100 ) >> temp;
   s = temp; 
   return input; 
} 
