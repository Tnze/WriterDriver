package main

import (
	"io"
	"io/ioutil"
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

	if len(os.Args) > 1 {
		gcode, err := ioutil.ReadFile(os.Args[1])
		if err != nil {
			log.Fatal(err)
		}
		_, err = s.Write(gcode)
		if err != nil {
			log.Fatal(err)
		}
	}

	go io.Copy(s, os.Stdin)

	io.Copy(os.Stdout, s) // 打印串口接收到到数据
}
