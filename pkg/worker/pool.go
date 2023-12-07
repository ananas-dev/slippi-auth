package worker

type ResultType uint

const (
	Success ResultType = iota
	Searching
	Timeout
	Err
)

type AuthClient struct {
	UID         string `json:"uid"`
	PlayKey     string `json:"playKey"`
	ConnectCode string `json:"connectCode"`
}

type Job struct {
	Code    string
	Timeout int
}

type Result struct {
	Type     ResultType
	AuthCode string
	Code     string
}

type WorkerPool struct {
	Jobs    chan Job
	Results chan Result
}

func (wp *WorkerPool) Start(clients []AuthClient) {
	wp.Jobs = make(chan Job, len(clients))
	wp.Results = make(chan Result, len(clients))

	for id, w := range clients {
		go SlippiWorker(id, w.UID, w.PlayKey, wp.Jobs, wp.Results)
	}
}
