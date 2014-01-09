#!/usr/bin/env lua

-- Simple command server, dispatches incoming requests as Lua function calls.
-- Call as "./server.lua" or "./server.lua -v" to enable debugging output.

require 'zmq'
require 'bencode'

-- configuration
local PORT = 'tcp://*:9389'
local VERBOSE = (arg[1] == "-v")

local cmds = require 'commands'
local context = zmq.init(1)
local socket = context:socket(zmq.REP)

-- wrap all processing so we can catch and return errors
local function process (msg)
  local request = bencode.decode(msg)
  if VERBOSE then print('', request[1]) end
  request[1] = cmds[request[1]]
  return unpack(request)
end

-- main server loop, never stops
print('server listening on '..PORT)
socket:bind(PORT)

while true do
  local message = socket:recv()
  local valid, result = pcall(process(message))
  if VERBOSE then
    if valid then print(result) else print(' ERROR:', result) end
  end
  socket:send(bencode.encode({valid and 0 or 1, result or ''}))
end
