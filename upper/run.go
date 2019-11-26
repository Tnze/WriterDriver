package main

import (
	"fmt"
	"io"
	"io/ioutil"
	"log"
	"math"
	"os"
	"strings"
	"time"

	"github.com/tarm/serial"
)

func main() {
	c := &serial.Config{Name: "/dev/cu.SLAB_USBtoUART", Baud: 115200}
	s, err := serial.OpenPort(c)
	if err != nil {
		log.Fatal(err)
	}
	go func() {
		if len(os.Args) > 1 {
			gcode, err := ioutil.ReadFile(os.Args[1])
			if err != nil {
				log.Fatal(err)
			}
			for _, v := range strings.Split(string(gcode), "\n") {
				var (
					g    int
					x, y float64
				)
				fmt.Sscanf(v, "G%d X%f Y%f", &g, &x, &y)
				sleeptime := time.Millisecond*20 + time.Duration(float64(time.Millisecond)*20*math.Sqrt(x*x+y*y))
				fmt.Printf("---%v-->%q (%f, %f)\n", sleeptime, v, x, y)
				time.Sleep(sleeptime)
				_, err = s.Write([]byte(v + "\n"))
				if err != nil {
					log.Fatal(err)
				}

			}
		}

		io.Copy(s, os.Stdin)
	}()

	io.Copy(os.Stdout, s) // 打印串口接收到到数据
}
