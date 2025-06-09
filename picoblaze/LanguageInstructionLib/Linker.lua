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
end