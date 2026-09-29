#ifndef GAME_ENTITY_SYSTEM_HPP
#define GAME_ENTITY_SYSTEM_HPP

#include <cstdint>
#include "../classes/entity.hpp"

class Pawn;

const unsigned long entity_identity_size = 0x70;

static Entity** localentity_ptr = nullptr;

static inline bool ges_ptr(const void* p) {
  uintptr_t a = (uintptr_t)p;
  return a > 0x10000ull && a < 0x00007FFFFFFFFFFFull;
}

class GameEntitySystem {
public:
  Entity* entity_from_index(unsigned int i) {
    if (i > 0x4000) return nullptr;
    void** bucket_slot = (void**)((uintptr_t)this + 0x10 + 0x8 * (i >> 9));
    if (!ges_ptr(bucket_slot)) return nullptr;
    void* bucket_ptr = *bucket_slot;
    if (!ges_ptr(bucket_ptr)) return nullptr;
    Entity* e = *(Entity**)((uintptr_t)bucket_ptr + entity_identity_size * (i & 0x1FF));
    if (!ges_ptr(e)) return nullptr;
    return e;
  }

  Pawn* pawn_from_pawn_handle(int handle) {
    unsigned int idx = (unsigned int)(handle & 0x7FFF);
    if (idx == 0 || idx > 0x4000) return nullptr;
    return (Pawn*)entity_from_index(idx);
  }

  Pawn* pawn_from_index(unsigned int i) {
    Entity* entity = this->entity_from_index(i);
    if (!entity) return nullptr;
    return pawn_from_pawn_handle(entity->get_pawn_handle());
  }

  Entity* get_localentity(void) {
    if (!ges_ptr(localentity_ptr)) return nullptr;
    Entity* e = *localentity_ptr;
    if (!ges_ptr(e)) return nullptr;
    return e;
  }

  Pawn* get_localpawn(void) {
    Entity* localentity = this->get_localentity();
    if (!localentity) return nullptr;
    return this->pawn_from_pawn_handle(localentity->get_pawn_handle());
  }
};

inline static GameEntitySystem* entity_system;

#endif
