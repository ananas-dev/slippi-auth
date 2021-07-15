# Slippi Authentication

## Build

```bash
mkdir build
cd build
cmake ..
make

# run with:
./SlippiAuth
```

## Websocket API

> The websocket server is located on localhost port 9002.

### Client messages

Queue an user:
```json
{
  "type": "queue",
  "discordId": 582645006100201485,
  "userCode":"XXX#123",
  "timeout": 10000
}
```

### Server messages

There was an error connecting to the Slippi servers:
```json
{
  "type": "slippiErr",
  "discordId": 582645006100201485,
  "userCode":"XXX#123"
}
```
The clients are all occupied:
```json
{
  "type": "noReadyClient",
  "discordId": 582645006100201485,
  "userCode": "XXX#123"
}
```

A client is searching for the user:
```json
{
  "type": "searching",
  "discordId": 582645006100201485,
  "botCode": "AUTH#123",
  "userCode": "XXX#123"
}
```

There was an error in the json:
```json
{ "type": "jsonErr" }
```

The command is unknown:
```json
{ "type": "unknownCommand" }
```

There is a missing argument:
```json
{ "type": "missingArg", "what": "code"}
```

A user got authenticated:
```json
{
  "type": "authenticated",
  "discordId": 582645006100201485,
  "userCode": "XXX#123"
}
```

A user got a timeout:
```json
{
  "type": "timeout",
  "discordId": 582645006100201485,
  "userCode": "XXX#123"
}
```
