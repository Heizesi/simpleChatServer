function start()
    print("trying create server\r\n")
    chatserver = server("127.0.0.1",1299)
    print("Server started on port 1299\r\n")
    chatserver:start()
end

function close()
    chatserver:close()
    chatserver = nil
end