#include "../math.hpp"
#include "../interfaces/input_system.hpp"
#include "../gui/config.hpp"
#include "../gui/menu.hpp"
#include "../print.hpp"

void aimbot(Vec3 original_view_angles);

bool (*input_create_move_original)(void*, int, bool);

bool input_create_move_hook(void* me, int slot, bool active) {
  bool ret = input_create_move_original(me, slot, active);
  if (input && config.aimbot.master && !menu_focused)
    aimbot(input->get_view_angles());
  return ret;
}
