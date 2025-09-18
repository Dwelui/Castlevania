local SERVER_DOMAIN = "http://192.168.32.8:8080"
local URL = "/api/turtle/chopper/v1"

local has_block, block_data = turtle.inspect()
local body

if has_block then
	body = textutils.serializeJSON(block_data, true)
else
	body = "No block"
end

local x, y, z = gps.locate()
local headers = {
	["id"] = tostring(os.getComputerID()),
	["position"] = x .. "," .. y .. "," .. z,
}

local res, err = http.post(SERVER_DOMAIN .. URL, body, headers)

if res then
	local data = textutils.unserializeJSON(res.readAll())

	for _, action in ipairs(data.actions) do
		local fn, err = load("return " .. action)
		if fn then
			local ok, result1, result2 = pcall(fn)

			if type(result2) == "table" then
				result2 = textutils.serializeJSON(result2, true)
			end
		end
	end
else
	print("Failed: ", err)
end
