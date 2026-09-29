#ifndef PAWN_HPP
#define PAWN_HPP

#include <cstdint>
#include <limits.h>
#include <string>
#include <cmath>

#include "../vec.hpp"
#include "../print.hpp"

enum class cs_team;

enum Bone {
  hip = 0,
  spine1 = 1,
  spine2 = 2,
  spine3 = 3,
  spine4 = 4,
  neck = 5,
  head = 6,
  left_shoulder = 8,
  left_elbow = 9,
  left_hand = 10,
  right_shoulder = 13,
  right_elbow = 14,
  right_hand = 15,
  left_hip = 22,
  left_knee = 23,
  left_foot = 24,
  right_hip = 25,
  right_knee = 26,
  right_foot = 27,
};

class Pawn {
public:
  // C_BaseEntity::m_pGameSceneNode
  void* get_game_scene_node(void) {
    return *(void**)(this + 0x4A0);
  }

  // C_BasePlayerPawn::m_pCameraServices
  void* get_camera_services(void) {
    return *(void**)(this + 0x12B0);
  }

  // CCSPlayerBase_CameraServices::m_iFOV
  float get_fov(void) {
    void* camera_services = this->get_camera_services();
    if (camera_services == nullptr) {
      return 0.0;
    }
    return *(int*)((unsigned long)camera_services + 0x2A0);
  }

  // CGameSceneNode::m_vecAbsOrigin
  Vec3 get_abs_origin(void) {
    void* game_scene_node = this->get_game_scene_node();
    if (game_scene_node == nullptr) return Vec3{};
    return *(Vec3*)((unsigned long)game_scene_node + 0xC8);
  }

  // CGameSceneNode::m_bDormant
  bool is_dormant(void) {
    void* game_scene_node = this->get_game_scene_node();
    if (game_scene_node == nullptr) return true;
    return *(bool*)((unsigned long)game_scene_node + 0x103);
  }

  // CSkeletonInstance::m_modelState (0x140, your a2x) + CModelState bone ptr (0x80)
  Vec3 get_bone_location(unsigned int i) {
    if (i > 128) return Vec3{};
    void* game_scene_node = this->get_game_scene_node();
    if (!game_scene_node) return Vec3{};

    // CSkeletonInstance::m_modelState (your a2x = 0x140) + CModelState bone ptr (0x80)
    void* bone_data = *(void**)((unsigned long)game_scene_node + 0x140 + 0x80);
    if (!bone_data) return Vec3{};
    uintptr_t p = (uintptr_t)bone_data;
    if (p < 0x10000ull || p > 0x00007FFFFFFFFFFFull) return Vec3{};

    Vec3 pos = *(Vec3*)(p + (unsigned)i * 32);
    if (!std::isfinite(pos.x) || !std::isfinite(pos.y) || !std::isfinite(pos.z))
      return Vec3{};
    return pos;
  }

  // C_BaseModelEntity::m_vecViewOffset
  Vec3 get_eye_position(void) {
    Vec3 location = this->get_abs_origin();
    Vec3 offset = *(Vec3*)(this + 0xEE0);
    return Vec3{location.x + offset.x, location.y + offset.y, location.z + offset.z};
  }

  // not in either dump — RCS only; boxes ignore this
  Vec3 get_aim_punch(void) {
    return Vec3{};
  }

  // C_BaseEntity::m_lifeState — 0 = alive. callers treat true as dead.
  bool get_lifestate(void) {
    return *(uint8_t*)(this + 0x4C4) != 0;
  }

  // C_BaseEntity::m_iTeamNum — uint8, T=2 CT=3
  enum cs_team get_cs_team(void) {
    return (cs_team)*(uint8_t*)(this + 0x557);
  }

  // C_BaseEntity::m_iHealth
  int get_health(void) {
    return *(int*)(this + 0x4BC);
  }

  // C_CSPlayerPawn::m_bGunGameImmunity
  bool get_gun_game_immunity(void) {
    return *(bool*)(this + 0x43A8);
  }
};

#endif
