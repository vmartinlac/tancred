#include <fstream>
#include <iostream>
#include <MotorsMessage.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Network.h>

int main(int num_args, char** args)
{
    if(num_args != 2)
    {
        std::cout << "Bad command line!" << std::endl;
        exit(1);
    }

    std::ofstream file;
    file.open(args[1]);

    if( file.is_open() == false )
    {
        std::cerr << "Could not open arduino interface!" << std::endl;
        exit(1);
    }

    yarp::os::Network conn;

    yarp::os::BufferedPort<MotorsMessage> port;
    port.open("/motors");

    while(true)
    {
        MotorsMessage* msg = port.read();

        if(msg)
        {
            file << "CMD " << msg->stop << " " << msg->steering << " " << msg->speed << std::endl;
        }
    }

    file.close();
    
    return 0;
}

