#!/usr/bin/env lua

-- Run a very simple test to issue some requests to server.lua via ZeroMQ.
-- Call as "./test.lua blink1.bin" or "./test.lua blink2.bin".

local zapi = require 'zapi'

print(1, table.concat(zapi:version(),'.'))
print(2, zapi:sum(1,2))

local fd = assert(io.open(arg[1], 'rb'))
local data = fd:read('*a')
fd:close()

print(3, zapi:upload2lpc(data))
