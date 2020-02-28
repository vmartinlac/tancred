#include <lcm/lcm-cpp.hpp>
#include <fstream>
#include <iostream>
#include <MotorCommandMessage.hpp>
#include "common.h"

class Handler
{
public:

    Handler()
    {
    }

    bool init(const char* device)
    {
        myFile.open(device);
        return myFile.is_open();
    }

    void finalize()
    {
        myFile.close();
    }

    void handle(const lcm::ReceiveBuffer* rbuf, const std::string& channel, const MotorCommandMessage* msg)
    {
        myFile << "CMD " << msg->steering << " " << msg->speed << std::endl;
    }

protected:

    std::ofstream myFile;
};

int main(int num_args, char** args)
{
    if(num_args != 2)
    {
        std::cout << "Bad command line!" << std::endl;
        exit(1);
    }

    Handler handler;

    if(handler.init(args[1]) == false)
    {
        std::cout << "Could not initialize message handler!" << std::endl;
        exit(1);
    }

    lcm::LCM conn(LCM_PROVIDER);

    if(conn.good() == false)
    {
        std::cout << "Could not initialize LCM!" << std::endl;
        exit(1);
    }

    conn.subscribe<MotorCommandMessage>("Motors", &Handler::handle, &handler);

    while(true)
    {
        conn.handle();
    }
    
    return 0;
}

