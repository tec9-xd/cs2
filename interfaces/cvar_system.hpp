#ifndef CVAR_SYSTEM_HPP
#define CVAR_SYSTEM_HPP

#include <string.h>
#include <cstdint>
#include "../classes/convar.hpp"

class CvarSystem {
public:
  Convar* get_convar(const char* convar_name) {
    if (!convar_name) return nullptr;

    uintptr_t self = (uintptr_t)this;
    if (self < 0x10000ull) return nullptr;

    uintptr_t objects = *(uintptr_t*)(self + 0x48);
    uintptr_t length  = *(uintptr_t*)(self + 0xA0);
    if (objects < 0x10000ull || length == 0 || length > 8192) return nullptr;

    for (uintptr_t i = 0; i < length; ++i) {
      uintptr_t object = *(uintptr_t*)(objects + i * 0x10);
      if (object < 0x10000ull) continue;
      const char* name = *(const char**)object;
      if ((uintptr_t)name < 0x10000ull) continue;
      if (strstr(name, convar_name)) return (Convar*)object;
    }
    return nullptr;
  }
};

inline static CvarSystem* cvar_system;

#endif
