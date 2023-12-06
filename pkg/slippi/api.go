package slippi

import (
	"encoding/json"
	"io"
	"net/http"
)

const ApiUri = "https://users-rest-dot-slippi.uc.r.appspot.com/user/"

type ApiResponse struct {
	UID           string `json:"uid"`
	DisplayName   string `json:"displayName"`
	ConnectCode   string `json:"connectCode"`
	LatestVersion string `json:"latestVersion"`
}

func FetchApi(uid string, result chan<- *ApiResponse) {
	resp, err := http.Get(ApiUri + uid)
	if err != nil {
		result <- nil
	}

	defer resp.Body.Close()
	body, err := io.ReadAll(resp.Body)

	if err != nil {
		result <- nil
	}

	s := ApiResponse{}

	err = json.Unmarshal(body, &s)

	if err != nil {
		result <- nil
	}

	result <- &s
}
