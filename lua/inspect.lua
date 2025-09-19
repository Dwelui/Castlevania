local all_block_data = {
	["up"] = select(2, turtle.inspectUp()),
	["front"] = select(2, turtle.inspect()),
	["down"] = select(2, turtle.inspectDown()),
}

print(textutils.serializeJSON(all_block_data, true))
