//! 
//! Copyright 2015 Adrien Mezerette
//! 
//! Licensed under the Apache License, Version 2.0 (the "License");
//! you may not use this file except in compliance with the License.
//! You may obtain a copy of the License at
//! 
//!     http://www.apache.org/licenses/LICENSE-2.0
//! 
//! Unless required by applicable law or agreed to in writing, software
//! distributed under the License is distributed on an "AS IS" BASIS,
//! WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//! See the License for the specific language governing permissions and
//! limitations under the License.
//! 
//! 
//! 

#include "WebRouter.h"



WebRouter::WebRouter() : itemsNbr(0) {}


bool WebRouter::on(Method method, const char * url, RouteCb routeCb, int options) {
	
	if(itemsNbr < WEBROUTER_MAX_ROUTE){
		
		items[itemsNbr].method = method;
		items[itemsNbr].url = url;
		items[itemsNbr].routeCb = routeCb;
		items[itemsNbr].options = options;
		
		itemsNbr++;
		return true;
	}
	
	return false;
}


bool WebRouter::loop(Client & client) {
	if(!client)
		return false;
	
	char buff[WEBROUTER_INTERNAL_BUFFER_LENGTH];
	int buffLength, i;
	Method method = UNKNOWN;
	bool match = false;
	
	// GET /hello.htm HTTP/1.1
	
	/*
	*  - 1 - read the METHOD
	*/
	buffLength = _readBytesUntil(client," ", buff, WEBROUTER_INTERNAL_BUFFER_LENGTH);
	if( buffLength < 3 )
		goto error;
	
	// guess the request method from the first characters
	switch(buff[0]){
		case 'G':
			method = GET;
			break;
		case 'P':
			switch(buff[1]){
				case 'O':
					method = GET;
					break;
				case 'U':
					method = PUT;
					break;
				case 'A':
					method = PATCH;
					break;
			}
			break;
		case 'D':
			method = DELETE;
			break;
	}
	
	if( method == UNKNOWN )
		goto error;
	
	
	
	/*
	*  - 2 - read the URL
	*/
	buffLength = _readBytesUntil(client," ?", buff, WEBROUTER_INTERNAL_BUFFER_LENGTH);
	if( buffLength < 1 )
		goto error;
	
	buff[--buffLength] = '\0';
	
	// does an item match the url ?
	for(i=0;i<WEBROUTER_MAX_ROUTE;i++)
		if((items[i].method == method || items[i].method == ANY) && urlMatch(buff,items[i].url) ){
			// match
			match = true;
			
			// pre-treatment
			if(items[i].options & SKIP_HEADER){
				
				// skip the headers
				client.find("\r\n\r\n");
				
				if(items[i].options & SKIP_REQUEST){
					// todo: read the Content-Length info to avoid the timed read !
					char s[16]; // do not touch the buffer 'buff' because it contains the url which may be useful afterwards
					while(client.readBytes(s,16)==16); // timed read !!!
				}
			}
			
			// call the route callback
			// buff contains the URL
			(*(items[i].routeCb))(client,buff);
			
			goto end;
		}
	
	
	notFound(client);
	goto end;
	
	
error:
	fail(client);
	
end:
	client.stop();
	
	return match;
}



// 
// /!\ the target found is read and correspond to the last char in the buffer !
// 
// Return :
// 	-1 : if the timeout was reached
// 	-2 : if there is a buffer overflow
// 	number of charcters read : if the target was found
// 
int WebRouter::_readBytesUntil(Stream & input,  const char * targets, char * buffer, size_t length) {
	
	unsigned int index = 0;
	int i, ts = strlen(targets);
	char c;
	while (index < length) {
		if( input.readBytes(&c,1) == 0 ) // == timedRead
			return -1;
		if(buffer!=NULL) {
			*buffer++ = c;
		}
		index++;
		for( i=0; i<ts; i++){
			if( c == targets[i])
				return (int)index;
		}
	}
	return -2;
	
}



//    wildcard
//      *          zero or more occurrence of any characters
//      +          1 or more occurrence of any characters
//      ?          any character
//
//	  pattern         url         result
//	  */cmd          test/cmd      true
//	  d/*/cmd        d/test/cmd    true
//	  d/*/cmd        d/e/cmd/g     false
//

bool WebRouter::urlMatch(const char * url, const char * pattern) {
	
	for ( ; *url == *pattern || *pattern == '?' ; url++, pattern++)
		if (*url == '\0')
			return true;
	
	// a difference was found
	switch(*pattern){
	
		case '+':
			if(*url == '\0')
				break; // end reached before comparing ("foo+" vs "foo")
			url++;// skip the first character !
			// treat it as *
			
		case '*':
			
			++pattern;// pattern point now to the target (next char after *)
			
			while(1){
				// skip char until the target was found or end of url reached
				for ( ; *url != *pattern && *url != '\0'; url++);
				
				if(*url != *pattern) // end reached before the target could be found
					break;
				
				// does the url match the pattern ?
				if(WebRouter::urlMatch(url,pattern))
					return true;
				
				// else no match, try to find the target furthermore
				url++;
			}
			
			break;
		
	}
	
	return false;
}

