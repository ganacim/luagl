local m = require "glviewer"
local t = {
    { { 1, 0, 0 }, { 1, 0, 0 } },
    { { 0, 1, 0 }, { 0, 1, 0 } },
    { { 0, 0, 1 }, { 0, 0, 1 } },
}
for k,v in ipairs(t) do
    local p = v[1]
    local n = v[2]
    m.add_triangle( p[1], p[2], p[3], n[1], n[2], n[3] )
end
m.show()
