local m = require "glviewer"

function sphere( theta, phi )
    return { math.cos(theta)*math.sin(phi),
            math.sin(theta)*math.sin(phi),
            math.cos(phi) }
end


function add_tri( p0, p1, p2 )
    m.add_point( p0[1], p0[2], p0[3], p0[1], p0[2], p0[3] )
    m.add_point( p1[1], p1[2], p1[3], p1[1], p1[2], p1[3] )
    m.add_point( p2[1], p2[2], p2[3], p2[1], p2[2], p2[3] )
end

local n=100
local du = 2*math.pi/n
local dv = math.pi/n
for u=0, 2*math.pi, du do
    for v=0, math.pi, dv do
        local p0 = sphere( u, v )
        local p1 = sphere( u+du, v )
        local p3 = sphere( u+du, v+dv )
        local p2 = sphere( u, v+dv )
        add_tri( p0, p1, p3 )
        add_tri( p0, p3, p2 )
    end
end

m.show()
