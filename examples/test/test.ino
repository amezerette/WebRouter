/*
  WebRouter Example
  
  This example test some URL pattern matching.
  
  created 4 Nov 2015
  by Adrien Mezerette
  
*/

#include <WebRouter.h>


struct Test {
   char*  url;
   char* pattern;
   bool  result;
};


#define LENGTH 10

Test c[LENGTH] = {
	{ "foo" , "f?o" , true },
	{ "foo" , "foobar" , false },
	{ "foobar" , "foo" , false },
	{ "foobar" , "foo*" , true },
	{ "foobar" , "*bar" , true },
	{ "foobar" , "f*bar" , true },
	{ "foo" , "*foo*" , true },
	{ "foobar" , "foo+" , true },
	{ "foobar" , "fo+" , true },
	{ "/foo/bar/j" , "*/j" , true }
};



void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
  for(int i=0; i<LENGTH; i++){
    
    Serial.print("url: ");
    Serial.print(c[i].url);
    Serial.print(" pattern: ");
    Serial.print(c[i].pattern);
    Serial.print(" result: ");
    
    bool r = WebRouter::urlMatch(c[i].url,c[i].pattern);
    
    Serial.print(r ? "true" : "false");
    
    Serial.print(" status: ");
    
    Serial.println(r==c[i].result ? "passed" : "REJECTED");
    
  }
}


void loop() {}
