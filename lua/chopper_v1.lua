local turtle_lib = require("turtle_lib")
local env = require("env")

local url = env.SERVER_DOMAIN .. env.CHOPPER_URL
local res, err = http.post(url, turtle_lib.get_vertical_blocks_data(), turtle_lib.get_headers())

if res then
	local data = textutils.unserializeJSON(res.readAll())

	print(data)
else
	print("ERROR: ", err)
end
