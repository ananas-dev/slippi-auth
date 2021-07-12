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

### Phase 1, configure server:

Set the timeout:
```json
{ "type": "setTimeout", "seconds": "2000" }
```

### Phase 2, queue a user:

```json
{ "type": "queue", "code": "XXX#123" }
```

### Phase 3, get first response

There was an error connecting to the Slippi servers:
```json
{
  "type": "slippiErr",
  "id": 0,
  "code":"AUTH#123",
  "targetCode":"XXX#123"
}
```
The clients are all occupied:
```json
{
  "type": "noReadyClient",
  "targetCode":"XXX#123"
}
```

A client is searching for the user:
```json
{
  "type": "searching",
  "id": 0,
  "code": "AUTH#123",
  "targetCode": "XXX#123"
}
```

There was an error in the json:
```json
{ "type": "jsonErr" }
```

The command is unknown:
```json
{ "type": "unkwnownCommand" }
```

There is a missing argument:
```json
{ "type": "missingArg", arg: "code"}
```

### Phase 4, get the result of the authentication

A user got authenticated:
```json
{
  "type": "authenticated",
  "id": 0,
  "targetCode": "XXX#123"
}
```

A user got a timeout:
```json
{
  "type": "timeout",
  "id": 0,
  "targetCode": "XXX#123"
}
```
