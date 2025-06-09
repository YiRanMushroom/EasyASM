function Linker(compiler)

    local linkerContext = compiler:GetLinkerContext()
    local LinkAddressRequestArray = linkerContext.LinkAddressRequestArray
    local labelToAddressMap = linkerContext.LabelToAddressMap

    for i = 1, #LinkAddressRequestArray do
        local request = LinkAddressRequestArray[i]
        local label = request.Label
        local start = request.Start
        local size = request.Size

        -- Find the address of the label
        local address = labelToAddressMap[label]
        if address == nil then
            return Exception.MakeLinkError(
                "Label '" .. label .. "' not found in the label to address map."
            )
        end

        -- Write the address to the instruction buffer at the specified start position
        compiler:ReplaceUnsignedNumber(
            address,
            10,
            start
        )
    end

    local linkConstantRequestArray = linkerContext.LinkConstantRequestArray
    local ConstantToValueMap = linkerContext.ConstantToValueMap

    for i = 1, #linkConstantRequestArray do
        local request = linkConstantRequestArray[i]
        local constantName = request.ConstantName
        local start = request.Start

        -- Find the value of the constant
        local constantValue = ConstantToValueMap[constantName]
        if constantValue == nil then
            return Exception.MakeLinkError(
                "Constant '" .. constantName .. "' not found in the constant to value map."
            )
        end

        -- Write the constant value to the instruction buffer at the specified start position
        compiler:ReplaceUnsignedNumber(
            constantValue,
            8,
            start
        )
    end
end