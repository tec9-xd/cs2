#ifndef ENTITY_HPP
#define ENTITY_HPP

class Entity {
public:
  int get_pawn_handle(void) {
    return *(int*)(this + 0xAAC);       // m_hPlayerPawn
  }

  const char* get_name(void) {
    return (const char*)(this + 0x87C); // m_iszPlayerName
  }

  void set_fov(int fov) {
    *(int*)(this + 0x90C) = fov;
  }
};

#endif
