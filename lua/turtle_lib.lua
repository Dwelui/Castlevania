local module = {}

local x, y, z = gps.locate()
function module.get_headers()
	return {
		["id"] = tostring(os.getComputerID()),
		["position"] = x .. "," .. y .. "," .. z,
	}
end

function module.get_vertical_blocks_data()
	return textutils.serializeJSON({
		["up"] = select(2, turtle.inspectUp()),
		["front"] = select(2, turtle.inspect()),
		["down"] = select(2, turtle.inspectDown()),
	}, true)
end

return module
