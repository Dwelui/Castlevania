local turtle_lib = require("turtle_lib")
local env = require("env")

local res, err = http.post(env.SERVER_DOMAIN .. env.CHOPPER_URL, turtle_lib.vertical_blocks_data, turtle_lib.headers)

if res then
	local data = textutils.unserializeJSON(res.readAll())

	print(data)
else
	print("ERROR: ", err)
end
