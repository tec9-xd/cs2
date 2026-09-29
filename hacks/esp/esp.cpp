#include "../../imgui/dearimgui.hpp"
#include "../../math.hpp"
#include "../../gui/config.hpp"
#include "../../interfaces/game_entity_system.hpp"
#include "../../classes/pawn.hpp"

#include <cstdio>
#include <cmath>

static bool esp_looks_ptr(const void* p) {
  uintptr_t a = (uintptr_t)p;
  return a > 0x10000ull && a < 0x00007FFFFFFFFFFFull;
}

static bool bone_ok(const Vec3& b, const Vec3& origin) {
  if (b.x == 0.f && b.y == 0.f && b.z == 0.f) return false;
  float dx = b.x - origin.x, dy = b.y - origin.y, dz = b.z - origin.z;
  float d2 = dx * dx + dy * dy + dz * dz;
  return d2 > 1.f && d2 < 200.f * 200.f;
}

static void bone_line(ImDrawList* dl, Pawn* pawn, const Vec3& origin, int a, int b, ImU32 col) {
  Vec3 ba = pawn->get_bone_location((unsigned)a);
  Vec3 bb = pawn->get_bone_location((unsigned)b);
  if (!bone_ok(ba, origin) || !bone_ok(bb, origin)) return;
  Vec3 sa, sb;
  if (!world_to_screen(ba, &sa) || !world_to_screen(bb, &sb)) return;
  dl->AddLine(ImVec2(sa.x, sa.y), ImVec2(sb.x, sb.y), col, 1.5f);
}

void draw_players() {
  if (!config.esp.master) return;
  if (!entity_system || !esp_looks_ptr(entity_system) || !view_matrix) return;

  Entity* localentity = entity_system->get_localentity();
  Pawn* localpawn = entity_system->get_localpawn();
  if (!esp_looks_ptr(localentity) || !esp_looks_ptr(localpawn)) return;

  int local_team = (int)localpawn->get_cs_team();
  if (local_team != 2 && local_team != 3) return;

  ImDrawList* draw_list = ImGui::GetWindowDrawList();
  if (!draw_list) return;
  draw_list->Flags &= ~ImDrawListFlags_AntiAliasedLines;

  for (unsigned int i = 1; i <= 64; ++i) {
    Entity* entity = entity_system->entity_from_index(i);
    if (!esp_looks_ptr(entity) || entity == localentity) continue;

    int handle = entity->get_pawn_handle();
    if (handle == -1) continue;
    unsigned int idx = (unsigned int)(handle & 0x7FFF);
    if (idx == 0 || idx > 0x4000) continue;

    Pawn* pawn = entity_system->pawn_from_pawn_handle(handle);
    if (!esp_looks_ptr(pawn) || pawn == localpawn) continue;

    if (pawn->get_lifestate()) continue;
    if (pawn->is_dormant()) continue;

    int team = (int)pawn->get_cs_team();
    if (team != 2 && team != 3) continue;
    if (team == local_team) continue;

    int hp = pawn->get_health();
    if (hp <= 0 || hp > 200) continue;

    Vec3 origin = pawn->get_abs_origin();
    if (origin.x == 0.f && origin.y == 0.f && origin.z == 0.f) continue;

    Vec3 s_feet, s_head;
    Vec3 head_world = {origin.x, origin.y, origin.z + 72.f};
    if (!world_to_screen(origin, &s_feet)) continue;
    if (!world_to_screen(head_world, &s_head)) continue;

    float hgt = s_feet.y - s_head.y;
    if (hgt < 8.f || hgt > 800.f) continue;
    float wid = hgt * 0.45f;

    ImVec2 minv(s_head.x - wid * 0.5f, s_head.y);
    ImVec2 maxv(s_head.x + wid * 0.5f, s_feet.y);

    if (config.esp.player.box) {
      draw_list->AddRect(minv, maxv, IM_COL32(255, 50, 50, 255), 0, 0, 1.5f);
    }

    if (config.esp.player.health_bar) {
      float frac = ((hp > 100) ? 100 : hp) / 100.f;
      draw_list->AddRectFilled(
          ImVec2(minv.x - 5, maxv.y - hgt * frac),
          ImVec2(minv.x - 2, maxv.y),
          IM_COL32(0, 255, 0, 220));
    }

    if (config.esp.player.health_text) {
      char buf[8];
      snprintf(buf, sizeof(buf), "%d", hp);
      draw_list->AddText(ImVec2(minv.x - 8, minv.y - 14), IM_COL32(255, 255, 255, 255), buf);
    }

    if (config.esp.player.name) {
      const char* name = entity->get_name();
      if (esp_looks_ptr(name) && (unsigned char)name[0] >= 32 && (unsigned char)name[0] < 127)
        draw_list->AddText(ImVec2(minv.x, minv.y - 28), IM_COL32(255, 255, 255, 255), name);
    }

    if (config.esp.player.skeleton) {
      ImU32 col = IM_COL32(255, 255, 255, 220);
      bone_line(draw_list, pawn, origin, hip, spine1, col);
      bone_line(draw_list, pawn, origin, spine1, spine2, col);
      bone_line(draw_list, pawn, origin, spine2, spine3, col);
      bone_line(draw_list, pawn, origin, spine3, spine4, col);
      bone_line(draw_list, pawn, origin, spine4, neck, col);
      bone_line(draw_list, pawn, origin, neck, (int)Bone::head, col);

      bone_line(draw_list, pawn, origin, spine4, left_shoulder, col);
      bone_line(draw_list, pawn, origin, left_shoulder, left_elbow, col);
      bone_line(draw_list, pawn, origin, left_elbow, left_hand, col);

      bone_line(draw_list, pawn, origin, spine4, right_shoulder, col);
      bone_line(draw_list, pawn, origin, right_shoulder, right_elbow, col);
      bone_line(draw_list, pawn, origin, right_elbow, right_hand, col);

      bone_line(draw_list, pawn, origin, hip, left_hip, col);
      bone_line(draw_list, pawn, origin, left_hip, left_knee, col);
      bone_line(draw_list, pawn, origin, left_knee, left_foot, col);

      bone_line(draw_list, pawn, origin, hip, right_hip, col);
      bone_line(draw_list, pawn, origin, right_hip, right_knee, col);
      bone_line(draw_list, pawn, origin, right_knee, right_foot, col);
    }
  }
}
