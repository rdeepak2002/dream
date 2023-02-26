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

#ifndef DREAM_WINDOW_H
#define DREAM_WINDOW_H

#include <iostream>

namespace Dream {
    class Window {
    public:
        virtual void update(float dt);

        virtual void pollEvents(float dt);

        virtual void swapBuffers();

        [[nodiscard]] virtual bool shouldClose() const;

        virtual std::pair<int, int> getWindowDimensions();

        void setIsLoading(bool isLoading);

        virtual void setWindowBorderColor(double r, double g, double b);

    protected:
        Window();

        bool shouldCloseFlag;
        bool isLoading;
    };
}

#endif //DREAM_WINDOW_H
