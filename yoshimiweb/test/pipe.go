package main

import (
//	"bufio"
	"fmt"
	"log"
	"os"
//	"syscall"
	"time"
)

var yin = "/tmp/yin"
var yout = "/tmp/yout"

func main() {
	//os.Remove(pipeFile)
	//err := syscall.Mkfifo(pipeFile, 0666)
	//if err != nil {
	//	log.Fatal("Make named pipe file error:", err)
	//}
	//go scheduleWrite()
	fmt.Println("open a named pipe (y input) for write.")
	ofile, err := os.OpenFile(yin, os.O_RDWR|os.O_APPEND, os.ModeNamedPipe)
	if err != nil {
		log.Fatal("Open named pipe file error:", err)
	}

	fmt.Println("open a named pipe (y output) for read.")
	ifile, err := os.OpenFile(yout, os.O_RDONLY, os.ModeNamedPipe)
	if err != nil {
		log.Fatal("Open named pipe file error:", err)
	}

	ifile.SetReadDeadline(time.Now().Add(2000 * time.Millisecond))
	fmt.Println("clear input.")
	reply := make([]byte, 8196)
	goon := true
	for goon {

		l, err := ifile.Read(reply)
		fmt.Printf("\nxxx %d %s xxx %s",l,reply,err)
		if err != nil {
			goon = false
		}
	}


	eee := ifile.SetReadDeadline(time.Now().Add(100 * time.Millisecond))
	fmt.Printf("\neee %v \n",eee)


/*

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
*/




	fmt.Println("write string to y input.")
	ofile.WriteString("help\n")

	fmt.Println("read from y output.")

/*
	reader := bufio.NewReader(ifile)

	for {
		line, err := reader.ReadBytes('\n')
		if err == nil {
			fmt.Print("load string:" + string(line))
		}
	}
*/


	response := make([]byte, 0)

	goon = true
	for goon {

		l, err := ifile.Read(reply)
		response = append(response, reply[:l]...)
		fmt.Printf("\nxxx %d %s xxx %s",l,reply,err)
		if err != nil {
			goon = false
		}
	}

	fmt.Printf("from yoshimi: %s",response)


}
/*
func scheduleWrite() {
	fmt.Println("start schedule writing.")
	f, err := os.OpenFile(pipeFile, os.O_RDWR|os.O_CREATE|os.O_APPEND, 0777)
	if err != nil {
		log.Fatalf("error opening file: %v", err)
	}
	i := 0
	for {
		fmt.Println("write string to named pipe file.")
		f.WriteString(fmt.Sprintf("test write times:%d\n", i))
		i++
		time.Sleep(time.Second)
	}
}
*/
