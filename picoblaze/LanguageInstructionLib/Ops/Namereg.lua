function ParseNameReg(compiler)
    local compilerContext = compiler:GetCompilerContext()
    local regNameArray = compilerContext.RegNameArray
    local tokenStream = compiler:GetTokenStream()

    local thisToken = tokenStream:ParseCurrent()

    local regNum = Lib.ParseSimpleRegister(tokenStream, thisToken)


    if regNum == nil then
        for i = 0, 15 do
            if regNameArray[i] == thisToken then
                regNum = i
                break
            end
        end
    end

    if regNum == nil then
        return Exception.MakeCompileErrorWithLocation(
            tokenStream,
            "Invalid register name: '" .. thisToken .. "'. Expected 's0' to 's15' or a valid register name."
        )
    end

    if regNum < 0 or regNum > 15 then
        return Exception.MakeCompileErrorWithLocation(
            tokenStream,
            "Register number out of range: " .. tostring(regNum) .. ". Expected a value between 0 and 15."
        )
    end

    thisToken = tokenStream:ParseCurrent()
    if thisToken == nil then
        return Exception.MakeCompileErrorWithLocation(tokenStream, "No token found in the token stream.")
    end

    if thisToken ~= "," then
        return Exception.MakeCompileErrorWithLocation(
            tokenStream,
            "Expected ',' after register name, but found '" .. thisToken .. "' instead."
        )
    end


    thisToken = tokenStream:ParseCurrent()

    if Lib.ParseSimpleRegister(tokenStream, thisToken) == nil then
        for i = 0, 15 do
            if regNameArray[i] == thisToken then
                return Exception.MakeCompileErrorWithLocation(
                    tokenStream,
                    "Register name '" .. thisToken .. "' is already defined as alias for register s" .. tostring(i)
                )
            end
        end

        local couldBeANumber = Lib.ParseSimpleUnsigned(tokenStream, thisToken)

        if couldBeANumber~=nil and couldBeANumber >= 0 and couldBeANumber <= 255 then
            return Exception.MakeCompileErrorWithLocation(
                tokenStream,
                "Register name '" .. thisToken .. "' cannot be a number. Register names must not be confused with immediate values."
            )
        end

        regNameArray[regNum] = thisToken
    else
        return Exception.MakeCompileErrorWithLocation(
            tokenStream,
            "Invalid register name: '" .. thisToken .. "'. New name of register cannot be confused with a build in register name."
        )
    end

end