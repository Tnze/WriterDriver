package main

import (
	"bufio"
	"fmt"
	"io"
	"log"
	"os"
	"strings"

	"github.com/mattn/go-colorable"
	"github.com/tarm/serial"
)

const (
	norm   = "\033[0m"
	yellow = "\033[33m"
	green  = "\033[32m"
	blue   = "\033[34m"
	red    = "\033[31m"
)

var sendLogger = log.New(colorable.NewColorableStdout(), yellow+"<--- ", log.Ltime|log.Lmicroseconds)
var recvLogger = log.New(colorable.NewColorableStdout(), blue+"---> ", log.Ltime|log.Lmicroseconds)
var erroLogger = log.New(colorable.NewColorableStdout(), red+"-!!- ", log.Ltime|log.Lmicroseconds)
var normLogger = log.New(colorable.NewColorableStdout(), norm+"---- ", log.Ltime|log.Lmicroseconds)
var succLogger = log.New(colorable.NewColorableStdout(), green+"-ok- ", log.Ltime|log.Lmicroseconds)

var okWaiter = make(chan string, 5) // 命令确认通道，容量决定了发送到下位机的最多命令数

func main() {

	c := &serial.Config{Name: "/dev/cu.SLAB_USBtoUART", Baud: 115200}
	s, err := serial.OpenPort(c)
	if err != nil {
		normLogger.Fatal(err)
	}
	go func() {
		// 发送文件
		if len(os.Args) > 1 {
			f, err := os.Open(os.Args[1])
			if err != nil {
				sendLogger.Fatal("open file error: ", err)
			}
			scanner := bufio.NewScanner(f)
			for scanner.Scan() {
				gcode := scanner.Bytes()
				if len(gcode) > 0 && gcode[0] == 'G' {
					okWaiter <- string(gcode)
				}
				if err := sendLine(s, gcode); err != nil {
					sendLogger.Fatal(err)
				}
			}
		}

		scanner := bufio.NewScanner(os.Stdin)
		for scanner.Scan() {
			line := scanner.Bytes()

			if len(line) > 0 && line[0] == 'G' {
				okWaiter <- string(line)
			}
			if err := sendLine(s, line); err != nil {
				sendLogger.Fatal(err)
			}
		}
		if err := scanner.Err(); err != nil {
			sendLogger.Fatal(err)
		}
	}()

	// 打印串口接收到到数据
	r := bufio.NewReader(s)
	for {
		line, _, err := r.ReadLine()
		if err != nil {
			recvLogger.Fatal(err)
		}
		strline := string(line)

		if strings.HasPrefix(strline, "//") {
			strline = norm + strline
		} else if strings.HasSuffix(strline, "ok") {
			<-okWaiter
			strline = green + strline
		} else if strings.HasSuffix(strline, "rs") {
			strline = red + strline
		}

		recvLogger.Print(strline)
	}
}

var lastLineCode int

func sendLine(s io.Writer, gcode []byte) error {
	prefix := fmt.Sprintf("N%d %s", lastLineCode+1, gcode)
	lastLineCode++
	// 计算校验码
	var cs byte
	for i := range prefix {
		cs = cs ^ prefix[i]
	}
	pl := fmt.Sprintf("%s*%d\n", prefix, cs)
	sendLogger.Print(pl)
	_, err := s.Write([]byte(pl))
	return err
}
