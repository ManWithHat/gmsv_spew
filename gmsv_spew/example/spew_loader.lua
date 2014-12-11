require("spew")

--[[
	SPEW TYPES (these aren't strings)
	----------------
		SPEW_MESSAGE
		SPEW_WARNING
		SPEW_ASSERT
		SPEW_ERROR
		SPEW_LOG
	----------------

	Call engine.HookSpew(true|false) to enable or disable the hooking.
--]]

--[[
	This would hide all caught messages and log them to garrysmod/data/spew.txt
--]]
hook.Add("ShouldSpew", "ShouldSpew", function(message, spewType, color, level, group)
	file.Append("spew.txt", message)

	return false
end)