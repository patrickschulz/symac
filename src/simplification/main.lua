local simplification = require "simplification"
local util = require "util"

local pretty = require "pl.pretty"

local weightmap = {
    ["R"] = 0,
    ["C"] = 0,
    ["R1"] = 1,
    ["C1"] = 1,
    ["R2"] = 0,
    ["C2"] = 0,
    ["1"] = 0,
    ["gm"] = 1,
    ["rout"] = 1,
    ["gm1"] = 1,
    ["rout1"] = 1,
    ["gm2"] = 1,
    ["rout2"] = 1,
    ["Cl"] = 0,
    ["Cs"] = 0
}

local expressions = {
    { { "R1", "C1" }, { "R2", "C2"} },
    { { "1" }, { "gm", "rout" } },
    { { "R", "C1" }, { "gm", "rout", "R", "C2" } },
    { { "R", "C1" }, { "gm", "rout", "R", "C2" } },
    { { "gm1", "rout1" }, { "gm1", "rout1", "gm2", "rout2" } },
    { { "rout1", "Cl"}, { "rout1", "Cs" }, { "rout2", "Cl" }, { "gm2", "rout2", "rout1", "Cl" } },
}

for _, expression in ipairs(expressions) do
    local weights = simplification.calculate_weights(expression, weightmap)
    local tokeep = simplification.calculate_indices_to_keep(weights)
    local newexpression = simplification.create_new_expression(expression, tokeep)

    print(string.format("%s   ---->   %s", util.to_string(expression), util.to_string(newexpression)))
end
