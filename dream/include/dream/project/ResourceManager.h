//
// Created by Deepak Ramalingam on 11/17/22.
//

#ifndef DREAM_RESOURCEMANAGER_H
#define DREAM_RESOURCEMANAGER_H

#include <iostream>
#include <unordered_map>
#include <map>

namespace Dream {
    class ResourceManager {
    private:
        /**
         * Map to find the path of a file from its GUID
         * Key: guid
         * Value: filePath
         */
        std::map<std::string, std::string> guidMap;
        /**
         * Map to find a fileID reference within a file
         * Key: <guid, fileID>
         * Value: data
         */
        std::map<std::pair<std::string, std::string>, void*> fileIDMap;
    public:
        /**
         * Get the path of a file given a GUID
         * @param guid the GUID of the file
         * @return path of the file from the root of the project directory
         */
        std::string getFilePathFromGUID(const std::string& guid);
        /**
         * Make a GUID refer to a filepath
         * @param guid the GUID of the file
         * @param filepath path of the file that can be referenced from teh GUID
         */
        void setFilePathFromGUID(const std::string& guid, const std::string& filepath);
        /**
         * Get data of an object from a part of a file
         * Example: Get data of submesh from an entire mesh file
         * @param guid the GUID of the file
         * @param fileID the ID of the sub part of the file that we want to get
         * @return data of the object
         */
        void* getData(const std::string& guid, const std::string& fileID="");
        /**
         * Store data for a part of a file and associate it with a GUID and fileID
         * Example: Store data for a submesh of an entire mesh
         * @param guid the GUID of the file
         * @param fileID the ID of teh sub part of the file that we want to store
         * @param data data to be stored
         */
        void storeData(const std::string& guid, const std::string& fileID, void* data);
        /**
         * Store data for an entire file and associate it with a GUID
         * Example: store data for an entire texture
         * @param giud
         * @param data
         */
        void storeData(const std::string& guid, void* data);
        bool hasData(const std::string& guid);
        bool hasData(const std::string& guid, const std::string& fileID);
    };
}

#endif //DREAM_RESOURCEMANAGER_H
