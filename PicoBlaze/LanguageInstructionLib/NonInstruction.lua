function AddLabel(compiler, label)
    local linkerContext = compiler:GetLinkerContext()
    if linkerContext.LabelToAddressMap[label] ~= nil then
        return Exception.MakeCompileErrorWithLocation(
            compiler:GetTokenStream(),
            "Label '" .. label .. "' is already defined."
        )
    end

    linkerContext.LabelToAddressMap[label] = compiler:GetBitBufferSize() / 18
end

function ProcessNonInstruction(compiler)
    local tokenStream = compiler:GetTokenStream()
    local thisToken = tokenStream:ParseCurrent()
    if thisToken == nil then
        return Exception.MakeCompilerImplementationErrorWithLocation(
            tokenStream,
            "No token found in the token stream."
        )
    end
    local nextToken = tokenStream:PeekCurrent()
    if nextToken == nil then
        return Exception.MakeCompilerImplementationErrorWithLocation(
            tokenStream,
            "No next token found in the token stream."
        )
    end

    if nextToken ~= ":" then
        return Exception.MakeCompileErrorWithLocation(
            tokenStream,
            "Expected ':' after '" .. thisToken .. "'. This may be a label missing a colon, or the token was mistakenly interpreted as an instruction or directive-possibly due to a spelling error or because the user assumed it exists in the instruction/directive set-but it is not recognized."
        )
    end

    tokenStream:SkipCurrent() -- ":"

    tokenStream:SetNewLine(true)

    return AddLabel(compiler, thisToken)
end