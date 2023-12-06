package worker

type AuthClient struct {
	UID         string `json:"uid"`
	PlayKey     string `json:"playKey"`
	ConnectCode string `json:"connectCode"`
}

type Job struct {
	Code    string `json:"code"`
	Timeout int    `json:"timeout"`
}

type WorkerPool struct {
	Jobs    chan Job
	Results chan WorkerResult
}

func (wp *WorkerPool) Start(clients []AuthClient) {
	wp.Jobs = make(chan Job, len(clients))
	wp.Results = make(chan WorkerResult, len(clients))

	for id, w := range clients {
		go SlippiWorker(id, w.UID, w.PlayKey, wp.Jobs, wp.Results)
	}
}
