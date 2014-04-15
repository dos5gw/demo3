#ifndef _DEMO_STRING_H
#define _DEMO_STRING_H

#include <iostream>
using std::ostream;
using std::istream;

class XString
{
   friend ostream &operator<<( ostream &, const XString & );
   friend istream &operator>>( istream &, XString & );
public:
   XString( const char * = "" ); // conversion/default constructor
   XString( const XString & ); // copy constructor
   ~XString(); // destructor

   const XString &operator=( const XString & ); // assignment operator
   const XString &operator+=( const XString & ); // concatenation operator

   bool operator!() const; // is XString empty?
   bool operator==( const XString & ) const; 
   bool operator<( const XString & ) const;
   bool operator!=( const XString &right ) const
   { 
      return !( *this == right ); 
   } 

   bool operator>( const XString &right ) const
   { 
      return right < *this; 
   } 
 
   bool operator<=( const XString &right ) const
   { 
      return !( right < *this ); 
   }
   
   bool operator>=( const XString &right ) const
   { 
      return !( *this < right ); 
   }

   char &operator[]( int ); 
   char operator[]( int ) const; 
   XString operator()( int, int = 0 ) const;
   int getLength() const; 
private:
   int length; // string length (not counting null terminator)
   char *sPtr; // pointer to start of pointer-based string

   void setXString( const char * ); // utility function
}; // end class XString

#endif
