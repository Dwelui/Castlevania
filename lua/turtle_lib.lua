local x, y, z = gps.locate()
local headers = {
	["id"] = tostring(os.getComputerID()),
	["position"] = x .. "," .. y .. "," .. z,
}

local vertical_blocks_data = textutils.serializeJSON({
	["up"] = select(2, turtle.inspectUp()),
	["front"] = select(2, turtle.inspect()),
	["down"] = select(2, turtle.inspectDown()),
}, true)
