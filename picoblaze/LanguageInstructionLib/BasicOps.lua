function ProcessAdd(tokens)
    for i = 1, #tokens do
        print(tokens[i]:GetContent())
    end

    if #tokens <= 2 then
        return "Error: Not enough arguments for 'add' command."
    end

    return InstructionBinary.new(8)
end

function ProcessSub(tokens)
    if #tokens <= 2 then
        return "Error: Not enough arguments for 'sub' command."
    end

    return InstructionBinary.new(8)
end