function OnBeforeCompile(compiler)
    compiler:GetLinkerContext().LinkRequestArray = {}
    compiler:GetLinkerContext().LabelToAddressMap = {}

end

function OnBeforeLink(compiler)
end

function OnAfterLink(compiler)
end