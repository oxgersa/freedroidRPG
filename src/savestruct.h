
/**
 * \file savestruct.h
 * \brief Lua based save/load subsystem's definitions
 */
#include "savestruct_internal.h"

/// \defgroup genrw Auto-generated read/write of C struct
/// \ingroup luasaveload
///
/// Functions used to read and write the C structures defined in struct.h.
/// These functions are auto-generated by the gen_savestruct.py python script.

/*! \ingroup genrw */
void write_blast(struct auto_string *, blast *);
/*! \ingroup genrw */
void read_blast(lua_State *, int, blast *);
/*! \ingroup genrw */
void write_bullet(struct auto_string *, bullet *);
/*! \ingroup genrw */
void read_bullet(lua_State *, int, bullet *);
/*! \ingroup genrw */
void write_configuration_for_freedroid(struct auto_string *, configuration_for_freedroid *);
/*! \ingroup genrw */
void read_configuration_for_freedroid(lua_State *, int, configuration_for_freedroid *);
/*! \ingroup genrw */
void write_enemy(struct auto_string *, enemy *);
/*! \ingroup genrw */
void read_enemy(lua_State *, int, enemy *);
/*! \ingroup genrw */
void write_finepoint(struct auto_string *, finepoint *);
/*! \ingroup genrw */
void read_finepoint(lua_State *, int, finepoint *);
/*! \ingroup genrw */
void write_gps(struct auto_string *, gps *);
/*! \ingroup genrw */
void read_gps(lua_State *, int, gps *);
/*! \ingroup genrw */
void write_item(struct auto_string *, item *);
/*! \ingroup genrw */
void read_item(lua_State *, int, item *);
/*! \ingroup genrw */
void write_keybind_t(struct auto_string *, keybind_t *);
/*! \ingroup genrw */
void read_keybind_t(lua_State *, int, keybind_t *);
/*! \ingroup genrw */
void write_melee_shot(struct auto_string *, melee_shot *);
/*! \ingroup genrw */
void read_melee_shot(lua_State *, int, melee_shot *);
/*! \ingroup genrw */
void write_mission(struct auto_string *, mission *);
/*! \ingroup genrw */
void read_mission(lua_State *, int, mission *);
/*! \ingroup genrw */
void write_moderately_finepoint(struct auto_string *, moderately_finepoint *);
/*! \ingroup genrw */
void read_moderately_finepoint(lua_State *, int, moderately_finepoint *);
/*! \ingroup genrw */
void write_npc(struct auto_string *, npc *);
/*! \ingroup genrw */
void read_npc(lua_State *, int, npc *);
/*! \ingroup genrw */
void write_point(struct auto_string *, point *);
/*! \ingroup genrw */
void read_point(lua_State *, int, point *);
/*! \ingroup genrw */
void write_spell_active(struct auto_string *, spell_active *);
/*! \ingroup genrw */
void read_spell_active(lua_State *, int, spell_active *);
/*! \ingroup genrw */
void write_tux_t(struct auto_string *, tux_t *);
/*! \ingroup genrw */
void read_tux_t(lua_State *, int, tux_t *);
/*! \ingroup genrw */
void write_upgrade_socket(struct auto_string *, upgrade_socket *);
/*! \ingroup genrw */
void read_upgrade_socket(lua_State *, int, upgrade_socket *);
/*! \ingroup genrw */
void write_automap_data_t_array(struct auto_string *, automap_data_t *, int);
/*! \ingroup genrw */
void read_automap_data_t_array(lua_State *, int, automap_data_t *, int);
/*! \ingroup genrw */
void write_blast_array(struct auto_string *, blast *, int);
/*! \ingroup genrw */
void read_blast_array(lua_State *, int, blast *, int);
/*! \ingroup genrw */
void write_bullet_array(struct auto_string *, bullet *, int);
/*! \ingroup genrw */
void read_bullet_array(lua_State *, int, bullet *, int);
/*! \ingroup genrw */
void write_float_array(struct auto_string *, float *, int);
/*! \ingroup genrw */
void read_float_array(lua_State *, int, float *, int);
/*! \ingroup genrw */
void write_gps_array(struct auto_string *, gps *, int);
/*! \ingroup genrw */
void read_gps_array(lua_State *, int, gps *, int);
/*! \ingroup genrw */
void write_int32_t_array(struct auto_string *, int32_t *, int);
/*! \ingroup genrw */
void read_int32_t_array(lua_State *, int, int32_t *, int);
/*! \ingroup genrw */
void write_item_array(struct auto_string *, item *, int);
/*! \ingroup genrw */
void read_item_array(lua_State *, int, item *, int);
/*! \ingroup genrw */
void write_item_dynarray(struct auto_string *, item_dynarray *);
/*! \ingroup genrw */
void read_item_dynarray(lua_State *, int, item_dynarray *);
/*! \ingroup genrw */
void write_melee_shot_array(struct auto_string *, melee_shot *, int);
/*! \ingroup genrw */
void read_melee_shot_array(lua_State *, int, melee_shot *, int);
/*! \ingroup genrw */
void write_mission_array(struct auto_string *, mission *, int);
/*! \ingroup genrw */
void read_mission_array(lua_State *, int, mission *, int);
/*! \ingroup genrw */
void write_moderately_finepoint_array(struct auto_string *, moderately_finepoint *, int);
/*! \ingroup genrw */
void read_moderately_finepoint_array(lua_State *, int, moderately_finepoint *, int);
/*! \ingroup genrw */
void write_spell_active_array(struct auto_string *, spell_active *, int);
/*! \ingroup genrw */
void read_spell_active_array(lua_State *, int, spell_active *, int);
/*! \ingroup genrw */
void write_string_array(struct auto_string *, string *, int);
/*! \ingroup genrw */
void read_string_array(lua_State *, int, string *, int);
/*! \ingroup genrw */
void write_string_dynarray(struct auto_string *, string_dynarray *);
/*! \ingroup genrw */
void read_string_dynarray(lua_State *, int, string_dynarray *);
/*! \ingroup genrw */
void write_uint8_t_array(struct auto_string *, uint8_t *, int);
/*! \ingroup genrw */
void read_uint8_t_array(lua_State *, int, uint8_t *, int);
/*! \ingroup genrw */
void write_upgrade_socket_dynarray(struct auto_string *, upgrade_socket_dynarray *);
/*! \ingroup genrw */
void read_upgrade_socket_dynarray(lua_State *, int, upgrade_socket_dynarray *);
