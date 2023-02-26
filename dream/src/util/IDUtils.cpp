/**********************************************************************************
 *  Dream is a software for developing real-time 3D experiences.
 *  Copyright (C) 2023 Deepak Ramalignam
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 **********************************************************************************/

#include "dream/util/IDUtils.h"
#include "dream/util/Logger.h"

#ifdef WIN32
#include <Rpc.h>
#elif __linux__
#include <uuid/uuid.h>
#elif __APPLE__

#include <uuid/uuid.h>

#endif

#include <filesystem>
#include <yaml-cpp/yaml.h>

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
        uuid_generate_random(uuid);
        char s[37];
        uuid_unparse(uuid, s);
        return s;
#else
        static bool printedGUIDWarning;
        if (!printedGUIDWarning) {
            Logger::warn("Hardware does not support GUID generation, defaulting to pseudorandom generation");
            printedGUIDWarning = true;
        }
        return uuid::generate_uuid_v4();
#endif
    }

    std::string IDUtils::newFileID(std::string uniqueIdentifier) {
        MD5 md5(uniqueIdentifier);
        return md5.hexdigest();
    }

    std::string IDUtils::getGUIDForFile(std::string filepath, bool crashOnFail) {
        std::string metaFilePath = filepath + ".meta";
        if (!std::filesystem::exists(metaFilePath)) {
            if (crashOnFail) {
                Logger::fatal("Cannot find meta file for " + metaFilePath);
            } else {
                Logger::warn("Cannot find meta file for " + metaFilePath);
                return "";
            }
        }
        YAML::Node doc = YAML::LoadFile(metaFilePath);
        return doc["guid"].as<std::string>();
    }
}
