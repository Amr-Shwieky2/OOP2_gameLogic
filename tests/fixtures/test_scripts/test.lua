-- test.lua
-- Test script for unit tests

-- Define a table for the test module
local TestModule = {}

-- Initialize function
function TestModule.init()
    return true
end

-- Test function that returns a number
function TestModule.getValue()
    return 42
end

-- Test function that takes parameters
function TestModule.add(a, b)
    return a + b
end

-- Test function that returns multiple values
function TestModule.getStats()
    return 100, 200, "test"
end

-- Test function for error handling
function TestModule.generateError()
    error("Test error message")
end

-- Return the module
return TestModule