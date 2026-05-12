/*


*/

#ifndef __TASK_H__
#define __TASK_H__

#include "Config.h"

class Task
{
  private:

  public:
    Task() = delete;
    static void begin();
    static void task1(void *);
};

#endif
