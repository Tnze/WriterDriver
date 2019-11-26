package main

import (
	"bufio"
	"fmt"
	"image"
	"image/color"
	"image/png"
	"os"
)

func main() {
	const w, h = 582, 582
	const k = 5.0

	im := image.NewGray(image.Rectangle{Max: image.Point{X: w, Y: h}})
	scanner := bufio.NewScanner(os.Stdin)
	for scanner.Scan() {
		var g int
		var x, y float64
		fmt.Sscanf(scanner.Text(), "G%d X%f Y%f", &g, &x, &y)
		// fmt.Println(g, x, y)
		im.Set(int(x*k), int(y*k), color.Gray{Y: 255})
	}

	f, _ := os.OpenFile("test.png", os.O_WRONLY|os.O_CREATE, 0666)
	png.Encode(f, im)
	f.Close()
}
