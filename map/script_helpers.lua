-- aliases for dialogs
next = set_next_node
show = enable_node
hide = disable_node
get_program = get_program_revision

-- gettext function for dialogs
function _(a)
    return a
end

-- compute the "town score" to determine whether the player can become
-- a red guard
function get_town_score()
    town_score = 0
    if done_quest("Bender's problem") then
	town_score = town_score + 10
    end

    if done_quest("The yellow toolkit") then
	town_score = town_score + 15
    end

    if done_quest("Anything but the army snacks, please!") then
	town_score = town_score + 10
    end

    if done_quest("Novice Arena") then
	town_score = town_score + 10
    end

    if done_quest("Time to say goodnight") then
	town_score = town_score + 20
    end

    if done_quest("Opening a can of bots...") then
	town_score = town_score + 15
    end

    return town_score
end

-- this table describes obstacle states
obstacle_states = {
	[6] = {["opened"] = 6, ["closed"] = 26,},
	[7] = {["opened"] = 6, ["closed"] = 26,},
	[8] = {["opened"] = 6, ["closed"] = 26,},
	[9] = {["opened"] = 6, ["closed"] = 26,},
	[10] = {["opened"] = 6, ["closed"] = 26,},

	[11] = {["opened"] = 11, ["closed"] = 27,},
	[12] = {["opened"] = 11, ["closed"] = 27,},
	[13] = {["opened"] = 11, ["closed"] = 27,},
	[14] = {["opened"] = 11, ["closed"] = 27,},
	[15] = {["opened"] = 11, ["closed"] = 27,},

	[26] = {["closed"] = 26, ["opened"] = 6,},
	[27] = {["closed"] = 27, ["opened"] = 11,},
	[32] = {["enabled"] = 32, ["disabled"] = 324,},
	[34] = {["enabled"] = 34, ["disabled"] = 326,},

	[181] = {["opened"] = 181, ["closed"] = 191,},
	[182] = {["opened"] = 181, ["closed"] = 191,},
	[183] = {["opened"] = 181, ["closed"] = 191,},
	[184] = {["opened"] = 181, ["closed"] = 191,},
	[185] = {["opened"] = 181, ["closed"] = 191,},

	[186] = {["opened"] = 186, ["closed"] = 192,},
	[187] = {["opened"] = 186, ["closed"] = 192,},
	[188] = {["opened"] = 186, ["closed"] = 192,},
	[189] = {["opened"] = 186, ["closed"] = 192,},
	[190] = {["opened"] = 186, ["closed"] = 192,},

	[191] = {["closed"] = 191, ["opened"] = 181,},
	[192] = {["closed"] = 192, ["opened"] = 186,},

	[235] = {["broken"] = 237,},
	[236] = {["broken"] = 237,},
	[237] = {["broken"] = 237,},
	[322] = {["broken"] = 237,},
	[324] = {["disabled"] = 324, ["enabled"] = 32,},
	[326] = {["disabled"] = 326, ["enabled"] = 34,},
	[351] = {["closed"] = 351, ["opened"] = 353,},
	[352] = {["closed"] = 352, ["opened"] = 358,},

	[353] = {["opened"] = 353, ["closed"] = 351,},
	[354] = {["opened"] = 353, ["closed"] = 351,},
	[355] = {["opened"] = 353, ["closed"] = 351,},
	[356] = {["opened"] = 353, ["closed"] = 351,},
	[357] = {["opened"] = 353, ["closed"] = 351,},

	[358] = {["opened"] = 358, ["closed"] = 352,},
	[359] = {["opened"] = 358, ["closed"] = 352,},
	[360] = {["opened"] = 358, ["closed"] = 352,},
	[361] = {["opened"] = 358, ["closed"] = 352,},
	[362] = {["opened"] = 358, ["closed"] = 352,},
};

function get_obstacle_state_id(id, state)
	a = rawget(obstacle_states, id);
	if (a == nil) then
		error("Obstacle number " .. id .. " does not have any states defined.", 2);
	end

	a = rawget(a, state)
	if (a == nil) then
		error("State \"" .. state .. "\" is not defined for obstacle type " .. id .. ".", 2);
	end

	return a
end

function change_obstacle_state(label, state)
	id = get_obstacle_type(label);

	change_obstacle_type(label, get_obstacle_state_id(id, state));
end

function cmp_obstacle_state(label, state)
	id = get_obstacle_type(label);

	return (id == get_obstacle_state_id(id, state));
end

-- Quest functions
function add_quest(quest, text)
	assign_quest(quest, text)
	cli_says("   ".._"New Quest assigned.","NO_WAIT")
	npc_says("")
end

function update_quest(quest, text)
	add_diary_entry(quest, text)
	cli_says("   ".._"Quest log updated.","NO_WAIT")
	npc_says("")
end

function end_quest(quest)
	complete_quest(quest)
	cli_says("   ".._"Quest completed.","NO_WAIT")
	npc_says("")
end

function npc_says(text,no_wait)
	chat_npc_says(apply_bbcode(text,"\3","\2"),no_wait)
end

function display_console_message(text)
	event_display_console_message(apply_bbcode(text,"\4","\5"))
end

-- Magic font-numbers table:
-- number  font face         default
-- '\1'    medium orange    tux_says()
-- '\2'    medium yellow    npc_says()
-- '\3'    medium blue      cli_cays()
-- '\4'    small blue
-- '\5'    small yellow   display_console_message()
function apply_bbcode(text,magic_num_b,magic_num_nrm)
	text = string.gsub(text,  '%[b%]', magic_num_b)
	text = string.gsub(text, '%[/b%]', magic_num_nrm)
	return text
end

function npc_says_random(...)
	if (arg[#arg] == "NO_WAIT") then
		npc_says(arg[math.random(#arg-1)],"NO_WAIT")
	else
		npc_says(arg[math.random(#arg)])
	end
end

function tux_says_random(...)
	if (arg[#arg] == "NO_WAIT") then
		tux_says(arg[math.random(#arg-1)],"NO_WAIT")
	else
		tux_says(arg[math.random(#arg)])
	end
end

function get_random(...)
	return arg[math.random(#arg)]
end

function del_gold(gold_amount)
	if (gold_amount <= get_gold()) then
		add_gold(-gold_amount)
		return true
	else
		return false
	end
end

function del_item(item_name) 
	if (has_item_backpack(item_name)) then
		del_item_backpack(item_name)
		return true
	else
		return false
	end
end

function del_points(num_points)
	if (get_training_points() >= num_points) then
		del_training_points(num_points)
		return true
	else
		return false
	end
end

function can_tux_train(gold_amount, num_points)
	if (get_gold() < gold_amount) then
		return false
	end
	
	if (get_training_points() < num_points) then
		return false
	end

	return true
end

function train_skill(gold_amount, num_points, skill)
	if (get_gold() < gold_amount) then
		return false
	end
	
	if (get_training_points() < num_points) then
		return false
	end

	add_gold(-gold_amount)
	del_training_points(num_points)
	improve_skill(skill)
	return true
end

function train_program(gold_amount, num_points, program)
	if (get_gold() < gold_amount) then
		return false
	end
	
	if (get_training_points() < num_points) then
		return false
	end

	add_gold(-gold_amount)
	del_training_points(num_points)
	improve_program(program)
	return true
end

function del_health(num_points)
	if (num_points < get_tux_hp()) then
		hurt_tux(num_points)
		return true
	else
		return false
	end
end

function difficulty()
	levels = {"easy", "normal", "hard"}
	return levels[difficulty_level()+1]
end

