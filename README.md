# Slippi Auth

## How does it work

Slippi-auth is a service that provides authentication using a slippi account. It works by faking a netplay player which the person wanting to authentify has to connect to in direct mode on slippi.

## Usage

```sh
go run cmd/slippi-auth/main.go
```

## Websocket API

> The websocket server runs on 0.0.0.0:9002

### Client messages

Queue an user:

```json
{
    "type": "queue",
    "code": "XXX#123",
    "timeout": 60000, // in ms
}
```

### Server messages

User has been authenticated:

```json
{
    "type": "success",
    "code": "XXX#123",
}
```

The authentication timed out:

```json
{
    "type": "timeout",
    "code": "XXX#123",
}
```

There was an error:

```json
{
    "type": "err",
}
```

There are no available authentication clients:

```json
{
    "type": "full",
}
```
