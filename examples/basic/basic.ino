/*
  WebRouter Example
  
  
  A simple web server that use the WebRouter library
  that maps route callbacks to specific HTTP request methods and URLs.
  
  
  Once you have uploaded this sketch into your arduino,
  open the serial monitor to get the IP address of your arduino.
  Then open a browser and go to :
    http://<ARDUINO_IP>/hello/world
  Your arduino should answer you "hello world".
  For any other URL, the arduino should answer "404 unrouted url: ..."
  
  
  This example use the Ethernet library.

  
  created 4 Nov 2015
  by Adrien Mezerette
  
*/

#include <SPI.h>
#include <Ethernet.h>
#include <WebRouter.h>

// see the WebServer example of the Ethernet library to get
// more details about the configuration of your Ethernet shield.
//
// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 1, 177);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);


// Instantiate a WebRouter object :
WebRouter webRouter;

/*
	Define your callbacks here !
*/
void hello(Client& client, const char * url){
	client.print("HTTP/1.0 200 OK\r\n");
	client.print("\r\n");
	client.print("hello ");
	// get the name from the url
	url++;
	while(*url++!='/');
	client.print(url);
}

void unrouted(Client& client, const char * url){
	client.print("HTTP/1.0 200 OK\r\n");
	client.print("\r\n");
	client.print("404 unrouted url: ");
	client.print(url);
}


void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  // set the routes :
  webRouter.on(WebRouter::GET,"/hello/+",hello);
  webRouter.on(WebRouter::ANY,"*",unrouted);
  

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  
}


void loop() {
	EthernetClient client = server.available();
	if (client) {
		if(webRouter.loop(client))
			Serial.println("Received a request !");
	}
}
