package main

import (
	"fmt"
	"net"
	"net/http"
	"strings"
	"time"
)

func commandHandler(w http.ResponseWriter, r *http.Request) {
	command := strings.TrimPrefix(r.URL.Path, "/command/")
	port := strings.TrimPrefix(r.URL.RawQuery, "fsport=")
	response, _ := fluidsynthCommand("localhost:"+port, command)
	fmt.Fprint(w, string(response))
}

func main() {
	http.HandleFunc("/command/", commandHandler)
	http.Handle("/", http.FileServer(http.Dir("/opt/lib/fluidweb/www")))
	http.ListenAndServe(":9999", nil)
}

func fluidsynthCommand(servAddr string, command string) ([]byte, error) {
	tcpAddr, err := net.ResolveTCPAddr("tcp", servAddr)
	if err != nil {
		return nil, err
	}

	conn, err := net.DialTCP("tcp", nil, tcpAddr)
	if err != nil {
		return nil, err
	}
	conn.SetReadDeadline(time.Now().Add(10 * time.Millisecond))

	_, err = conn.Write([]byte(command + "\n"))
	if err != nil {
		return nil, err
	}

	response := make([]byte, 0)
	reply := make([]byte, 8196)

	goon := true
	for goon {

		l, err := conn.Read(reply)
		response = append(response, reply[:l]...)
		if err != nil {
			goon = false
		}

	}

	conn.Close()
	return response, nil
}
