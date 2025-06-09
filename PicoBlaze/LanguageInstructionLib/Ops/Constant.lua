function ParseConstant(compiler)
    local tokenStream = compiler:GetTokenStream()
    local thisToken = tokenStream:ParseCurrent()
    local linkerContext = compiler:GetLinkerContext()
    local constantTable = linkerContext.ConstantToValueMap

    if thisToken == nil then
        return Exception.MakeCompileErrorWithLocation(
            tokenStream,
            "Expected a constant name."
        )
    end

    if constantTable[thisToken] ~= nil then
        return Exception.MakeCompileErrorWithLocation(
            tokenStream,
            "Constant '" .. thisToken .. "' is already defined."
        )
    end

    local testValues = Lib.ParseRegister(compiler, thisToken)
    if testValues ~= nil then
        return Exception.MakeCompileErrorWithLocation(
            tokenStream,
            "Constant '" .. thisToken .. "' is already defined as a register."
        )
    end
    testValues = Lib.ParseSimpleUnsigned(tokenStream, thisToken)
    if testValues ~= nil and testValues >= 0 and testValues <= 255 then
        return Exception.MakeCompileErrorWithLocation(
            tokenStream,
            "Value '" .. thisToken .. "' cannot be used as a constant because it is an unsigned immediate value."
        )
    end

    local constantName = thisToken
    thisToken = tokenStream:ParseCurrent()

    if thisToken == nil or thisToken ~= "," then
        return Exception.MakeCompileErrorWithLocation(
            tokenStream,
            "Expected a ',' after constant name '" .. constantName .. "'."
        )
    end

    thisToken = tokenStream:ParseCurrent()

    local constantValue = Lib.ParseSimpleUnsigned(compiler, thisToken)
    if constantValue == nil or constantValue < 0 or constantValue > 255 then
        return Exception.MakeCompileErrorWithLocation(
            tokenStream,
            "Invalid constant value: '" .. thisToken .. "'. Expected a value between 0 and 255."
        )
    end

    constantTable[constantName] = constantValue
end