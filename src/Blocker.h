#ifndef BLOCKER_H
#define BLOCKER_H

#include "Conf.h"

class Blocker
{
        public:
                Blocker(Conf Conf);
                ~Blocker();
                int Run();
};
#endif
