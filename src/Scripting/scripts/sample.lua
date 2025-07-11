-- sample.lua
-- Basic Lua script to test the scripting system

-- Define a table for the module
local Sample = {}

-- Initialize function
function Sample.init()
    print("Sample script initialized")
    return true
end

-- Update function that will be called each frame
function Sample.update(deltaTime)
    -- Just a placeholder function
    return deltaTime
end

-- Example function to calculate something
function Sample.calculate(a, b)
    return a + b
end

-- Return the module table
return Sample