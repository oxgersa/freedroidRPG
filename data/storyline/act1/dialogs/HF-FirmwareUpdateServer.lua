---------------------------------------------------------------------
-- This file is part of Freedroid
--
-- Freedroid is free software; you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation; either version 2 of the License, or
-- (at your option) any later version.
--
-- Freedroid is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with Freedroid; see the file COPYING. If not, write to the
-- Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
-- MA 02111-1307 USA
----------------------------------------------------------------------

local Npc = FDrpg.get_npc()
local Tux = FDrpg.get_tux()

return {
	EveryTime = function()
		if (not HF_FirmwareUpdateServer_uploaded_faulty_firmware_update) then
			play_sound("effects/Menu_Item_Deselected_Sound_0.ogg")
			-- Are we human? CAPTCHA!!!
			number_one=math.random(2,7)
			number_two=math.random(1,number_one-1)
			captcha = number_one - number_two
			if (captcha == 1) then
				captcha = _"one"
			elseif (captcha == 2) then
				captcha = _"two"
			elseif (captcha == 3) then
				captcha = _"three"
			elseif (captcha == 4) then
				captcha = _"four"
			elseif (captcha == 5) then
				captcha = _"five"
			elseif (captcha == 6) then
				captcha = _"six"
			end
			response = user_input_string(string.format(_"CAPTCHA: Please write the lowercase word that answers the following: %d - %d = ?", number_one, number_two))
			if (captcha ~= response) then
				Npc:says(_"Non-human detected. Administering paralyzing shock.")
				Npc:says(_"NOTE: If you are a human, try again, and make sure you enter a word and not digits.")
				freeze_tux_npc(7)
				Tux:hurt(20)
				Tux:heat(20)
				play_sound("effects/Menu_Item_Selected_Sound_1.ogg")
				end_dialog()
			else
				Npc:says(_"Welcome to MS firmware update server for region #54658.")
				Npc:says(_"Please select action")
				show("node1", "node3", "node99")
			end
		end
	end,

	{
		id = "node1",
		text = _"Get current OS version",
		code = function()
			if (not HF_FirmwareUpdateServer_uploaded_faulty_firmware_update) then
				Npc:says(_"Currently deployed firmware is v. 5.0.8+worldto-nogpl.")
			else
			--; TRANSLATORS: %s in %swashere  = Tux:get_player_name() 
				Npc:says(_"Currently deployed firmware is v. 5.0.8+%swashere+gpl", Tux:get_player_name())
				if (not HF_FirmwareUpdateServer_Spencer) then
					--; TRANSLATORS: "hello?"
					Npc:says(_"*Bzzt*ello?")
					Tux:says(_"What? What is this?")
					start_chat("Spencer")
					Tux:update_quest("Propagating a faulty firmware update", _"Spencer somehow contacted me through the firmware server terminal. From what he told me, it worked - the hostile bots are all deactivated! I am now supposed to stand by for further instructions...")
					HF_FirmwareUpdateServer_Spencer = true
				end
			end
		end,
	},
	{
		id = "node3",
		text = _"Propagate firmware update",
		code = function()
			Npc:says(_"Compressing image....")
			Npc:says(_"Advertising update to clients...")
			Npc:says(_"Waiting for all connections....")
			Npc:says(_"...")
			Npc:says(_"...")
			Npc:says(_"Please resolve merge conflicts:")
			if (takeover(get_program("Hacking")+4)) then
				Npc:says(_"Conflicts resolved,")
				Npc:says(_"Firmware update propagated!")
				Npc:says(_"ALL: SECURITY: could not find 'evil.conf': No such file or directory.", "NO_WAIT")
				Npc:says(_"	Exiting with state 1.")
				HF_FirmwareUpdateServer_uploaded_faulty_firmware_update = true
				change_obstacle_state("ServerRoomDoor", "closed")
				Tux:update_quest("Propagating a faulty firmware update", _"I managed to upload the faulty firmware update to all bots within transmission range. Either the siege on the town ends now... Or everything else does.")
				kill_faction("ms", "no_respawn")
				hide("node3")
			else
				Npc:says(_"Merge failed.")
				Npc:says(_"Exiting...")
				play_sound("effects/Menu_Item_Selected_Sound_1.ogg")
				end_dialog()
			end
		end,
	},
	{
		id = "node99",
		text = _"Logout",
		code = function()
			play_sound("effects/Menu_Item_Selected_Sound_1.ogg")
			end_dialog()
			if (HF_FirmwareUpdateServer_Spencer) and (not HF_Spencer_teleported) then
				local guard = FDrpg.get_npc("Town-TuxGuard")
				change_obstacle_type("59-Teleporter", 19) --Prime the teleporter!
				add_obstacle(59, 57.6, 29.5, 16)
				-- We would like the teleportation to be a little delayed for effect.
				play_sound("effects/new_teleporter_sound.ogg")
				Spencer:teleport("59-Teleporter-1")
				Spencer:set_destination("59-Teleporter-1")
				delay_game(2.5)
				play_sound("effects/new_teleporter_sound.ogg")
				Bender:teleport("59-Teleporter-1")
				Bender:set_destination("59-Teleporter-2", "Bender")
				delay_game(2.5)
				play_sound("effects/new_teleporter_sound.ogg")
				guard:teleport("59-Teleporter-1", "Town-TuxGuard")
				guard:set_destination("59-Teleporter-3")
				HF_Spencer_teleported = true
			end
		end,
	},
}
