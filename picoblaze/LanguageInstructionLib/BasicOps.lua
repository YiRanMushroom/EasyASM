function ProcessAdd(compiler)
    Exception.ThrowCompilerImplementationError("Test throw")
end

function ProcessSub(tokens)
    if #tokens <= 2 then
        return "Error: Not enough arguments for 'sub' command."
    end

    return InstructionBinary.new(8)
end