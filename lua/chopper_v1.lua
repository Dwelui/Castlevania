local turtle_lib = require("turtle_lib")
local env = require("env")

local url = env.SERVER_DOMAIN .. env.CHOPPER_URL
local res, err = http.post(url, { blocks = turtle_lib.get_vertical_blocks_data() }, turtle_lib.get_headers())

if res then
	local data = textutils.unserializeJSON(res.readAll())

	local fn, err = load("return " .. data.action.name)
	if fn then
		local ok, result1, result2 = pcall(fn)

		-- if type(result1) == "table" then
		-- 	result1 = textutils.serializeJSON(result1)
		-- end
		--
		-- if type(result2) == "table" then
		-- 	result2 = textutils.serializeJSON(result2)
		-- end

		local response = textutils.serializeJSON({
            blocks = turtle_lib.get_vertical_blocks_data(),
			actionResult = {
				action = data.action.name,
				result = {
					result1,
					result2,
				},
			},
		})

		res, err = http.post(url, response, turtle_lib.get_headers())
	end
else
	print("ERROR: ", err)
end
