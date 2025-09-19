local up_block_exists, up_block_data = turtle.inspectUp()
local front_block_exists, front_block_data = turtle.inspect()
local down_block_exists, down_block_data = turtle.inspectDown()

local all_block_data = {
    ["up"] = up_block_data,
    ["front"] = front_block_data,
    ["down"] = down_block_data
}

print(all_block_data)
