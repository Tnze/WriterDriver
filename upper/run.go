package main

import (
	"io"
	"log"
	"os"

	"github.com/tarm/serial"
)

func main() {
	c := &serial.Config{Name: "/dev/cu.SLAB_USBtoUART", Baud: 115200}
	s, err := serial.OpenPort(c)
	if err != nil {
		log.Fatal(err)
	}

	go io.Copy(os.Stdout, s) // 打印串口接收到到数据

	_, err = s.Write([]byte("G0 X0 Y0"))
	if err != nil {
		log.Fatal(err)
	}

	select {}
}
