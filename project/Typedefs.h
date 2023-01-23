#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include "ParsedVerseLocation.h"
#include "VerseLocation.h"

namespace qbv {
    typedef qbv::VerseLocation (*MyFunc)(qbv::ParsedVerseLocation);
}


#endif // TYPEDEFS_H
