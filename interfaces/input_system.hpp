#ifndef INPUT_SYSTEM_HPP
#define INPUT_SYSTEM_HPP

#include "../vec.hpp"
#include "../print.hpp"
#include <cmath>
#include <cstdint>

class Input {
public:
  static int view_angle_off;

  static int detect_va_off(void* self) {
    static const int cands[] = { 0x548, 0x688, 0x6C0, 0x6B0, 0x7B0, 0x812 };
    for (int off : cands) {
      Vec3 v = *(Vec3*)((uintptr_t)self + off);
      if (v.x >= -89.2f && v.x <= 89.2f && fabsf(v.z) < 0.02f &&
          fabsf(v.y) <= 361.f && std::isfinite(v.x) && std::isfinite(v.y))
        return off;
    }
    return 0x548; // a2x linux: +1352
  }

  void set_thirdperson(bool value) {
    *(bool*)(this + 0x261) = value;
  }

  bool is_thirdperson(void) {
    return *(bool*)(this + 0x261);
  }

  void set_view_angles(Vec3 angles) {
    if (view_angle_off < 0) view_angle_off = detect_va_off(this);
    *(Vec3*)((uintptr_t)this + view_angle_off) = angles;
  }

  Vec3 get_view_angles(void) {
    if (view_angle_off < 0) view_angle_off = detect_va_off(this);
    return *(Vec3*)((uintptr_t)this + view_angle_off);
  }

  void set_shoot(bool value) {
    *(bool*)(this + 0x288) = value;
  }
};

inline int Input::view_angle_off = -1;
inline static Input* input;

#endif
