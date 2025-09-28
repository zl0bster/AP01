/* This class is virtual prototype
* for callback realization AKA signal-slot
* without QObject inheritance
*/
#ifndef CALLBACKPROTO_H
#define CALLBACKPROTO_H

#include <string>

using CALLBACK_P = void (*)(const std::string &);

struct CallBackProto{
    virtual void acceptRESULT(const std::string & ){};
    virtual void acceptALARM(const std::string & ){};
};

#endif // CALLBACKPROTO_H
