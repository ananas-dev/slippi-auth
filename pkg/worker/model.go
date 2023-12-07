package worker

type SlippiCreateTicket struct {
	Type         string       `json:"type"`
	User         SlippiUser   `json:"user"`
	Search       SlippiSearch `json:"search"`
	AppVersion   string       `json:"appVersion"`
	IPAddressLAN string       `json:"ipAddressLan"`
}

type SlippiUser struct {
	UID         string `json:"uid"`
	PlayKey     string `json:"playKey"`
	ConnectCode string `json:"connectCode"`
	DisplayName string `json:"displayName"`
}

type SlippiSearch struct {
	Mode        int    `json:"mode"`
	ConnectCode []byte `json:"connectCode"`
}

type SlippiCreateTicketResp struct {
	Type string `json:"type"`
}

type SlippiGetTicketResp struct {
	Type       string         `json:"type"`
	MatchId    string         `json:"matchId"`
	IsAssigned bool           `json:"isAssigned"`
	IsHost     bool           `json:"isHost"`
	Players    []SlippiPlayer `json:"players"`
	Stages     []int          `json:"stages"`
}

type SlippiPlayer struct {
	UID           string   `json:"uid"`
	PlayKey       string   `json:"playKey"`
	ConnectCode   string   `json:"connectCode"`
	DisplayName   string   `json:"displayName"`
	IpAddress     string   `json:"ipAddress"`
	IpAddressLan  string   `json:"ipAddressLan"`
	Port          uint16   `json:"port"`
	IsLocalPlayer bool     `json:"isLocalPlayer"`
	ChatMessages  []string `json:"chatMessages"`
}
