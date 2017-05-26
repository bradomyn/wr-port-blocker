#ifndef CONN_H
#define CONN_H

#include <iostream>
#include "Log.h"

class Conn {
        public:
                virtual int Config() = 0;
                virtual int SendCommand(const char* ) = 0;

};

extern Conn *ConnProto;
#endif
