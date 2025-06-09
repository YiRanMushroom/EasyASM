function ProcessAdd(compiler)
    return Exception.MakeCompilerImplementationError(
        "Add operation is not supported in this version of the compiler."
    )
end

function ProcessSub(tokens)
    if #tokens <= 2 then
        return "Error: Not enough arguments for 'sub' command."
    end

    return InstructionBinary.new(8)
end