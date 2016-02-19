//! 
//! \file
//! \brief Simple HTTP router library
//! \author Adrien Mezerette
//! \date 2015/11/04
//! \version 0.1
//! 
//! \section LICENSE
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

#ifndef _WEBROUTER_H_
#define _WEBROUTER_H_



#include <Client.h>


//!
//! \def WEBROUTER_MAX_ROUTE
//!
//! The maximum number of routes allowed.
//!
#define WEBROUTER_MAX_ROUTE 12

//!
//! The size of the internal buffer used to parse the first line of a HTTP request.
//! The size must be greater than the longest possible URL
//!
#define WEBROUTER_INTERNAL_BUFFER_LENGTH 64


//!
//! \class WebRouter
//!
//! \brief This class provides a simple HTTP router that maps route callbacks to specific HTTP request methods and URLs.
//!
//! This class provides a simple HTTP router that maps route callbacks to specific HTTP request methods and URLs.
//!
//! You may also use wildcard characters in you URL pattern.
//! Example sketches are given in the 'examples' folder.
//!
//! Example:
//! 
//!     only GET request with with URL "/hello" will match :
//!     webRouter.on(WebRouter::GET,"/hello",callback);
//!     
//!     only POST request with an URL starting with "/foo/" will match :
//!     webRouter.on(WebRouter::POST,"/foo/*",callback);
//!     
//!     any other request
//!     webRouter.on(WebRouter::ANY,"*",unrouted);
//!
//!
//! Only the following wildcard characters are allowed for URL pattern :
//!   - '*' : zero or more occurrence of any characters
//!   - '+' : 1 or more occurrence of any characters
//!   - '?' : any character
//!
class WebRouter
{
	
	public:
		
		//! 
		//! \enum Method
		//! 
		//! Enumerate the allowed HTTP request methods.
		//! 
		enum Method {
			UNKNOWN, //!< for internal use only
			GET,     //!< GET request method selector
			POST,    //!< POST request method selector
			PUT,     //!< PUT request method selector
			PATCH,   //!< PATCH request method selector
			DELETE,  //!< DELETE request method selector
			ANY      //!< select any request method
		};
		//! 
		//! \enum Options
		//! 
		//! Enumerate the possible options.
		//! 
		enum Options {
			NONE         = 0b00000000, //!< no option
			SKIP_HEADER  = 0b00000001, //!< skip the header before executing the route callback.
			SKIP_REQUEST = 0b00000011  //!< skip the whole incoming request before executing the route callback.
		};
		
		
		//! 
		//! The route callback definition.
		//! 
		//! \param client The incoming client.
		//! \param url client The URL of the HTTP request.
		//! 
		typedef void (*RouteCb)(Client& client,const char* url);
		
		
		//!
		//! Create a WebRouter instance.
		//!
		WebRouter();
		
		//!
		//! Add a new route.
		//!
		//! \param method The HTTP request method.
		//! \param url The URL pattern.
		//! \param routeCb The callback to execute when this route matches the incoming request.
		//! \param options Some options.
		//! \return true on success. It returns false only if the number of routes attached is greater than WEBROUTER_MAX_ROUTE.
		//! \sa Method for available method selectors.
		//! \sa RouteCb for the signature of the route callback.
		//!
		bool on(Method method, const char * url, RouteCb routeCb, int options = 0);
		
		//!
		//! Get the number of routes attached to this instance.
		//!
		//! \return the number of routes attached.
		//!
		inline int length() const {
			return itemsNbr;
		}
		
		//!
		//! Process the route evaluation on a new incoming client.
		//!
		//! \param client The new incoming client.
		//! \return true if a route matches.
		//!
		bool loop(Client & client);
		
		
		
		//!
		//! Print the HTTP response "400 Bad Request" to a client.
		//!
		//! \param client The client to send the response.
		//!
		static inline void fail(Client & client){
			client.print("HTTP/1.0 400 Bad Request\r\n\r\n");
		}
		//!
		//! Print the HTTP response "200 OK" to a client.
		//!
		//! \param client The client to send the response.
		//!
		static inline void success(Client & client){
			client.print("HTTP/1.0 200 OK\r\n\r\n");
		}
		//!
		//! Print the HTTP response "404 Not Found" to a client.
		//!
		//! \param client The client to send the response.
		//!
		static inline void notFound(Client & client){
			client.print("HTTP/1.0 404 Not Found\r\n\r\n");
		}
		
		
		
	private:
		
		struct Item {
			Method method;
			const char * url;
			RouteCb routeCb;
			int options;
		};
		
		Item items[WEBROUTER_MAX_ROUTE];
		int itemsNbr;
		
		
		static int _readBytesUntil(Stream & input,  const char * targets, char * buffer, size_t length);
	public:
		static bool urlMatch(const char * url, const char * pattern);
	
};




#endif /*_WEBROUTER_H_*/

