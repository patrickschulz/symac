local netlistlines = {
".SUBCKT sub n1 n2 n3",
    "R n1 localnet R0",
    "R n3 0! R1",
    "C n2 n3 C0",
".ENDS",
"Xsub vin vout vref",
"Xsub vin vff vzero",
"L vin vout L0"
}

function map_subcircuit_nodes(netlist, subname)
    local subcircuit = netlist.subcircuits[subname]
    local components = {}
    for i, component in ipairs(subcircuit.components) do
        local nodes = component.nodes
        components[i] = {
            typ = component.typ,
            nodes = {},
            value = component.value
        }
        for _, node in ipairs(nodes) do
            local mapped_node = node
            if not string.match(node, ".*%!$") then
                mapped_node = netlist.submap[node]
                if not mapped_node then -- local net
                    mapped_node = string.format("%s:%d", node, netlist.subinstances[subname])
                end
            end
            table.insert(components[i].nodes, mapped_node)
        end
    end
    return components
end

function read_subcircuit_header(line)
    -- parse subcircuit line
    local name, rest = string.match(line, ".SUBCKT (%w+) (.+)$")
    local subcircuit = { nodes = {}, components = {} }
    -- read in all nodes
    for node in string.gmatch(rest, "(%w+)") do
        table.insert(subcircuit.nodes, node)
    end
    return subcircuit, name
end

function read_subcircuit_body(line, netlist)
    -- either the line is a component or the end of the subcircuit definition
    if string.match(line, "%.ENDS") then
        netlist.state = "idle"
    else
        -- read component
        local component = read_component(line)
        table.insert(netlist.currentsub.components, component)
    end
end

function translate_subcircuit(line, netlist)
    local subname, nodestring = string.match(line, "X(%w+)%s+(.*)$")
    netlist.subinstances[subname] = (netlist.subinstances[subname] and netlist.subinstances[subname] + 1) or 1
    local i = 1
    for node in string.gmatch(nodestring, "(%w+)") do
        netlist.submap[netlist.subcircuits[subname].nodes[i]] = node
        i = i + 1
    end
    local components = map_subcircuit_nodes(netlist, subname)
    return components
end

function read_component(line)
    local typ, n1, n2, value = string.match(line, "(.) ([%w!]+) ([%w!]+) (%w+)")
    local component = { typ = typ, nodes = { n1, n2 }, value = value }
    return component
end

-- read netlist
local netlist = {
    state = "idle",
    currentsub = nil,
    subcircuits = {},
    submap = {},
    subinstances = {},
    components = {}
}

for _, line in ipairs(netlistlines) do
    if netlist.state == "idle" then
        if string.match(line, "%.SUBCKT") then
            local subcircuit, name = read_subcircuit_header(line)
            -- store subcircuit
            netlist.subcircuits[name] = subcircuit
            -- now the netlist parser is in read mode (subcircuits)
            netlist.state = "read_in"
            netlist.currentsub = subcircuit
        elseif string.match(line, "X(%w+)") then
            local components = translate_subcircuit(line, netlist)
            for _, c in ipairs(components) do
                table.insert(netlist.components, c)
            end
        else
            local component = read_component(line, netlist)
            table.insert(netlist.components, component)
        end
    elseif netlist.state == "read_in" then
        read_subcircuit_body(line, netlist)
    end
end

print()
for _, component in ipairs(netlist.components) do
    print(string.format("%s %s %s", component.typ, table.concat(component.nodes, " "), component.value))
end
print()
