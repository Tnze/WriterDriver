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
	const w, h = 1000, 1000
	const k = 5.0

	im := image.NewRGBA(image.Rectangle{Max: image.Point{X: w, Y: h}})
	gc := draw2dimg.NewGraphicContext(im)
	gc.SetFillColor(color.RGBA{0x00, 0x00, 0x00, 0x00})
	gc.SetStrokeColor(color.RGBA{0xFF, 0xFF, 0xFF, 0xff})
	gc.SetLineWidth(1)
	gc.BeginPath() // Initialize a new path

	scanner := bufio.NewScanner(os.Stdin)
	for scanner.Scan() {
		var g int
		var x, y float64
		fmt.Sscanf(scanner.Text(), "G%d X%f Y%f", &g, &x, &y)
		// fmt.Println(g, x, y)
		// im.Set(500+int(x*k), 500+int(y*k), color.Gray{Y: 255})

		if g == 0 {
			gc.MoveTo(500+y*k, 500+x*k)
		} else {
			gc.LineTo(500+y*k, 500+x*k)
		}
	}
	gc.Close()
	gc.FillStroke()

	draw2dimg.SaveToPngFile("test.png", im)
}
