local userIds = {}
local userNames = {}

local function addUser(id,name)
    userIds[name] = id
    userNames[id] = name
end

local function split(str, sep)
    local result = {}
    local i = 1
    local j = 1

    while i <= #str do
        if string.sub(str, i, i) == sep then
            table.insert(result, string.sub(str, j, i - 1))
            j = i + 1
            i = i + 1
        else
            i = i + 1
        end
    end

    if j <= #str then
        table.insert(result, string.sub(str, j, #str-1))
    end

    return result
end

local function printT(t)
    for k,v in pairs(t)
    do
        print(k .. ":" .. v)
    end
end


function parse(id,msg)
    local str = msg
    local elemnts = split(str,":")
    local type = elemnts[1]
    if (type == "login")
    then
        local data = elemnts[2]
        if (userIds[data] ~= nil)
        then
            chatserver:sendto(id,"user already exists")
        else
            addUser(id,data)
            chatserver:sendto(id,"login success")
            local cnt=0
            -- while (cnt <= 1000)
            -- do
            --     cnt = cnt + 1
            --     chatserver:sendto(id,"this is a big data send test.")
            -- end
        end
    elseif (type == "send")
    then
        local user = elemnts[2]
        local data = elemnts[3]
        local toid = userIds[user]
        if (toid == nil)
        then
            chatserver:sendto(id,"user not online")
            return
        end
        local message = userNames[id] .. "[private]:" .. data
        chatserver:sendto(toid,message)
    elseif (type == "broadcast")
    then
        local message = userNames[id] .. "[public]:" .. elemnts[2]
        chatserver:broadcast(message)
    elseif (type == "logout")
    then
        chatserver:Closeconnection(id)
    else
        -- print("unknown type" .. type .. "\r\n")
    end
end
