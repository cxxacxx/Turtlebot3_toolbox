#include <stdio.h>
#include <string.h>
#include "serialib.h"

serialib LS;

int t3_drive_init() {                                                           
    
    // Open serial port
    int Ret;
    Ret=LS.Open("/dev/ttyACM1",57600);                                        // Open serial link at 115200 bauds
    if (Ret!=1) {
    perror("The following error occurred:");                                                           // If an error occured...
        printf ("Error while opening port. Permission problem ?\n");        // ... display a message ...
        return Ret;                                                         // ... quit the application
    }
    return 0;
}

int vel_set() {
    
    int i;
    int Ret;
    char Buffer[20];
    
    Ret=LS.WriteString("enable 1\n");                                             // Send the command on the serial port
    if (Ret!=1) {                                                           // If the writting operation failed ...
        printf ("Error while writing data\n");                              // ... display a message ...
        return Ret;                                                         // ... quit the application.
    }
    printf ("Write operation is successful \n");

    // Read a string from the serial device
    Ret=LS.ReadString(Buffer,'\n',128,10000);                                // Read a maximum of 128 characters with a timeout of 5 seconds                                                                                   // The final character of the string must be a line feed ('\n')
    if (Ret>0)      
        
        // If a string has been read from, print the string
            printf ("String read from serial port : %s \n",Buffer);
    else
        printf ("TimeOut reached. No data received !\n");                   // If not, print a message.
    
    usleep(1000);

    Ret=LS.WriteString("read_PM\n");                                             // Send the command on the serial port
    if (Ret!=1) {                                                           // If the writting operation failed ...
        printf ("Error while writing data\n");                              // ... display a message ...
        return Ret;                                                         // ... quit the application.
    }
    printf ("Write operation is successful \n");

    // Read a string from the serial device
    Ret=LS.ReadString(Buffer,'\n',128,10000);                                // Read a maximum of 128 characters with a timeout of 5 seconds                                                                                   // The final character of the string must be a line feed ('\n')
    if (Ret>0)      
        
        // If a string has been read from, print the string
            printf ("String read from serial port : %s \n",Buffer);
    else
        printf ("TimeOut reached. No data received !\n");                   // If not, print a message.
    
    usleep(1000);

    Ret=LS.WriteString("set_vel 5 5\n");                                             // Send the command on the serial port
    if (Ret!=1) {                                                           // If the writting operation failed ...
        printf ("Error while writing data\n");                              // ... display a message ...
        return Ret;                                                         // ... quit the application.
    }
    printf ("Write operation is successful \n");

    // Read a string from the serial device
    Ret=LS.ReadString(Buffer,'\n',128,10000);                                // Read a maximum of 128 characters with a timeout of 5 seconds                                                                                   // The final character of the string must be a line feed ('\n')
    if (Ret>0)      
        
        // If a string has been read from, print the string
            printf ("String read from serial port : %s \n",Buffer);
    else
        printf ("TimeOut reached. No data received !\n");                   // If not, print a message.
    
    usleep(10000000);

    Ret=LS.WriteString("set_vel 0 0\n");                                             // Send the command on the serial port
    if (Ret!=1) {                                                           // If the writting operation failed ...
        printf ("Error while writing data\n");                              // ... display a message ...
        return Ret;                                                         // ... quit the application.
    }
    printf ("Write operation is successful \n");

    // Read a string from the serial device
    Ret=LS.ReadString(Buffer,'\n',128,10000);                                // Read a maximum of 128 characters with a timeout of 5 seconds                                                                                   // The final character of the string must be a line feed ('\n')
    if (Ret>0)      
        
        // If a string has been read from, print the string
            printf ("String read from serial port : %s \n",Buffer);
    else
        printf ("TimeOut reached. No data received !\n");                   // If not, print a message.
    
    usleep(1000);
        
    
    return 0;
}

void port_close(){
    LS.Close();
}

int main(){
    t3_drive_init();
    vel_set();
    port_close();
    return 0;
}