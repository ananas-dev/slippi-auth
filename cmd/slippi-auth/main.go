package main

import (
	"encoding/json"
	"log"
	"os"
	"time"

	"github.com/ananas-dev/slippi-auth/pkg/worker"
	"github.com/codecat/go-enet"
)

func ParseConfig(path string) ([]worker.AuthClient, error) {
	data, err := os.ReadFile(path)

	if err != nil {
		return nil, err
	}

	client := []worker.AuthClient{}

	err = json.Unmarshal(data, &client)

	if err != nil {
		return nil, err
	}

	return client, nil
}

func main() {
	// Initialize enet
	enet.Initialize()
	defer enet.Deinitialize()

	log.Println("Started auth server")

	clients, err := ParseConfig("./clients.json")

	if err != nil {
		log.Fatal("Cannot parse clients.json")
	}

	pool := worker.WorkerPool{}

	pool.Start(clients)

	select {
	case pool.Jobs <- worker.Job{Code: "RULE#34", Timeout: 300000}:
		log.Println("New task queued")
	default:
		log.Println("Clients full")
	}

	for {
		time.Sleep(time.Second)
	}
}
