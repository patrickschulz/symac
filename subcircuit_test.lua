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

function read_subcircuit_header(line, netlist)
    local name, rest = string.match(line, ".SUBCKT (%w+) (.+)$")
    netlist.subcircuits[name] = { nodes = {}, components = {} }
    for node in string.gmatch(rest, "(%w+)") do
        table.insert(netlist.subcircuits[name].nodes, node)
    end
    netlist.state = "read_in"
    netlist.currentsub = name
end

function read_subcircuit_body(line, netlist)
    if string.match(line, "%.ENDS") then
        netlist.state = "idle"
    else
        local typ, n1, n2, value = string.match(line, "(.) ([%w!]+) ([%w!]+) (%w+)")
        table.insert(netlist.subcircuits[netlist.currentsub].components, { typ = typ, nodes = { n1, n2 }, value = value })
    end
end

function instantiate_subcircuit(line, netlist)
    local subname, nodestring = string.match(line, "X(%w+)%s+(.*)$")
    netlist.subinstances[subname] = (netlist.subinstances[subname] and netlist.subinstances[subname] + 1) or 1
    local i = 1
    for node in string.gmatch(nodestring, "(%w+)") do
        netlist.submap[netlist.subcircuits[subname].nodes[i]] = node
        i = i + 1
    end
    local components = map_subcircuit_nodes(netlist, subname)
    for _, c in ipairs(components) do
        table.insert(netlist.components, c)
    end
end

function read_component(line, netlist)
    local typ, n1, n2, value = string.match(line, "(.) ([%w!]+) ([%w!]+) (%w+)")
    table.insert(netlist.components, { typ = typ, nodes = { n1, n2 }, value = value })
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
            read_subcircuit_header(line, netlist)
        elseif string.match(line, "X(%w+)") then
            instantiate_subcircuit(line, netlist)
        else
            read_component(line, netlist)
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
