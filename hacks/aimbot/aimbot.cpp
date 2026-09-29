#include "aimbot.hpp"
#include "../../gui/config.hpp"
#include "../../math.hpp"
#include "../../interfaces/input_system.hpp"
#include "../../interfaces/game_entity_system.hpp"
#include "../../classes/pawn.hpp"
#include "../../gui/menu.hpp"

bool shoot_next_tick = false;

static bool aim_looks_ptr(const void* p) {
  uintptr_t a = (uintptr_t)p;
  return a > 0x10000ull && a < 0x00007FFFFFFFFFFFull;
}

static Vec3 aim_point(Pawn* pawn) {
  Vec3 origin = pawn->get_abs_origin();
  Vec3 bone = pawn->get_bone_location(6);
  float dx = bone.x - origin.x, dy = bone.y - origin.y, dz = bone.z - origin.z;
  float d2 = dx * dx + dy * dy + dz * dz;
  if (d2 > 4.f && d2 < 120.f * 120.f && bone.z > origin.z)
    return bone;
  return Vec3{origin.x, origin.y, origin.z + 72.f};
}

void aimbot(Vec3 original_view_angles) {
  if (!config.aimbot.master || menu_focused) return;
  if (!entity_system || !aim_looks_ptr(entity_system) || !input) return;

  bool key = is_button_down(config.aimbot.key);
  if (!key && !config.aimbot.auto_shoot) {
    target_pawn = nullptr;
    return;
  }

  Pawn* localpawn = entity_system->get_localpawn();
  Entity* localentity = entity_system->get_localentity();
  if (!aim_looks_ptr(localpawn) || !aim_looks_ptr(localentity)) return;
  if (localpawn->get_lifestate()) return;

  int local_team = (int)localpawn->get_cs_team();
  if (local_team != 2 && local_team != 3) return;

  Vec3 eye = localpawn->get_eye_position();
  if (eye.x == 0.f && eye.y == 0.f && eye.z == 0.f)
    eye = localpawn->get_abs_origin();

  static bool once = false;
  if (!once) {
    print("aimbot live  va_off=0x%x  va=%.1f %.1f  team=%d\n",
          Input::view_angle_off, original_view_angles.x, original_view_angles.y, local_team);
    once = true;
  }

  Pawn* best = nullptr;
  float best_fov = config.aimbot.fov;
  Vec3 best_delta{};

  for (unsigned int i = 1; i <= 64; ++i) {
    Entity* entity = entity_system->entity_from_index(i);
    if (!aim_looks_ptr(entity) || entity == localentity) continue;
    int handle = entity->get_pawn_handle();
    if (handle == -1) continue;
    Pawn* pawn = entity_system->pawn_from_pawn_handle(handle);
    if (!aim_looks_ptr(pawn) || pawn == localpawn) continue;
    if (pawn->get_lifestate() || pawn->is_dormant()) continue;
    int team = (int)pawn->get_cs_team();
    if (team != 2 && team != 3 || team == local_team) continue;
    int hp = pawn->get_health();
    if (hp <= 0 || hp > 200) continue;

    Vec3 bone = aim_point(pawn);
    Vec3 diff{bone.x - eye.x, bone.y - eye.y, bone.z - eye.z};
    float yaw_hyp = sqrtf(diff.x * diff.x + diff.y * diff.y);
    float pitch = atan2f(diff.z, yaw_hyp) * radpi;
    float yaw   = atan2f(diff.y, diff.x) * radpi;
    Vec3 desired{-pitch, yaw, 0.f};

    float x = remainderf(desired.x - original_view_angles.x, 360.f);
    float y = remainderf(desired.y - original_view_angles.y, 360.f);
    if (x > 89.f) x = 89.f; else if (x < -89.f) x = -89.f;
    float fov = hypotf(x, y);
    if (fov < best_fov) {
      best_fov = fov;
      best = pawn;
      best_delta = {x, y, 0.f};
    }
  }

  target_pawn = best;
  if (!best) return;

  float smooth = config.aimbot.smooth;
  if (smooth < 1.f) smooth = 1.f;
  Vec3 final_ang{
    original_view_angles.x + best_delta.x / smooth,
    original_view_angles.y + best_delta.y / smooth,
    0.f
  };
  if (final_ang.x > 89.f) final_ang.x = 89.f;
  else if (final_ang.x < -89.f) final_ang.x = -89.f;
  final_ang.y = remainderf(final_ang.y, 360.f);

  input->set_view_angles(final_ang);
}
