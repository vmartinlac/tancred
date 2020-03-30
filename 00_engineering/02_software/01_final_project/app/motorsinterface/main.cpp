#include <fstream>
#include <iostream>
#include <MotorsMessage.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Network.h>

class Glue : public yarp::os::TypedReaderCallback<MotorsMessage>
{
public:

    void init(const std::string& device)
    {
        myFile.open(device);

        if( myFile.is_open() == false )
        {
            std::cerr << "Could not open arduino interface!" << std::endl;
            exit(1);
        }
    }

    void finalize()
    {
        myFile.close();
    }

    void onRead(MotorsMessage& msg)
    {
        const char forward = (msg.forward) ? 'R' : 'S';
        myFile << forward << msg.steering << std::endl;
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

    Glue glue;

    glue.init(args[1]);

    yarp::os::Network conn;

    yarp::os::BufferedPort<MotorsMessage> port;
    port.open("/motorsinterface/input/motors");
    port.useCallback(glue);

    std::cout << "Press ENTER to quit" << std::endl;
    getchar();

    port.close();
    glue.finalize();
    
    return 0;
}

