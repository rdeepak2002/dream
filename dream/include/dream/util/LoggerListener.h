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

#ifndef DREAM_LOGGERLISTENER_H
#define DREAM_LOGGERLISTENER_H

#include <iostream>

namespace Dream {
    class LoggerListener {
    public:
        virtual void debugLogPublished(std::string text) = 0;
        virtual void warnLogPublished(std::string text) = 0;
        virtual void errorLogPublished(std::string text) = 0;
        virtual void infoLogPublished(std::string text) = 0;
        virtual void fatalLogPublished(std::string text) = 0;
    };
}

#endif //DREAM_LOGGERLISTENER_H
