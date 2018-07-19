#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>   
#include<sys/ioctl.h>
#include<unistd.h>  
#include<iostream>
#include<fstream>
#include<errno.h>
#include<vector>
#include <ctime>
#include <chrono>

#include "BoxSerializer.h"

using namespace std;

/**
    Send data to the connected host
*/
bool send_data(int socket,std::string data)
{
    //Send some data
    if( send(socket , data.c_str() , strlen( data.c_str() ) , 0) < 0)
    {
        perror("Send failed : ");
        return false;
    }
    cout<<"Data send\n";
     
    return true;
}
 
/**
    Receive data from the connected host
*/
std::string receive(int socket,int size=512)
{
    char buffer[size];
    std::string reply;
     
    //Receive a reply from the server
    if( recv(socket , buffer , sizeof(buffer) , 0) < 0)
    {
        puts("recv failed");
    }
     
    reply = buffer;
    return reply;
}

int send_image(int socket)
{
        
auto begin = std::chrono::steady_clock::now();
  FILE *picture;
  int size, read_size, stat, packet_index;
  char send_buffer[10240], read_buffer[256];
  packet_index = 1;
    
 // picture = fopen("IMG_20180311_114857.jpg", "r");
    
  picture = fopen("IMG_20180305_191422.jpg", "r");
  printf("Getting Picture Size\n");   
  if(picture == NULL) {
       printf("Error Opening Image File"); } 
  fseek(picture, 0, SEEK_END);
  size = ftell(picture);
  fseek(picture, 0, SEEK_SET);
  printf("Total Picture size: %i\n",size);
  //Send Picture Size
  printf("Sending Picture Size\n");
  write(socket, (void *)&size, sizeof(int));
  //Send Picture as Byte Array
  printf("Sending Picture as Byte Array\n");
  do { //Read while we get errors that are due to signals.
     stat=read(socket, &read_buffer , 255);
     printf("Bytes read: %i\n",stat);
  } while (stat < 0);
  printf("Received data in socket\n");
  //printf("Socket data: %c\n", read_buffer);

    auto end = std::chrono::steady_clock::now();
  
  auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
  std::cout << "The time: " << elapsed_ms.count() << " ms\n";
  while(!feof(picture)) 
  {
   //while(packet_index = 1){
    //Read from the file into our send buffer
    read_size = fread(send_buffer, 1, sizeof(send_buffer)-1, picture);
    //Send data through our socket 
    do{
      stat = write(socket, send_buffer, read_size);  
    }while (stat < 0);
    printf("Packet Number: %i\n",packet_index);
    printf("Packet Size Sent: %i\n",read_size);     
    printf(" \n");
    printf(" \n");
    packet_index++;  
    //Zero out our send buffer
    bzero(send_buffer, sizeof(send_buffer));
  }
  std::string receiveString=receive(socket);
  std::cout<<receiveString<<std::endl;
  std::vector<bbox_t> receiveBbox_t= BoxSerializer::fromString(receiveString);
  for(auto box:receiveBbox_t)
  std::cout<<" w= "<<box.w<<std::endl;

}

  int main(int argc , char *argv[])
  {
    
   // unsigned int start_time =  clock(); 
  int socket_desc;
  struct sockaddr_in server;
  char *parray;


  //Create socket
  socket_desc = socket(AF_INET , SOCK_STREAM , 0);

  if (socket_desc == -1) {
  printf("Could not create socket");
  }

  memset(&server,0,sizeof(server));
  server.sin_addr.s_addr = inet_addr("192.168.90.103");
  server.sin_family = AF_INET;
  server.sin_port = htons( 8889 );

  //Connect to remote server
  if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0) {
  cout<<strerror(errno);
  close(socket_desc);
  puts("Connect Error");
  return 1;
  }

  puts("Connected\n");

    //auto begin = std::chrono::steady_clock::now();
  send_image(socket_desc);
    //auto end = std::chrono::steady_clock::now();
  
  //auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
  //std::cout << "The time: " << elapsed_ms.count() << " ms\n";

  close(socket_desc);

 
 // unsigned int end_time = clock(); // конечное время
 // unsigned int search_time = end_time - start_time; // искомое время
 // std::cout<<"During= "<<search_time<<std::endl;
  return 0;
  }