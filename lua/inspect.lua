local all_block_data = {
    ["up"] = turtle.inspectUp(),
    ["front"] = turtle.inspect(),
    ["down"] = turtle.inspectDown()
}

print(textutils.serializeJSON(all_block_data, true))
