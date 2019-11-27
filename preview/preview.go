package main

import (
	"bufio"
	"fmt"
	"image"
	"image/color"
	"os"

	"github.com/llgcode/draw2d/draw2dimg"
)

func main() {
	const w, h = 800, 600
	const k = 20.0

	im := image.NewRGBA(image.Rectangle{Max: image.Point{X: w, Y: h}})
	gc := draw2dimg.NewGraphicContext(im)
	gc.SetFillColor(color.RGBA{0x00, 0x00, 0x00, 0x00})
	gc.SetStrokeColor(color.RGBA{0xAA, 0xAA, 0xAA, 0xFF})
	gc.SetLineWidth(1)
	gc.BeginPath() // Initialize a new path

	// 绘制坐标轴
	gc.MoveTo(0, h/2)
	gc.LineTo(w, h/2)
	gc.MoveTo(w/2, 0)
	gc.LineTo(w/2, h)

	scanner := bufio.NewScanner(os.Stdin)
	for scanner.Scan() {
		var g int
		var x, y float64
		fmt.Sscanf(scanner.Text(), "G%d X%f Y%f", &g, &x, &y)
		// fmt.Println(g, x, y)

		if g == 0 {
			gc.MoveTo(w/2+y*k, h/2+x*k)
		} else {
			gc.LineTo(w/2+y*k, h/2+x*k)
		}

		im.SetRGBA(int(w/2+y*k), int(h/2+x*k), color.RGBA{0xFF, 0xFF, 0xFF, 0xff})
	}
	gc.Close()
	gc.FillStroke()

	draw2dimg.SaveToPngFile("test.png", im)
}
