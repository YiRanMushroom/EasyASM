function ProcessNonInstruction(compiler)
    local NotNewLineException = compiler:GetTokenStream():AssertIsNewLine()
    if NotNewLineException then
        return NotNewLineException
    end
    print("Non-instruction encountered in the code: " .. compiler:GetTokenStream():ParseCurrent())
end