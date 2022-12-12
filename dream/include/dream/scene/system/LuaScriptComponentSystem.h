//
// Created by Deepak Ramalingam on 11/26/22.
//

#ifndef DREAM_LUASCRIPTCOMPONENTSYSTEM_H
#define DREAM_LUASCRIPTCOMPONENTSYSTEM_H

#include <sol/sol.hpp>
#include <set>

namespace Dream {
    class LuaScriptComponentSystem {
    public:
        LuaScriptComponentSystem();
        ~LuaScriptComponentSystem();
        void update(float dt);
    private:
        sol::state lua;
        std::set<std::string> errorPrintedForScript;
    };
}


#endif //DREAM_LUASCRIPTCOMPONENTSYSTEM_H
