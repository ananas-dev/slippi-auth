package ws

import (
	"encoding/json"
	"log"
	"net/http"
	"time"

	"github.com/ananas-dev/slippi-auth/pkg/worker"
	"github.com/gorilla/websocket"
)

const (
	writeWait      = 10 * time.Second
	pongWait       = 60 * time.Second
	pingPeriod     = (pongWait * 9) / 10
	maxMessageSize = 512
)

var newline = []byte{'\n'}

var upgrader = websocket.Upgrader{
	ReadBufferSize:  1024,
	WriteBufferSize: 1024,
	CheckOrigin: func(r *http.Request) bool {
		return true
	},
}

type Client struct {
	pool *worker.WorkerPool
	conn *websocket.Conn
	send chan []byte
}

type QueueMSG struct {
	Type    string `json:"type"`
	Code    string `json:"code"`
	Timeout int    `json:"timeout"`
}

type SearchingMSG struct {
	Type     string `json:"type"`
	AuthCode string `json:"auth-code"`
	Code     string `json:"code"`
}

type OutcomeMSG struct {
	Type string `json:"type"`
	Code string `json:"code"`
}

func (c *Client) readPump() {
	defer func() {
		// Maybe cancel every auth here
		c.conn.Close()
	}()
	c.conn.SetReadLimit(maxMessageSize)
	c.conn.SetReadDeadline(time.Now().Add(pongWait))
	c.conn.SetPongHandler(func(string) error { c.conn.SetReadDeadline(time.Now().Add(pongWait)); return nil })
	for {
		_, bytes, err := c.conn.ReadMessage()
		if err != nil {
			if websocket.IsUnexpectedCloseError(err, websocket.CloseGoingAway, websocket.CloseAbnormalClosure) {
				log.Printf("Error: %v", err)
			}
			break
		}

		msg := QueueMSG{}

		err = json.Unmarshal(bytes, &msg)

		if err != nil {
			log.Printf("Error: Invalid msg")
		}

		switch msg.Type {
		case "queue":
			select {
			case c.pool.Jobs <- worker.Job{Code: msg.Code, Timeout: msg.Timeout}:
				// Good
			default:
				log.Println("Error: Clients full")
				w, err := c.conn.NextWriter(websocket.TextMessage)
				if err != nil {
					return
				}
				out, _ := json.Marshal(&OutcomeMSG{Type: "full", Code: msg.Code})
				w.Write(out)
			}
		default:
			log.Printf("Error: Invalid msg")
		}
	}
}

func (c *Client) writePump() {
	ticker := time.NewTicker(pingPeriod)
	defer func() {
		ticker.Stop()
		c.conn.Close()
	}()
	for {
		select {
		case msg, ok := <-c.pool.Results:
			c.conn.SetWriteDeadline(time.Now().Add(writeWait))
			if !ok {
				c.conn.WriteMessage(websocket.CloseMessage, []byte{})
				return
			}

			w, err := c.conn.NextWriter(websocket.TextMessage)
			if err != nil {
				return
			}

			var out []byte

			switch msg.Type {
			case worker.Searching:
				out, _ = json.Marshal(&SearchingMSG{Type: "searching", AuthCode: msg.AuthCode, Code: msg.Code})
			case worker.Err:
				out, _ = json.Marshal(&OutcomeMSG{Type: "err", Code: msg.Code})
			case worker.Success:
				out, _ = json.Marshal(&OutcomeMSG{Type: "success", Code: msg.Code})
			case worker.Timeout:
				out, _ = json.Marshal(&OutcomeMSG{Type: "timeout", Code: msg.Code})
			}

			w.Write(out)

			// Add queued chat messages to the current websocket message.
			n := len(c.send)
			for i := 0; i < n; i++ {
				w.Write(newline)
				w.Write(<-c.send)
			}

			if err := w.Close(); err != nil {
				return
			}
		case <-ticker.C:
			c.conn.SetWriteDeadline(time.Now().Add(writeWait))
			if err := c.conn.WriteMessage(websocket.PingMessage, nil); err != nil {
				return
			}
		}
	}
}

func ServeWs(pool *worker.WorkerPool, w http.ResponseWriter, r *http.Request) {
	conn, err := upgrader.Upgrade(w, r, nil)
	if err != nil {
		log.Println(err)
		return
	}
	client := &Client{pool: pool, conn: conn, send: make(chan []byte, 256)}

	go client.writePump()
	go client.readPump()
}
