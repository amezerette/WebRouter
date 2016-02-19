# WebRouter


This library provides a simple HTTP router that maps route callbacks
to specific HTTP request methods and URLs.
You may also use wildcard characters in you URL pattern.

    webRouter.on(WebRouter::GET,"/hello/?worl+",callback);

Example sketches are given in the 'examples' folder.


## URL pattern

Only the following wildcard characters are allowed for URL pattern :
  - '*' : zero or more occurrence of any characters
  - '+' : 1 or more occurrence of any characters
  - '?' : any character


## Example
    
    WebRouter webRouter;
    
    // Define your callbacks here !
    void callback(Client& client, const char * url){
        client.print("HTTP/1.0 200 OK\r\n");
        client.print("\r\n");
        client.print("hello");
    }
    void unrouted(Client& client, const char * url){
        client.print("HTTP/1.0 200 OK\r\n");
        client.print("\r\n");
        client.print("404 unrouted url: ");
        client.print(url);
    }
    
    void setup() {
        // only GET request with with URL "/hello" will match :
        webRouter.on(WebRouter::GET,"/hello",callback);
        
        // only POST request with an URL starting with "/foo/" will match :
        webRouter.on(WebRouter::POST,"/foo/*",callback);
        
        // any other request
        webRouter.on(WebRouter::ANY,"*",unrouted);
        
        // setup your server here
        // Ethernet.begin(mac, ip) ... or wifi setup
    }
    
    void loop() {
        // listen for incoming client
        webRouter.loop( server.available() );
	}
