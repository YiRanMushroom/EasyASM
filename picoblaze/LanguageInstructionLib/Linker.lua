function Linker(compiler)

    local linkerContext = compiler:GetLinkerContext()
    local linkRequestArray = linkerContext.LinkRequestArray
    local labelToAddressMap = linkerContext.LabelToAddressMap

--     print("Linker: Processing " .. #linkRequestArray .. " link requests.")

    for i = 1, #linkRequestArray do
        local request = linkRequestArray[i]
        local label = request.Label
        local start = request.Start

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
end