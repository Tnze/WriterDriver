// svg2gcode实现了从svg图片到gcode的转换
// 当前只转换svg中的path路径，并且只转换其中的l和c，
// 即线段和三次贝塞尔曲线，而且贝塞尔曲线没有做匀速化。
// 是一个非常原始简陋的转换器。
package main

import (
	"bufio"
	"encoding/xml"
	"fmt"
	"io"
	"log"
	"math"
	"os"
	"strconv"
	"strings"
	"unicode"
	"unicode/utf8"
)

var output io.Writer = os.Stdout

const mirror = true
const k = -0.05
const offsetX = 12
const offsetY = -16

func main() {
	log.SetFlags(log.Lshortfile)
	var r io.Reader
	var err error
	if len(os.Args) < 2 {
		r = os.Stdin
	} else {
		r, err = os.Open(os.Args[1])
		if err != nil {
			log.Fatal("open file fail: ", err)
		}
	}

	decoder := xml.NewDecoder(r)
	for {
		token, err := decoder.Token()
		if err == io.EOF {
			break
		} else if err != nil {
			log.Fatal("decode token error: ", err)
		}

		if se, ok := token.(xml.StartElement); ok {
			switch se.Name.Local {
			case "path": // path标签
				for _, attr := range se.Attr {
					if attr.Name.Local == "d" {
						findPath(attr.Value)
					}
				}
			}
		}
	}

	fmt.Fprintf(output, "G0 X0 Y0\n")
}

type pathWord struct {
	tk   rune
	args []float64
}

func findPath(path string) {
	// log.Print(path)
	var r io.Reader = strings.NewReader(path)

	ch := make(chan pathWord)
	go readGcode(r, ch) // 读取word并发送到ch
	sendGcode(ch)       // 从ch读word并发送到串口
}

func readGcode(r io.Reader, cmd chan<- pathWord) (err error) {
	scanner := bufio.NewScanner(r)
	scanner.Split(scanPathCmd)

	for scanner.Scan() {
		var w pathWord

		word := scanner.Text()
		if len(word) != 1 {
			return fmt.Errorf("d should start with a single letter, but got %q", word)
		}

		w.tk = rune(word[0])
		argsN := arglen[w.tk]
		w.args = make([]float64, argsN)
		for i := 0; i < argsN; i++ {
			if !scanner.Scan() {
				return fmt.Errorf("args not enough of command[%c]", w.tk)
			}
			w.args[i], err = strconv.ParseFloat(scanner.Text(), 64)
			if err != nil {
				return
			}
		}

		// if mirror {
		// 	w.x = -w.x
		// }
		// // 缩放
		// w.x *= k
		// w.y *= k
		// // 交换x、y
		// w.x, w.y = w.y, w.x
		cmd <- w
	}
	close(cmd)
	return
}

var arglen = map[rune]int{
	'M': 2, 'm': 2,
	'L': 2, 'l': 2,
	'C': 6, 'c': 6,
}

func sendGcode(ch <-chan pathWord) {
	var x, y float64
	for v := range ch {
		// 把相对定位转换为绝对定位
		if unicode.IsLower(v.tk) {
			v.x += x
			v.y += y
			v.tk = unicode.ToUpper(v.tk)
		}
		switch v.tk {
		case 'M': // Move to
			fmt.Fprintf(output, "G0 X%f Y%f\n", v.x+offsetX, v.y+offsetY)
			x, y = v.x, v.y

		case 'L': // line to
			fmt.Fprintf(output, "G1 X%f Y%f\n", v.x+x+offsetX, v.y+y+offsetY)
			x, y = v.x, v.y

		case 'C': // curve to
			b := [3]pathWord{v, <-ch, <-ch}                                                                                        // 读入三组坐标(包括当前的一组)
			for t := 0.0; t <= 1; t += 1.0 / math.Min(10, math.Max(3, 0*math.Sqrt((x-b[2].x)*(x-b[2].x))+(y-b[2].y)*(y-b[2].y))) { // 估计曲线长度确定插值数
				c1 := (1 - t) * (1 - t) * (1 - t)
				c2 := 3 * t * (1 - t) * (1 - t)
				c3 := 3 * t * t * (1 - t)
				c4 := t * t * t

				bx := c1*x + c2*(x+b[0].x) + c3*(x+b[1].x) + c4*(x+b[2].x)
				by := c1*y + c2*(y+b[0].y) + c3*(y+b[1].y) + c4*(y+b[2].y)

				fmt.Fprintf(output, "G1 X%f Y%f\n", bx+offsetX, by+offsetY)
			}

			x, y = x+b[2].x, y+b[2].y
		default:
			log.Panicf("unsupported path cmd[%c]", v.tk)
		}
	}
}

// 扫描类似 M250 150 L150 350 L350 350 Z的数据
func scanPathCmd(data []byte, atEOF bool) (advance int, token []byte, err error) {
	// Skip leading spaces.
	start := 0
	for width := 0; start < len(data); start += width {
		var r rune
		r, width = utf8.DecodeRune(data[start:])
		if !unicode.IsSpace(r) && r != ',' {
			break
		}
	}
	if r, width := utf8.DecodeRune(data[start:]); unicode.IsLetter(r) {
		return start + width, data[start : start+width], nil
	}
	// Scan until space, marking end of word.
	for width, i := 0, start; i < len(data); i += width {
		var r rune
		r, width = utf8.DecodeRune(data[i:])
		if unicode.IsSpace(r) || r == ',' || unicode.IsLetter(r) {
			return i, data[start:i], nil
		}
	}
	// If we're at EOF, we have a final, non-empty, non-terminated word. Return it.
	if atEOF && len(data) > start {
		return len(data), data[start:], nil
	}
	// Request more data.
	return start, nil, nil
}
