function ProcessNonInstruction(compiler)
    print("Non-instruction encountered in the code: " .. compiler:GetTokenStream():ParseCurrent())
end