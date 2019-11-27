// svg2gcode实现了从svg图片到gcode的转换
// 当前只转换svg中的path路径，并且只转换其中的l和c，
// 即线段和三次贝塞尔曲线，而且贝塞尔曲线没有做匀速化。
// 是一个非常原始简陋的转换器。
package main

import (
	"encoding/xml"
	"fmt"
	"io"
	"log"
	"math"
	"os"
	"strings"
)

var output io.Writer = os.Stdout

const mirror = true
const k = -0.05
const offsetX = 12
const offsetY = -16

func main() {
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

func findPath(path string) {
	// log.Print(path)
	r := strings.NewReader(path)

	type word struct {
		tk   byte
		x, y float64
	}

	ch := make(chan word)
	go func() { // 读取word并发送到ch
		for {
			var w word
			if _, err := fmt.Fscanf(r, "%c", &w.tk); err != nil {
				log.Fatal("parse path error1: ", err)
			}

			if w.tk == 'z' {
				break
			}

			if _, err := fmt.Fscanf(r, "%f,%f", &w.x, &w.y); err != nil {
				log.Fatal("parse path error2: ", err)
			}
			if mirror {
				w.x = -w.x
			}
			// 缩放
			w.x *= k
			w.y *= k
			// 交换x、y
			w.x, w.y = w.y, w.x

			// log.Printf("[%c] %.2f, %.2f", w.tk, w.x, w.y)
			ch <- w
		}
		close(ch)
	}()

	var x, y float64
	for v := range ch {
		switch v.tk {
		case 'm': // 起始
			fmt.Fprintf(output, "G0 X%f Y%f\n", v.x+offsetX, v.y+offsetY)
			x, y = v.x, v.y
		case 'l': // 直线
			fmt.Fprintf(output, "G1 X%f Y%f\n", v.x+x+offsetX, v.y+y+offsetY)
			x, y = v.x+x, v.y+y
		case 'c': // 三次贝塞尔
			b := [3]word{v, <-ch, <-ch}                                                                                            // 读入三组坐标(包括当前的一组)
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
			log.Panicf("unsupported path attr d[%c]", v.tk)
		}
	}
}
