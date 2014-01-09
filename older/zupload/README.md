This is a Truly Simple (TM) client-server example based on ZeroMQ, to upload a
compiled binary image into an LXPxpresso connected via the /dev/ttyAMA0 serial
port on a Raspberry Pi, and using GPIO 18 and 23 to reset and enter ISP mode.
The actual work is done by a script called "up2lpc".

Principle of operation
----------------------

* server.lua is started once and runs forever, listening on a ZMQ "REP" port
* whenever a request comes in, it gets decoded and dispatched to commands.lua
* the result is sent back and the server then waits for the next request
* the demo client is "test.lua" and has a few trivial hard-coded requests
* zapi.lua wraps each request, sends it out, and then waits for the reply
* the reply is unwrapped and passed back to the caller, completing the RPC
* errors in the server are caught, wrapped up, and sent back to the client
* the client then re-throws these errors locally, same as if there were no RPC
* wrapping is done using the bencode.lua standard extension for now

The key trick here, is that zapi.lua can wrap any call. If we were to call:

    zapi.blah(123)

... then that call will be sent to the server, who has no idea what to do with
it, throws an error, the error comes back, and we as client end up with the
same type of failure as if the call had been processed locally (and failed).

The same goes for arguments: arguments are wrapped up, sent over, and then
unwrapped again to Lua data. The one important limitation is that only a few
datatypes are supported for all the arguments as well as for return values:

* strings (anything, including binary data)
* integers (no fractional part or floats, nor huge numbers)
* tables with only numeric indices 1..N, i.e. pure lists
* tables with string keys, i.e. associative arrays / dictionaries
* and any nested combination of these

Transparancy
------------

What this means in practice, is that this ZMQ-based mechanism is API agnostic.

To extend the API, just add a function on the server (and restart / reload it).
No adjustment is needed on the client side. Arguments and results are wrapped 
(serialized) automatically, as long as the above datatype restrictions are met.

In other words, if you add something like this to server.lua:

    function M.triple (v)
      return 3 * v
    end

... then, once the server has been restarted, all clients can call this code.
Defining an API is no harder (or easier) remotely with RPC, than locally. But
it'll work across the network and between all languages with a ZeroMQ binding.

Notes
-----

* the datatype limitation is severe, but guarantees full language independence
* note also that anything can be encoded as binary data, and sent as string
* routing is very crude: a fixed port, and a hardwired IP address in the code
* no recovery if the server were to crash (but all Lua errors do get caught)
* you need to know where each service like this is running, no registry
* no way to detect a stuck or dead server, so clients will wait forever
* a slow server (i.e. long running request) holds up all subsequent requests

All in all this is definitely not sufficient to build up the Tosca system, but
it may be useful anyway to get acquainted with ZeroMQ in Lua.
