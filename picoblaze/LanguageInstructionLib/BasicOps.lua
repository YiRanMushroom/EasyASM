function ProcessAdd(tokens) {
    if #tokens <= 2 then
        return "Error: Not enough arguments for 'add' command."
    end

    return InstructionBinary(8)
}

function ProcessSub(tokens) {
    if #tokens <= 2 then
        return "Error: Not enough arguments for 'sub' command."
    end

    return InstructionBinary(8)
}