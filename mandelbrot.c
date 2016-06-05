/*  Garwerd Liang, Kelvin Lau
 *  bmpServer.c
 *  1917 serve that 3x3 bmp from lab3 Image activity
 *
 *  Created by Tim Lambert on 02/04/12.
 *  Containing code created by Richard Buckland on 28/01/11.
 *  Copyright 2012 Licensed under Creative Commons SA-BY-NC 3.0. 
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <math.h>

#include "pixelColor.h"
#include "mandelbrot.h"

typedef struct _complex{
    double a; 
    double b;
}complex;

typedef unsigned char  bits8;
typedef unsigned short bits16;
typedef unsigned int   bits32;

int waitForConnection (int serverSocket);
int makeServerSocket (int portno);
void serveBMP (int socket, double x, double y, double zoomLevel);
complex squareComplex(complex num);
complex addComplex(complex num1, complex num2);
//int isComplexMandelbrot (complex t);
void writeHeader(int socket);
int escapeSteps (double x, double y);

#define SIMPLE_SERVER_VERSION 1.0
#define REQUEST_BUFFER_SIZE 1000
#define DEFAULT_PORT 6969
#define NUMBER_OF_PAGES_TO_SERVE 10

#define BYTES_PER_PIXEL 3
#define BITS_PER_PIXEL (BYTES_PER_PIXEL*8)
#define NUMBER_PLANES 1
#define PIX_PER_METRE 2835
#define MAGIC_NUMBER 0x4d42
#define NO_COMPRESSION 0
#define OFFSET 54
#define DIB_HEADER_SIZE 40
#define NUM_COLORS 0
#define SIZE_X 512
#define SIZE_Y 512
// after serving this many pages the server will halt

int main (int argc, char *argv[]) {
    double x,y;
    double zoomLevel;
      
   printf ("************************************\n");
   printf ("Starting simple server %f\n", SIMPLE_SERVER_VERSION);
   printf ("Serving bmps since 2012\n");   
   
   int serverSocket = makeServerSocket (DEFAULT_PORT);   
   printf ("Access this server at http://localhost:%d/\n", DEFAULT_PORT);
   printf ("************************************\n");
   
   char request[REQUEST_BUFFER_SIZE];
   
   int numberServed = 0;
   while (numberServed < NUMBER_OF_PAGES_TO_SERVE) {
      
      printf ("*** So far served %d pages ***\n", numberServed);
      
      int connectionSocket = waitForConnection (serverSocket);
      // wait for a request to be sent from a web browser, open a new
      // connection for this conversation
      
      // read the first line of the request sent by the browser  
      int bytesRead;
      bytesRead = read (connectionSocket, request, (sizeof request)-1);
      assert (bytesRead >= 0);
      // were we able to read any data from the connection?
            
      // print entire request to the console 
      printf (" *** Received http request ***\n %s\n", request);
      
      //send the browser a simple html page using http
      printf (" *** Sending http response ***\n");
      
      // close the connection after sending the page- keep aust beautiful
	  int flag = 0;
	  flag = sscanf(request,"GET /tile_x-%lf_y%lf_z%lf.bmp", &x, &y, &zoomLevel);
	  if (flag){
		  serveBMP (connectionSocket, x, y, zoomLevel);
	  }else{
            char* message;
 
         message =
           "HTTP/1.0 200 Found\n"
           "Content-Type: text/html\n"
           "\n";
         printf ("about to send=> %s\n", message);
         write (connectionSocket, message, strlen (message));
 
         message =
             "<!DOCTYPE html>\n"
             "<script src=\"http://almondbread.cse.unsw.edu.au/tiles.js\"></script>"
             "\n";      
         write (connectionSocket, message, strlen (message));
	  }
    
	  
      close(connectionSocket);
      
      numberServed++;
   } 
   
   // close the server connection after we are done- keep aust beautiful
   printf ("** shutting down the server **\n");
   close (serverSocket);
   
   return EXIT_SUCCESS; 
}

void serveBMP (int socket, double x, double y, double zoomLevel) {
   char* message;
   
   // first send the http response header
   
   // (if you write stings one after another like this on separate
   // lines the c compiler kindly joins them togther for you into
   // one long string)
   message = "HTTP/1.0 200 OK\r\n"
                "Content-Type: image/bmp\r\n"
                "\r\n";
   printf ("about to send=> %s\n", message);
   write (socket, message, strlen (message));
   writeHeader(socket);
   // now send the BMP
   double w = 2;

   double distancePixel = pow(w, -zoomLevel);

   double startx = x - ((SIZE_X/2)-1) * distancePixel;
   double starty = y - ((SIZE_Y/2)-1) * distancePixel;

   int row = 0;
   int col = 0;

   unsigned char red, blue, green;
   int steps = 0;

     while (row < SIZE_Y){
      col = 0;
      while (col < SIZE_X){
         steps = escapeSteps(startx, starty);

         if (steps == 0){
            red = 0;
            blue = 0;
            green = 0;
         }else{
            red = stepsToRed (steps);
            blue = stepsToBlue (steps);
            green = stepsToGreen (steps);
         //colour stuff goes here
         }
         write (socket, &blue, sizeof blue);
         write (socket, &green, sizeof green);
         write (socket, &red, sizeof red);
         startx = startx + distancePixel;
         col++;
      }
      startx = x - ((SIZE_X/2)-1) * distancePixel;
      starty = starty + distancePixel;
      row++;
}
}


// start the server listening on the specified port number
int makeServerSocket (int portNumber) { 
   
   // create socket
   int serverSocket = socket (AF_INET, SOCK_STREAM, 0);
   assert (serverSocket >= 0);   
   // error opening socket
   
   // bind socket to listening port
   struct sockaddr_in serverAddress;
   memset ((char *) &serverAddress, 0,sizeof (serverAddress));
   
   serverAddress.sin_family      = AF_INET;
   serverAddress.sin_addr.s_addr = INADDR_ANY;
   serverAddress.sin_port        = htons (portNumber);
   
   // let the server start immediately after a previous shutdown
   int optionValue = 1;
   setsockopt (
      serverSocket,
      SOL_SOCKET,
      SO_REUSEADDR,
      &optionValue, 
      sizeof(int)
   );

   int bindSuccess = 
      bind (
         serverSocket, 
         (struct sockaddr *) &serverAddress,
         sizeof (serverAddress)
      );
   
   assert (bindSuccess >= 0);
   // if this assert fails wait a short while to let the operating 
   // system clear the port before trying again
   
   return serverSocket;
}

// wait for a browser to request a connection,
// returns the socket on which the conversation will take place
int waitForConnection (int serverSocket) {
   // listen for a connection
   const int serverMaxBacklog = 10;
   listen (serverSocket, serverMaxBacklog);
   
   // accept the connection
   struct sockaddr_in clientAddress;
   socklen_t clientLen = sizeof (clientAddress);
   int connectionSocket = 
      accept (
         serverSocket, 
         (struct sockaddr *) &clientAddress, 
         &clientLen
      );
   
   assert (connectionSocket >= 0);
   // error on accept
   
   return (connectionSocket);
}

complex squareComplex(complex num) {
    complex result;
    result.a = num.a*num.a - num.b*num.b;
    result.b = 2 * num.a * num.b;
    return result;
}

complex addComplex(complex num1, complex num2) {
    complex result;
    result.a = num1.a + num2.a;
    result.b = num1.b + num2.b;
    return result;
}

int escapeSteps (double x, double y) {
    complex c;
    c.a = x;
    c.b = y;

    complex z;
    z.a = 0;
    z.b = 0;

    int result = 0;

    int i = 1;
    while (i < 256){
        z = addComplex(squareComplex(z), c);
        if (z.a*z.a + z.b*z.b > 4) {
            result = i;
            break;
        }
        i++;
    }
    result = i;
    return result;
}

void writeHeader(int socket) {

    bits16 magicNumber[1] = {MAGIC_NUMBER};
    write (socket, magicNumber, sizeof magicNumber);

    bits32 fileSize[1] = {OFFSET + (SIZE_X * SIZE_Y * BYTES_PER_PIXEL)};
    write (socket, fileSize, sizeof fileSize[1]);

    bits32 reserved[1] = {0};
    write (socket, reserved, sizeof reserved);

    bits32 offset[1] = {OFFSET};
    write (socket, offset, sizeof offset);

    bits32 dibHeaderSize[1] = {DIB_HEADER_SIZE};
    write (socket, dibHeaderSize, sizeof dibHeaderSize);

    bits32 width[1] = {SIZE_X};
    write (socket, width, sizeof width);

    bits32 height[1] = {SIZE_Y};
    write (socket, height, sizeof height);

    bits16 planes[1] = {NUMBER_PLANES};
    write (socket, planes, sizeof planes);

    bits16 bitsPerPixel[1] = {BITS_PER_PIXEL};
    write (socket, bitsPerPixel, sizeof bitsPerPixel);

    bits32 compression[1] = {NO_COMPRESSION};
    write (socket, compression, sizeof compression);

    bits32 imageSize[1] = {(SIZE_X * SIZE_Y * BYTES_PER_PIXEL)};
    write (socket, imageSize, sizeof imageSize);

    bits32 hResolution[1] = {PIX_PER_METRE};
    write (socket, hResolution, sizeof hResolution);

    bits32 vResolution[1] = {PIX_PER_METRE};
    write (socket, vResolution, sizeof vResolution);

    bits32 numColors[1] = {NUM_COLORS};
    write (socket, numColors, sizeof numColors);

    bits32 importantColors[1] = {NUM_COLORS};
    write (socket, importantColors, sizeof importantColors);
}


