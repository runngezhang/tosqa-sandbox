-- Client interface code to push RPC calls over ZeroMQ using bencoded strings.

-- configuration
-- local PORT = 'tcp://127.0.0.1:9389'
local PORT = 'tcp://192.168.1.127:9389'
--local PORT = 'tcp://10.0.0.97:9389'
local DEBUG = false

require 'zmq'
require 'bencode'

if DEBUG then require 'serialization' end

local M = {} -- public module interface

function M.apiVers () return {0,1} end

function M.apiDebug (flag) DEBUG = flag end

-- connect to the server/dispatcher via its known port
local context = zmq.init(1)
M.socket = context:socket(zmq.REQ)
M.socket:connect(PORT)

-- set up special behavior to capture all module accesses as function calls
-- this allows proxying "zapi:abc(...)" with arbitrary "abc" names and args
local mt = {}
setmetatable(M, mt)

-- translate a "zapi:abc(1,2,3)" call into a {'abc',1,2,3} RPC request
function mt:__index(name, ...)
  return function (context, ...)
    local s = context.socket
    -- if DEBUG then print(serialization.serialize({name, ...})) end
    s:send(bencode.encode({name, ...})) -- send out as bencoded request
    local reply = bencode.decode(s:recv()) -- and wait for the bencoded reply
    if reply[1] == 0 then return reply[2] end
    return false, reply[2] -- use standard Lua convention for returning errors
  end
end

return M
