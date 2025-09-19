local module = {}

function module.get_headers()
	local x, y, z = gps.locate()

	return {
		id = tostring(os.getComputerID()),
		position = x .. "," .. y .. "," .. z,
	}
end

function module.get_vertical_blocks_data()
	local result = {}

	local ok, data = turtle.inspectUp()
	if ok then
		result.up = data
	end

	ok, data = turtle.inspect()
	if ok then
		result.forward = data
	end

	ok, data = turtle.inspectDown()
	if ok then
		result.down = data
	end

	return textutils.serializeJSON(result, true)
end

return module
