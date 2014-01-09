-- Public command API, as exposed by the server.

local M = {} -- public module interface

-- current version
function M.version () return {0,1,0} end

-- trivial callback for smoke testing
function M.sum (a,b) return a+b end

-- upload a binary image to the LPCXpresso attached on the serial port
-- calls the "up2lpc"  wrapper script to avoid sudo troubles
function M.upload2lpc (data)
  -- store data in a temp file
  local tempname = os.tmpname()
  local fd1 = io.open(tempname, 'wb')
  fd1:write(data)
  fd1:close()
  -- run the uploader and collect its output
  local fd2 = io.popen('up2lpc '..tempname)
  local result = fd2:read('*a')
  fd2:close()
  -- clean up tempfile and return all output
  os.remove(tempname)
  return result;
end

return M