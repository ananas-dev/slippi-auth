package worker

import (
	"encoding/json"
	"errors"
	"fmt"
	"log"
	"net"
	"strconv"

	"github.com/ananas-dev/slippi-auth/pkg/slippi"
	"github.com/codecat/go-enet"
)

const SlippiHost = "mm.slippi.gg"
const SlippiPort = 43113

func connect(host *enet.Host, maxAttemps int) error {
	for i := 0; i < maxAttemps; i++ {
		ev := (*host).Service(500)

		if ev.GetType() == enet.EventConnect {
			return nil
		}
	}

	return errors.New("max attemps exceded")
}

func disconnect(host *enet.Host, peer *enet.Peer) {
	(*peer).Disconnect(0)
	(*host).Destroy()
}

func send(peer *enet.Peer, msg []byte) error {
	err := (*peer).SendBytes(msg, 0, enet.PacketFlagReliable)

	if err != nil {
		fmt.Println(err)
		return errors.New("could not send message to peer")
	}

	return nil
}

func rcv(host *enet.Host, response interface{}, timeoutMs int) error {
	hostServiceTimeoutMs := 250
	if timeoutMs < hostServiceTimeoutMs {
		timeoutMs = hostServiceTimeoutMs
	}

	maxAttemps := timeoutMs / hostServiceTimeoutMs

	for i := 0; i < maxAttemps; i++ {
		ev := (*host).Service(uint32(hostServiceTimeoutMs))

		if ev.GetType() == enet.EventNone {
			continue
		}

		switch ev.GetType() {
		case enet.EventReceive:
			packet := ev.GetPacket()

			defer packet.Destroy()

			packetBytes := packet.GetData()

			err := json.Unmarshal(packetBytes, response)

			if err != nil {
				return errors.New("could not parse json response")
			}

			return nil

		case enet.EventDisconnect:
			return errors.New("lost connection to the server")
		default:
			continue
		}
	}

	return errors.New("timeout exceded")
}

func SlippiWorker(id int, uid string, key string, jobs <-chan Job, results chan<- Result) {
	for j := range jobs {
		log.Println(j)

		hostPort := uint16(41000 + id)

		slippiApiChan := make(chan *slippi.ApiResponse)
		go slippi.FetchApi(uid, slippiApiChan)

		client, err := enet.NewHost(enet.NewListenAddress(hostPort), 1, 3, 0, 0)

		if err != nil {
			fmt.Println("Error: Could not create client")
			results <- Result{Type: Err, AuthCode: "", Code: j.Code}
			continue
		}

		server, err := client.Connect(enet.NewAddress(SlippiHost, SlippiPort), 3, 0)

		if err != nil {
			fmt.Println("Error: Could not create server")
			results <- Result{Type: Err, AuthCode: "", Code: j.Code}
			continue
		}

		connect(&client, 20)

		slippiApiResponse := <-slippiApiChan

		if slippiApiResponse == nil {
			fmt.Println("Error: No response from slippi api")
			results <- Result{Type: Err, AuthCode: "", Code: j.Code}
			continue
		}

		authCode := slippiApiResponse.ConnectCode

		ticket := SlippiCreateTicket{
			Type: "create-ticket",
			User: SlippiUser{
				UID:         uid,
				PlayKey:     key,
				ConnectCode: slippiApiResponse.ConnectCode,
				DisplayName: slippiApiResponse.DisplayName,
			},
			Search: SlippiSearch{
				Mode:        2,
				ConnectCode: []byte(j.Code),
			},
			AppVersion:   slippiApiResponse.LatestVersion,
			IPAddressLAN: "127.0.0.1:" + strconv.FormatInt(int64(41000+id), 10),
		}

		log.Printf("[%s] searching for %s\n", authCode, j.Code)
		results <- Result{Type: Searching, AuthCode: authCode, Code: j.Code}

		ticket_bytes, err := json.Marshal(ticket)

		if err != nil {
			fmt.Println("Error:", err)
			results <- Result{Type: Err, AuthCode: authCode, Code: j.Code}
			continue
		}

		send(&server, ticket_bytes)

		var createTicketResp SlippiCreateTicketResp
		err = rcv(&client, &createTicketResp, 5000)

		if err != nil {
			fmt.Println("Error:", err)
			results <- Result{Type: Err, AuthCode: authCode, Code: j.Code}
			continue
		}

		var getTicketResp SlippiGetTicketResp
		err = rcv(&client, &getTicketResp, j.Timeout)

		if err != nil {
			fmt.Println("Error:", err)
			results <- Result{Type: Timeout, AuthCode: authCode, Code: j.Code}
			continue
		}

		var user SlippiPlayer

		for _, player := range getTicketResp.Players {
			if player.ConnectCode == j.Code {
				user = player
				results <- Result{Type: Success, AuthCode: authCode, Code: j.Code}
			}
		}

		disconnect(&client, &server)

		client, err = enet.NewHost(enet.NewListenAddress(hostPort), 10, 3, 0, 0)

		if err != nil {
			fmt.Println("Error:", err)
			continue
		}

		host, portStr, err := net.SplitHostPort(user.IpAddress)

		if err != nil {
			fmt.Println("Error:", err)
			continue
		}

		port, err := strconv.Atoi(portStr)
		if err != nil {
			fmt.Println("Error:", err)
			continue
		}

		opponent, err := client.Connect(enet.NewAddress(host, uint16(port)), 3, 0)

		if err != nil {
			fmt.Println("Error:", err)
			continue
		}

		connect(&client, 15)
		disconnect(&client, &opponent)
	}
}
