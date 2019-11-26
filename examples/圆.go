package main

import (
	"fmt"
	"math"
)

func main() {
	r := 10.0
	g := 0
	for i := 0.0; i < 2*math.Pi; i += 2 * math.Pi / 64 {
		fmt.Printf("G%d X%.2f Y%.2f\n", g, math.Sin(i)*r, math.Cos(i)*r)
		g = 1
	}
	// fmt.Printf("G1 X%.2f Y%.2f\n", math.Sin(i)*r, math.Cos(i)*r)
	fmt.Printf("G0 X0 Y0\n")
}
