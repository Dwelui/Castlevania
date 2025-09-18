local SERVER_DOMAIN = "http://192.168.32.8:8080"
local URL = "/api/turtle/chopper/v1"

local has_block, block_data = turtle.inspect()
local body

if has_block then
	body = textutils.serializeJSON(block_data, true)
else
	body = "No block"
end

local url = SERVER_DOMAIN .. URL
local x, y, z = gps.locate()
local headers = {
	["computer-id"] = tostring(os.getComputerId()),
	["position"] = x .. "," .. y .. "," .. z,
}

local res, err = http.post(url, body, headers)

if res then
	local body = res.readAll()
	local data = textutils.unserializeJSON(body)

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
