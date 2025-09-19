local turtle_lib = require("turtle_lib")
local env = require("env")

local url = env.SERVER_DOMAIN .. env.CHOPPER_URL
local body = textutils.serializeJSON({ blocks = turtle_lib.get_vertical_blocks_data() })
local res, err = http.post(url, body, turtle_lib.get_headers())

if res then
	local data = textutils.unserializeJSON(res.readAll())

	local fn, err = load("return " .. data.action.name)

	local actionResult = {
		action = data.action.name,
	}

	if fn then
		local ok, result1, result2 = pcall(fn)

		actionResult.result = {
			result1,
			result2,
		}
	end

	local response = textutils.serializeJSON({
		blocks = turtle_lib.get_vertical_blocks_data(),
		actionResult = actionResult,
	})

	res, err = http.post(url, response, turtle_lib.get_headers())
else
	print("ERROR: ", err)
end
