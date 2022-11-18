//
// Created by Deepak Ramalingam on 11/18/22.
//

#include "dream/util/IDUtils.h"

#ifdef WIN32
#include <Rpc.h>
#elif __linux__
#include <uuid/uuid.h>
#elif __APPLE__
#include <uuid/uuid.h>
#endif

namespace uuid {
    std::string generate_uuid_v4() {
        std::stringstream ss;
        int i;
        ss << std::hex;
        for (i = 0; i < 8; i++) {
            ss << dis(gen);
        }
        ss << "-";
        for (i = 0; i < 4; i++) {
            ss << dis(gen);
        }
        ss << "-4";
        for (i = 0; i < 3; i++) {
            ss << dis(gen);
        }
        ss << "-";
        ss << dis2(gen);
        for (i = 0; i < 3; i++) {
            ss << dis(gen);
        }
        ss << "-";
        for (i = 0; i < 12; i++) {
            ss << dis(gen);
        };
        return ss.str();
    }
}

namespace Dream {
    std::string IDUtils::newGUID() {
#ifdef WIN32
    UUID uuid;
    UuidCreate ( &uuid );

    unsigned char * str;
    UuidToStringA ( &uuid, &str );

    std::string s( ( char* ) str );

    RpcStringFreeA ( &str );
    return s;
#elif __linux__
    uuid_t uuid;
    uuid_generate_random ( uuid );
    char s[37];
    uuid_unparse ( uuid, s );
    return s;
#elif __APPLE__
    uuid_t uuid;
    uuid_generate_random ( uuid );
    char s[37];
    uuid_unparse ( uuid, s );
    return s;
#else
    static bool printedGUIDWarning;
    if (!printedGUIDWarning) {
        std::cout << "WARNING: Hardware does not support GUID generation, defaulting to pseudorandom generation" << std::endl;
        printedGUIDWarning = true;
    }
    return uuid::generate_uuid_v4();
#endif
    }

    std::string IDUtils::newFileID(std::string uniqueIdentifier) {
        MD5 md5(uniqueIdentifier);
        return md5.hexdigest();
    }
}
