/*


*/

#ifndef __SATELLITE__
#define __SATELLITE__

#include <Arduino.h>

class Satellite
{
private:
    uint16_t m_idNode;
    uint8_t m_ip[4];

public:
    Satellite();
    void begin();
    uint16_t id();
    void id(uint16_t);
    static void watchDog(void *);
};

#endif
